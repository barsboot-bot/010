// ============================================================================
//  MusicRadio - Core
//  MR_Config.c : reading/writing JSON settings on both sides, safe defaults.
//  Layer: 3_Game
// ============================================================================

class MR_Config
{
	// ---- server side --------------------------------------------------------
	// Reads $mission:MusicRadio/AudioConfig.json. If the file is missing a
	// documented default is written so admins always have something to edit.
	static MR_Settings LoadServer()
	{
		string dirTwo = MR.DIR_MISSION;
		MR_Config.EnsureDir(dirTwo);

		string path = dirTwo + MR.CFG_AUDIO;
		MR_Settings s = null;

		if (!FileExist(path))
		{
			s = new MR_Settings();
			string text;
			s.WriteJson(text);
			MR_Json.SaveText(path, text);
			MR.Log("Default server config written to " + path);
			return s;
		}

		MR_JsonValue root = MR_Json.ParseFile(path);
		if (!root)
		{
			MR.LogError("Server config is invalid, using defaults: " + path);
			return new MR_Settings();
		}

		s = MR_Settings.FromJson(root);

		MR.Log("Server config loaded from " + path);
		return s;
	}

	// ---- client side --------------------------------------------------------
	// The client only needs layout/limits for its own UI. It is sent by the
	// server at connect time; the local file caches the last known values so a
	// player who joins a modded server with a plain client is not lost.
	static MR_Settings LoadClient()
	{
		string path = MR.DIR_PROFILE + MR.CFG_AUDIO_SAVED;
		if (!FileExist(path))
			return new MR_Settings();

		MR_JsonValue root = MR_Json.ParseFile(path);
		if (!root)
			return new MR_Settings();

		return MR_Settings.FromJson(root);
	}

	static void SaveClient(MR_Settings s)
	{
		if (!s)
			return;
		MR_Config.EnsureDir(MR.DIR_PROFILE);
		string text;
		s.WriteJson(text);
		MR_Json.SaveText(MR.DIR_PROFILE + MR.CFG_AUDIO_SAVED, text);
	}

	// ---- helpers ------------------------------------------------------------
	static void EnsureDir(string dir)
	{
		if (dir == string.Empty)
			return;
		if (!FileExist(dir))
		{
			MakeDirectory(dir);
			MR.Log("Created directory " + dir);
		}
	}

	// Human readable summary for console / logs.
	static void DumpSummary(MR_Settings s)
	{
		if (!s)
			return;
		MR.Log("---- settings ----");
		MR.Log("vehicle radio      : " + s.m_EnableVehicleRadio.ToString());
		MR.Log("handheld radio     : " + s.m_EnableHandheldRadio.ToString());
		MR.Log("require radio item : " + s.m_RequireRadioItemInCargo.ToString());
		MR.Log("local tracks       : " + s.m_AllowLocalTracks.ToString());
		MR.Log("broadcast          : " + s.m_AllowBroadcast.ToString());
		MR.Log("default volume     : " + s.m_DefaultVolume.ToString());
		MR.Log("default radius     : " + s.m_DefaultRadius.ToString());
		MR.Log("database index     : " + s.m_DbPath);
	}
}

// ============================================================================
//  MR_AuditLog : append-only log for moderation (who played what, when).
//  Layer: 3_Game, server only in practice.
// ============================================================================
class MR_AuditLog
{
	// Статические поля инициализируются ТОЛЬКО нулём: "= string.Empty" в
	// script.log даёт "FIX-ME: Only null assignment is allowed on plain data".
	// У FileHandle те же правила, поэтому признак открытого файла - отдельный
	// bool, а не сравнение хендла с 0.
	protected static FileHandle m_Handle;
	protected static bool       m_Open;
	protected static string     m_Path;

	static void Open(string path)
	{
		if (m_Open)
			Close();

		m_Path = path;
		MR_Config.EnsureDir(MR.DIR_PROFILE);
		MR_Config.EnsureDir(MR.DIR_MISSION);
		// FileHandle is "typedef int[] FileHandle": it has no IsOpen(), and
		// "m_Handle != 0" gives "Incompatible parameter 'm_Handle'"
		// (logs 17:46 and 18:00). What DOES work is the engine's own truth
		// test on the array itself - the same "if (!handle)" the DayZ wiki
		// uses for FindFileHandle. A failed open returns an empty array.
		m_Handle = OpenFile(path, FileMode.WRITE);
		if (!m_Handle)
		{
			m_Open = false;
			MR.LogWarn("Audit log disabled (cannot open " + path + ")");
			return;
		}

		m_Open = true;
		Line("log opened, ver " + MR.VERSION);
	}

	static void Close()
	{
		if (!m_Open)
			return;

		Line("log closed");
		CloseFile(m_Handle);
		// CloseFile() closes the stream; the handle itself is an int[] and
		// cannot be nulled or set to 0 (that was mr_config.c:136). Only the
		// flag is reset - every write path checks m_Open first.
		m_Open = false;
	}

	static void Line(string text)
	{
		if (!m_Open)
			return;

		string stamp = MR_AuditLog.Stamp();
		FPrint(m_Handle, stamp + " | " + text + "\n");
	}

	static string Stamp()
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;

		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		return year.ToString() + "-" + MR.Pad2(month) + "-" + MR.Pad2(day) + " " + MR.Pad2(hour) + ":" + MR.Pad2(minute) + ":" + MR.Pad2(second);
	}
}
