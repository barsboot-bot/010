// ============================================================================
//  MusicRadio - Core
//  MR_MediaResolver.c : locator -> SoundSet resolution, per platform.
//  Layer: 3_Game
//
//  THIS IS THE CONTRACT BETWEEN THE THREE MODS
//  -------------------------------------------
//  Mod 1 (server) and Mod 2 (logic) contain NO audio asset paths.
//  Only the media pack (mod 3) owns files, and it must expose each playable
//  item as a CfgSoundSets entry named:
//
//        MR_<PROVIDER>_<TRACKID>_SoundSet
//
//  Provider and track id are upper cased and stripped of anything that is not
//  [A-Z0-9_]. Example:
//        provider "MusicRadioDB", id "wasteland_01"
//        ->  MR_MUSICRADIODB_WASTELAND_01_SoundSet
//
//  A pack may instead declare an explicit "soundset" field per track, which
//  wins over the convention. Either way the CLIENT validates the name with
//  ConfigIsExisting() before anything is played: a missing or misnamed entry
//  degrades to a chat warning instead of a silent failure.
//
//  Only local .ogg media is supported. Streaming (relay / recorded chunks /
//  external Icecast) was evaluated and dropped from the mod: it always needed
//  either extra software on the player PC or a constantly republished pack.
//  The full analysis and the code are kept in _archive/internet_radio/.
// ============================================================================

class MR_MediaResolver
{
	// Build the canonical SoundSet name for a track inside a pack.
	static string BuildSoundSetName(string provider, string trackId)
	{
		return "MR_" + Normalize(provider) + "_" + Normalize(trackId) + "_SoundSet";
	}

	static string Normalize(string input)
	{
		string outS = input;
		outS.ToUpper();
		outS.Replace(" ", "_");
		outS.Replace("-", "_");
		outS.Replace(".", "_");
		outS.Replace("/", "_");
		outS.Replace("\\", "_");

		// Drop anything that is not A-Z 0-9 _ (keeps config class names legal)
		string clean = string.Empty;
		for (int i = 0; i < outS.Length(); i++)
		{
			string ch = outS.Substring(i, 1);
			if (MR_MediaResolver.IsNameChar(ch))
				clean = clean + ch;
		}
		return clean;
	}

	static bool IsNameChar(string ch)
	{
		if (ch == "_")
			return true;
		// ASCII ranges: 0-9 / A-Z
		if (ch >= "0" && ch <= "9")
			return true;
		if (ch >= "A" && ch <= "Z")
			return true;
		return false;
	}

	// ------------------------------------------------------------------ tracks
	// Resolve local media on the CLIENT. Returns "" when not playable here.
	static string ResolveTrack(MR_Track track, out string error)
	{
		error = string.Empty;

		if (!track)
		{
			error = "no track";
			return string.Empty;
		}

		string candidate = track.m_SoundSet;
		if (candidate == string.Empty)
			candidate = BuildSoundSetName(track.m_Provider, track.m_Id);

		if (!SoundSetExists(candidate))
		{
			// A pack might use its own naming for a whole folder; the track
			// may still declare "file" - in that case try the file based name
			// as a second chance before giving up.
			string alt = string.Empty;
			if (track.m_File != string.Empty)
				alt = BuildSoundSetName(track.m_Provider, track.m_File);

			if (alt != string.Empty && SoundSetExists(alt))
				candidate = alt;
			else
			{
				error = "soundset missing: " + candidate + " (is the media pack installed on the client?)";
				return string.Empty;
			}
		}

		track.m_Resolved = true;
		track.m_ResolvedSoundSet = candidate;
		return candidate;
	}

	static bool SoundSetExists(string name)
	{
		if (name == string.Empty)
			return false;
		return GetGame().ConfigIsExisting("CfgSoundSets " + name);
	}

	// Is this provider (media pack) present on the client at all?
	static bool ProviderInstalled(string provider)
	{
		string probe = "MR_" + Normalize(provider) + "_";
		// Cheap heuristic: any class in CfgSoundSets starting with the prefix.
		// ConfigGetChildrenCount avoids loading the full class list.
		string path = "CfgSoundSets";
		int count = GetGame().ConfigGetChildrenCount(path);
		for (int i = 0; i < count; i++)
		{
			string child = string.Empty;
			if (GetGame().ConfigGetChildName(path, i, child))
			{
				if (MR.StartsWith(child, probe))
					return true;
			}
		}
		return false;
	}
}
