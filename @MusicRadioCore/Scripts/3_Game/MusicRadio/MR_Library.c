// ============================================================================
//  MusicRadio - Core
//  MR_Library.c : media library = tracks from every installed media pack.
//  Layer: 3_Game
//
//  The library is built on BOTH sides, from the same data files:
//    * server: authoritative copy, used for validation and for sending the
//              library to late joiners ("what exists at all")
//    * client: resolved copy, used to translate a locator into a SoundSet
//
//  Because the media pack ships the .ogg files AND the json index, and both are
//  part of the same workshop item, the two copies stay in sync by construction.
// ============================================================================

class MR_Library
{
	protected ref array<ref MR_Track> m_Tracks;
	protected ref map<string, ref MR_Track> m_ById;

	// Diagnostics
	string m_LastError;
	int    m_LoadedFiles;
	int    m_SkippedEntries;

	void MR_Library()
	{
		m_Tracks = new array<ref MR_Track>();
		m_ById   = new map<string, ref MR_Track>();
		m_LastError = string.Empty;
		m_LoadedFiles = 0;
		m_SkippedEntries = 0;
	}

	// ------------------------------------------------------------------ load
	// Loads every music/**.json found in the media pack. On a dedicated server
	// the pack folder is resolved through the mission's mod list; on the client
	// the same files are read from the mounted PBO - FileExist() sees both.
	void LoadAll()
	{
		m_Tracks.Clear();
		m_ById.Clear();
		m_LoadedFiles = 0;
		m_SkippedEntries = 0;

		// Standard layout produced by the DB mod:
		//   MusicRadioDB/Music/index.json            <- always present, all packs merged
		//   MusicRadioDB/Music/<Genre>/<pack>.json   <- per genre / per playlist
		// The index alone is enough; the per-genre files are loaded too so that a
		// pack can be dropped in without regenerating the index.
		string baseDir = MR.PREFIX_DB + "/Music";

		TStringArray dirs = new TStringArray();
		MR_FileUtils.CollectDirs(baseDir, dirs);

		int enumerated = 0;
		foreach (string sub : dirs)
		{
			string dir = baseDir + "/" + sub;
			TStringArray files = new TStringArray();
			MR_FileUtils.CollectJsonFiles(dir, files);
			foreach (string file : files)
			{
				LoadFile(file);
				enumerated = enumerated + 1;
			}
		}

		// Root level files (index.json lives here)
		TStringArray rootFiles = new TStringArray();
		MR_FileUtils.CollectJsonFiles(baseDir, rootFiles);
		foreach (string rootFile : rootFiles)
		{
			LoadFile(rootFile);
			enumerated = enumerated + 1;
		}

		if (enumerated == 0)
		{
			// Directory enumeration inside a PBO can be restricted on some
			// builds; the merged index is the documented fallback.
			MR.LogWarn("No media files enumerated, trying the merged index only.");
			LoadFile(baseDir + "/index.json");
		}

		MR.Log("Library loaded: " + m_Tracks.Count().ToString() + " entries from " + m_LoadedFiles.ToString() + " files (skipped " + m_SkippedEntries.ToString() + ")");
	}

	void LoadFile(string path)
	{
		MR_JsonValue root = MR_Json.ParseFile(path);
		if (!root)
		{
			m_LastError = "cannot parse " + path;
			return;
		}

		string provider = root.GetString("provider");
		if (provider == string.Empty)
			provider = MR.PREFIX_DB;

		array<ref MR_JsonValue> entries = null;

		// Two accepted shapes:
		//   { "provider":"X", "tracks":[ {...}, {...} ] }
		//   { "provider":"X", "tracks":{ "1":{...}, "2":{...} } }
		MR_JsonValue tracksNode = root.Get("tracks");
		if (tracksNode)
			entries = tracksNode.CollectValues();
		else
			entries = root.CollectValues();   // file is just a list

		if (!entries || entries.Count() == 0)
		{
			MR.LogWarn("No entries in " + path);
			return;
		}

		m_LoadedFiles = m_LoadedFiles + 1;

		foreach (MR_JsonValue entry : entries)
		{
			MR_Track t = MR_Track.FromJson(entry, provider);
			if (!t)
			{
				m_SkippedEntries = m_SkippedEntries + 1;
				continue;
			}

			if (m_ById.Contains(t.m_Id))
			{
				// Same id in two packs: first one wins, report it once.
				MR.LogWarn("Duplicate track id '" + t.m_Id + "' in " + path + " (ignored)");
				m_SkippedEntries = m_SkippedEntries + 1;
				continue;
			}

			m_ById.Set(t.m_Id, t);
			m_Tracks.Insert(t);
		}
	}

	// ---------------------------------------------------------------- queries
	int Count()
	{
		return m_Tracks.Count();
	}

	// Rebuild the id index after the track array was modified from outside
	// (the client does that when the server manifest arrives).
	void Reindex()
	{
		if (!m_ById)
			m_ById = new map<string, ref MR_Track>();
		m_ById.Clear();
		foreach (MR_Track t : m_Tracks)
		{
			if (t && !m_ById.Contains(t.m_Id))
				m_ById.Set(t.m_Id, t);
		}
	}

	void Add(MR_Track track)
	{
		if (!track)
			return;
		m_Tracks.Insert(track);
	}

	void ClearIndex()
	{
		if (!m_ById)
			m_ById = new map<string, ref MR_Track>();
		m_ById.Clear();
	}

	array<ref MR_Track> All()
	{
		return m_Tracks;
	}

	MR_Track GetById(string id)
	{
		return m_ById.Get(id);
	}

	// Case-insensitive substring search over title/artist/album/tags.
	array<ref MR_Track> Search(string query, string kindFilter = "")
	{
		array<ref MR_Track> outArr = new array<ref MR_Track>();
		if (query == string.Empty && kindFilter == string.Empty)
			return m_Tracks;

		string q = query;
		q.ToLower();
		string kind = kindFilter;
		kind.ToLower();

		foreach (MR_Track t : m_Tracks)
		{
			if (!t.m_Enabled)
				continue;

			if (kind != string.Empty)
			{
				string tk = t.m_Kind;
				tk.ToLower();
				if (tk != kind)
					continue;
			}

			if (q == string.Empty)
			{
				outArr.Insert(t);
				continue;
			}

			string haystack = (t.m_Title + " " + t.m_Artist + " " + t.m_Album + " " + t.m_Id);
			foreach (string tag : t.m_Tags)
				haystack = haystack + " " + tag;
			haystack.ToLower();

			if (haystack.Contains(q))
				outArr.Insert(t);
		}

		return outArr;
	}

	// Every track of one album, in index order (= side A order).
	// Used by the cassette / MiniDisc players: one carrier = one album.
	array<ref MR_Track> ByAlbum(string albumId)
	{
		array<ref MR_Track> outArr = new array<ref MR_Track>();
		if (albumId == string.Empty)
			return outArr;
		foreach (MR_Track t : m_Tracks)
		{
			if (!t || !t.m_Enabled)
				continue;
			if (t.m_AlbumId == albumId)
				outArr.Insert(t);
		}
		return outArr;
	}

	int CountAlbum(string albumId)
	{
		int n = 0;
		foreach (MR_Track t : m_Tracks)
		{
			if (t && t.m_Enabled && t.m_AlbumId == albumId)
				n = n + 1;
		}
		return n;
	}

	array<ref MR_Track> ByKind(string kind)
	{
		string k = kind;
		k.ToLower();
		return Search(string.Empty, k);
	}

	// Random pick that is not the track currently playing.
	MR_Track RandomTrack(string kind = "music", string excludeId = "")
	{
		array<ref MR_Track> pool = ByKind(kind);
		if (excludeId != string.Empty && pool.Count() > 1)
		{
			array<ref MR_Track> filtered = new array<ref MR_Track>();
			foreach (MR_Track t : pool)
			{
				if (t.m_Id != excludeId)
					filtered.Insert(t);
			}
			pool = filtered;
		}

		if (pool.Count() == 0)
			return null;

		int idx = Math.RandomInt(0, pool.Count());
		return pool.Get(idx);
	}

	// ----------------------------------------------------------------- export
	// Serialised for the LIBRARY RPC (client only needs what is playable there)
	string SerializeManifest()
	{
		string s = "{\"v\":" + MR.PROTOCOL.ToString() + ",\"tracks\":[";
		bool first = true;
		foreach (MR_Track t : m_Tracks)
		{
			if (!t.m_Enabled)
				continue;
			if (!first)
				s = s + ",";
			first = false;
			// Local tracks are addressed by id and resolved against the
			// client's own copy of the media pack - no paths on the wire.
			// Поля трека собираются в массив: одна строка - один оператор.
			TStringArray parts = new TStringArray();
			parts.Insert("{\"id\":\"");
			parts.Insert(MR_Track.Escape(t.m_Id));
			parts.Insert("\",\"p\":\"");
			parts.Insert(MR_Track.Escape(t.m_Provider));
			parts.Insert("\",\"k\":\"");
			parts.Insert(MR_Track.Escape(t.m_Kind));
			parts.Insert("\",\"t\":\"");
			parts.Insert(MR_Track.Escape(t.m_Title));
			parts.Insert("\",\"a\":\"");
			parts.Insert(MR_Track.Escape(t.m_Artist));
			parts.Insert("\",\"al\":\"");
			parts.Insert(MR_Track.Escape(t.m_AlbumId));
			parts.Insert("\",\"d\":");
			parts.Insert(t.m_Duration.ToString());
			parts.Insert(",\"m\":");
			parts.Insert(t.m_MediaKind.ToString());
			parts.Insert("}");
			s = s + MR_Net.JoinParts(parts);
		}
		s = s + "}";
		return s;
	}
}

// ============================================================================
//  MR_FileUtils : small file system helpers (3_Game layer, no world objects)
// ============================================================================
class MR_FileUtils
{
	static bool IsJson(string name)
	{
		string n = name;
		n.ToLower();
		return MR.EndsWith(n, ".json");
	}

	// Collect *.json directly inside dir (non recursive).
	//
	// FILE ENUMERATION - the API is easy to get wrong, and three attempts at
	// this function failed for three different reasons. The canonical shape
	// (DayZ wiki, DZEXP-134 workaround, vanilla init.c):
	//
	//     FindFileHandle handle = FindFile(pattern, fileName, fileAttr, flags);
	//     if (handle)
	//     {
	//         // IMPORTANT: the FIRST match is already in fileName here
	//         bool found = true;
	//         while (found)
	//         {
	//             ... use fileName / fileAttr ...
	//             found = FindNextFile(handle, fileName, fileAttr);   // bool!
	//         }
	//         CloseFindFile(handle);
	//     }
	//
	// The mistakes that cost us compile errors (logs 17:46 and 18:00):
	//   * FindNextFile returns bool, not a handle. "handle = FindNextFile(...)"
	//     gives "Incompatible parameter 'handle'" whatever the argument count.
	//   * it takes THREE arguments: (handle, out fileName, out fileAttr).
	//   * the handle must be released with CloseFindFile().
	static int CollectJsonFiles(string dir, inout TStringArray files)
	{
		string pattern = dir + "/*.json";
		string fileName = string.Empty;
		FileAttr fileAttr;

		FindFileHandle handle = FindFile(pattern, fileName, fileAttr, FindFileFlags.ALL);
		if (!handle)
			return files.Count();

		// The first result is already delivered by FindFile() itself.
		bool found = true;
		while (found)
		{
			if (fileName != string.Empty && IsJson(fileName))
				files.Insert(dir + "/" + fileName);

			found = FindNextFile(handle, fileName, fileAttr);
		}

		CloseFindFile(handle);
		return files.Count();
	}

	// Collect sub directory NAMES (relative, single level) inside dir.
	static int CollectDirs(string dir, inout TStringArray dirs)
	{
		string pattern = dir + "/*";
		string fileName = string.Empty;
		FileAttr fileAttr;

		FindFileHandle handle = FindFile(pattern, fileName, fileAttr, FindFileFlags.DIRECTORIES);
		if (!handle)
			return dirs.Count();

		bool found = true;
		while (found)
		{
			if (fileName != string.Empty && fileName != "." && fileName != "..")
			{
				if (dirs.Find(fileName) == -1)
					dirs.Insert(fileName);
			}

			found = FindNextFile(handle, fileName, fileAttr);
		}

		CloseFindFile(handle);
		return dirs.Count();
	}
}
