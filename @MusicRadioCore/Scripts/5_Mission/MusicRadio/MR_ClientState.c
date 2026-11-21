// ============================================================================
//  MusicRadio - Core
//  MR_ClientState.c : what a client knows about the server radio world.
//  Layer: 5_Mission
// ============================================================================

class MR_ClientState
{
	protected static ref MR_Settings m_Settings;
	protected static ref array<string> m_Sessions;     // "type|idLow|idHigh|title|provider"
	protected static ref MR_DeviceBase m_OpenHost;     // host the menu was opened on
	protected static string m_LastMessage;

	static MR_Library Library()
	{
		return MR_LibraryIndex.Get();
	}

	static MR_Settings Settings()
	{
		if (!m_Settings)
			m_Settings = MR_Config.LoadClient();
		return m_Settings;
	}

	static void SetSettings(MR_Settings s)
	{
		if (s)
		{
			m_Settings = s;
			MR_ConfigRuntime.Set(s);
			MR_Config.SaveClient(s);
		}
	}

	static void SetOpenHost(MR_DeviceBase host)
	{
		m_OpenHost = host;
	}

	static MR_DeviceBase GetOpenHost()
	{
		return m_OpenHost;
	}

	static void SetMessage(string msg)
	{
		m_LastMessage = msg;
		MR.Log("UI: " + msg);
	}

	static string GetMessage()
	{
		return m_LastMessage;
	}

	// Rebuild the local library from the manifest the server pushed.
	static void ApplyManifest(string manifest)
	{
		MR_Library lib = Library();
		MR_JsonValue root = MR_Json.Parse(manifest);
		if (!root)
		{
			MR.LogWarn("Cannot parse library manifest");
			return;
		}

		array<ref MR_JsonValue> entries = root.GetArray("tracks");
		if (!entries)
			return;

		array<ref MR_Track> tracks = lib.All();
		tracks.Clear();
		lib.ClearIndex();

		int resolved = 0;
		int broken = 0;

		foreach (MR_JsonValue entry : entries)
		{
			MR_Track t = new MR_Track();
			t.m_Id        = entry.GetString("id");
			t.m_Provider  = entry.GetString("p");
			t.m_Kind      = entry.GetString("k", "music");
			t.m_Title     = entry.GetString("t");
			t.m_Artist    = entry.GetString("a");
			t.m_AlbumId   = entry.GetString("al");
			t.m_Duration  = entry.GetInt("d", 0);
			t.m_MediaKind = MR.MediaKindFromInt(entry.GetInt("m", 0));

			// Validate against the media packs actually installed on this client.
			if (t.m_MediaKind == MR_MediaKind.MEDIA_TRACK)
			{
				string err;
				string ss = MR_MediaResolver.ResolveTrack(t, err);
				if (ss == string.Empty)
				{
					broken = broken + 1;
					t.m_ResolveError = err;
				}
				else
					resolved = resolved + 1;
			}
			else
				resolved = resolved + 1;

			tracks.Insert(t);
		}

		lib.Reindex();

		MR.Log("Library synced: " + tracks.Count().ToString() + " entries, " + resolved.ToString() + " playable, " + broken.ToString() + " unavailable");

		if (broken > 0)
			MR_ClientState.SetMessage(broken.ToString() + " track(s) need a media pack update");
	}

	static void SetSessions(array<string> sessions)
	{
		m_Sessions = sessions;
	}

	static array<string> GetSessions()
	{
		return m_Sessions;
	}
}
