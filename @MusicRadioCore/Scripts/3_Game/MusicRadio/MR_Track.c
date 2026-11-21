// ============================================================================
//  MusicRadio - Core
//  MR_Track.c : one entry of the media library (a local .ogg track).
//  Layer: 3_Game
// ============================================================================

class MR_Track
{
	// --- identity -----------------------------------------------------------
	string          m_Id;            // stable id, e.g. "wasteland_01"
	string          m_Provider;      // pack that owns it, e.g. "MusicRadioDB"
	string          m_Kind;          // "music" | "ambient" | "sfx"
	string          m_Title;
	string          m_Artist;
	string          m_Album;        // display name ("Demo Album")
	string          m_AlbumId;      // album key = cassette/MD class suffix ("rock01")
	int             m_Year;
	int             m_Duration;      // seconds, 0 = unknown
	autoptr array<string> m_Tags;    // "rock","chill","night" ...

	// --- audio source -------------------------------------------------------
	// m_File: relative path inside the pack ("Music\\rock\\track01.ogg"),
	//         converted to a SoundSet name by MR_MediaResolver.
	MR_MediaKind    m_MediaKind;
	string          m_File;
	string          m_SoundSet;      // optional explicit override

	// --- playback properties ------------------------------------------------
	float           m_DefaultVolume; // 0..1
	bool            m_Loop;          // loop single track
	bool            m_Enabled;       // admins can disable one entry
	bool            m_Explicit;      // for a "no swearing" server filter
	int             m_Bitrate;       // kbps, informational (media metadata only)
	string          m_Cover;         // optional image name inside the pack

	// --- validation state (filled by the resolver) --------------------------
	bool            m_Resolved;
	string          m_ResolvedSoundSet;
	string          m_ResolveError;

	void MR_Track()
	{
		m_Id             = string.Empty;
		m_Provider       = string.Empty;
		m_Kind           = "music";
		m_Title          = string.Empty;
		m_Artist         = string.Empty;
		m_Album          = string.Empty;
		m_AlbumId        = string.Empty;
		m_Year           = 0;
		m_Duration       = 0;
		m_Tags           = new array<string>();
		m_MediaKind      = MR_MediaKind.MEDIA_NONE;
		m_File           = string.Empty;
		m_SoundSet       = string.Empty;
		m_DefaultVolume  = 0.8;
		m_Loop           = false;
		m_Enabled        = true;
		m_Explicit       = false;
		m_Bitrate        = 0;
		m_Cover          = string.Empty;
		m_Resolved       = false;
		m_ResolvedSoundSet = string.Empty;
		m_ResolveError   = string.Empty;
	}

	bool IsPlayable()
	{
		if (!m_Enabled)
			return false;
		if (m_MediaKind == MR_MediaKind.MEDIA_TRACK)
			return (m_File != string.Empty || m_SoundSet != string.Empty);
		return false;
	}

	// "Artist - Title" for the UI / chat / now playing feed.
	string DisplayName()
	{
		if (m_Artist != string.Empty && m_Title != string.Empty)
			return m_Artist + " - " + m_Title;
		if (m_Title != string.Empty)
			return m_Title;
		if (m_Id != string.Empty)
			return m_Id;
		return "unknown";
	}

	string DurationText()
	{
		if (m_Duration <= 0)
			return "--:--";
		int m = m_Duration / 60;
		int s = m_Duration % 60;
		return m.ToString() + ":" + MR.Pad2(s);
	}

	// ------------------------------------------------------------------ parse
	// Accepts both the compact form  { "id":"x", "artist":"y" ... }
	// and the "array of strings" shorthand for tags.
	static MR_Track FromJson(MR_JsonValue obj, string provider)
	{
		if (!obj || obj.m_Type != MR_JsonValue.T_OBJECT)
			return null;

		MR_Track t = new MR_Track();
		t.m_Provider = provider;

		t.m_Id       = obj.GetString("id");
		t.m_Kind     = obj.GetString("kind", "music");
		t.m_Title    = obj.GetString("title");
		t.m_Artist   = obj.GetString("artist");
		t.m_Album    = obj.GetString("album");
		t.m_AlbumId  = obj.GetString("albumId");
		t.m_Year     = obj.GetInt("year", 0);
		t.m_Duration = obj.GetInt("duration", 0);
		t.m_File     = obj.GetString("file");
		t.m_SoundSet = obj.GetString("soundset");
		t.m_Cover    = obj.GetString("cover");
		t.m_Bitrate  = obj.GetInt("bitrate", 0);
		t.m_Enabled  = obj.GetBool("enabled", true);
		t.m_Explicit = obj.GetBool("explicit", false);
		t.m_Loop     = obj.GetBool("loop", false);
		t.m_DefaultVolume = obj.GetFloat("volume", 0.8);

		// Durations are commonly written "3:42" by humans.
		MR_JsonValue durJson = obj.Get("duration");
		if (durJson && durJson.m_Type == MR_JsonValue.T_STRING)
			t.m_Duration = MR_Track.ParseTime(durJson.m_Str);

		// Tags: array of strings or a single comma separated string.
		array<ref MR_JsonValue> tagsArr = obj.GetArray("tags");
		if (tagsArr)
		{
			foreach (MR_JsonValue tv : tagsArr)
			{
				string tag = tv.AsString();
				if (tag != string.Empty)
					t.m_Tags.Insert(tag);
			}
		}
		else
		{
			MR_JsonValue tagsOne = obj.Get("tags");
			if (tagsOne && tagsOne.m_Type == MR_JsonValue.T_STRING)
			{
				TStringArray parts = new TStringArray();
				tagsOne.m_Str.Split(",", parts);
				foreach (string p : parts)
				{
					string trim = MR_Track.Trim(p);
					if (trim != string.Empty)
						t.m_Tags.Insert(trim);
				}
			}
		}

		// Every entry is a local .ogg from a media pack.
		if (t.m_File != string.Empty || t.m_SoundSet != string.Empty)
			t.m_MediaKind = MR_MediaKind.MEDIA_TRACK;

		// An id is mandatory; synthesise one from the title when missing so a
		// sloppy entry still works, but warn the pack author.
		if (t.m_Id == string.Empty)
		{
			t.m_Id = MR_Track.Slugify(t.DisplayName());
			MR.LogWarn("Track without id in provider '" + provider + "', generated: " + t.m_Id);
		}

		if (t.m_Title == string.Empty)
			t.m_Title = t.m_Id;

		return t;
	}

	string ToJson()
	{
		string tags = "\"tags\":[";
		for (int i = 0; i < m_Tags.Count(); i++)
		{
			if (i > 0) tags = tags + ",";
			tags = tags + "\"" + MR_Track.Escape(m_Tags.Get(i)) + "\"";
		}
		tags = tags + "]";

		string s = "{";
		s = s + "\"id\":\"" + MR_Track.Escape(m_Id) + "\",";
		s = s + "\"provider\":\"" + MR_Track.Escape(m_Provider) + "\",";
		s = s + "\"kind\":\"" + MR_Track.Escape(m_Kind) + "\",";
		s = s + "\"title\":\"" + MR_Track.Escape(m_Title) + "\",";
		s = s + "\"artist\":\"" + MR_Track.Escape(m_Artist) + "\",";
		s = s + "\"album\":\"" + MR_Track.Escape(m_Album) + "\",";
		s = s + "\"albumId\":\"" + MR_Track.Escape(m_AlbumId) + "\",";
		s = s + "\"year\":" + m_Year.ToString() + ",";
		s = s + "\"duration\":" + m_Duration.ToString() + ",";
		s = s + tags + ",";
		s = s + "\"file\":\"" + MR_Track.Escape(m_File) + "\",";
		s = s + "\"soundset\":\"" + MR_Track.Escape(m_SoundSet) + "\",";
		s = s + "\"volume\":" + m_DefaultVolume.ToString() + ",";
		s = s + "\"bitrate\":" + m_Bitrate.ToString() + ",";
		s = s + "\"enabled\":" + MR_Track.BoolJson(m_Enabled) + ",";
		s = s + "\"explicit\":" + MR_Track.BoolJson(m_Explicit);
		s = s + "}";
		return s;
	}

	// ------------------------------------------------------------------ utils
	static int ParseTime(string text)
	{
		if (text == string.Empty)
			return 0;

		TStringArray parts = new TStringArray();
		text.Split(":", parts);

		if (parts.Count() == 1)
			return parts.Get(0).ToInt();
		if (parts.Count() == 2)
			return parts.Get(0).ToInt() * 60 + parts.Get(1).ToInt();
		if (parts.Count() >= 3)
			return parts.Get(0).ToInt() * 3600 + parts.Get(1).ToInt() * 60 + parts.Get(2).ToInt();

		return 0;
	}

	static string Trim(string s)
	{
		string outS = s;
		outS.Trim();
		return outS;
	}

	static string Slugify(string s)
	{
		string outS = s;
		outS.ToLower();
		outS.Replace(" ", "_");
		outS.Replace(".", "");
		outS.Replace(",", "");
		outS.Replace("-", "_");
		outS.Replace("'", "");
		return outS;
	}

	static string Escape(string s)
	{
		string outS = s;
		outS.Replace("\\", "\\\\");
		outS.Replace("\"", "\\\"");
		return outS;
	}

	static string BoolJson(bool b)
	{
		if (b) return "true";
		return "false";
	}
}
