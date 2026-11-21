// ============================================================================
//  MusicRadio - Core
//  MR_Json.c : tiny tolerant JSON reader.
//
//  DayZ ships a JSON serializer but it refuses comments, trailing commas and
//  single quotes - all of which humans put into hand edited media databases.
//  This reader accepts a slightly relaxed JSON so pack authors can keep notes:
//      // comment        # comment        trailing commas        'single quotes'
//  It only produces a generic tree (MR_JsonValue). Parsing into typed
//  MR_Track / MR_Station objects happens in MR_Library.c so that a single
//  broken entry can be skipped without throwing away the whole database.
// ============================================================================

class MR_JsonValue
{
	static const int T_NULL   = 0;
	static const int T_STRING = 1;
	static const int T_NUMBER = 2;
	static const int T_BOOL   = 3;
	static const int T_OBJECT = 4;
	static const int T_ARRAY  = 5;

	int    m_Type;
	string m_Str;
	string m_Num;                       // kept as text - avoids locale issues
	bool   m_Bool;
	ref map<string, ref MR_JsonValue> m_Obj;
	ref array<ref MR_JsonValue>       m_Arr;

	void MR_JsonValue()
	{
		m_Type = T_NULL;
		m_Str  = string.Empty;
		m_Num  = "0";
		m_Bool = false;
	}

	// --- typed accessors (never fail, return defaults) ----------------------
	string AsString(string def = "")
	{
		if (m_Type == T_STRING) return m_Str;
		if (m_Type == T_NUMBER) return m_Num;
		if (m_Type == T_BOOL)   return m_Bool.ToString();
		return def;
	}

	float AsFloat(float def = 0)
	{
		if (m_Type == T_NUMBER) return m_Num.ToFloat();
		if (m_Type == T_STRING) return m_Str.ToFloat();
		return def;
	}

	int AsInt(int def = 0)
	{
		if (m_Type == T_NUMBER) return m_Num.ToInt();
		if (m_Type == T_STRING) return m_Str.ToInt();
		return def;
	}

	bool AsBool(bool def = false)
	{
		if (m_Type == T_BOOL) return m_Bool;
		if (m_Type == T_NUMBER) return (m_Num.ToInt() != 0);
		if (m_Type == T_STRING)
		{
			string t = m_Str;
			t.ToLower();
			if (t == "true" || t == "1" || t == "yes") return true;
			if (t == "false" || t == "0" || t == "no") return false;
		}
		return def;
	}

	// Object member lookup; returns null when missing.
	MR_JsonValue Get(string key)
	{
		if (m_Type != T_OBJECT || !m_Obj)
			return null;
		return m_Obj.Get(key);
	}

	string GetString(string key, string def = "")
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsString(def);
	}

	int GetInt(string key, int def = 0)
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsInt(def);
	}

	float GetFloat(string key, float def = 0)
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsFloat(def);
	}

	bool GetBool(string key, bool def = false)
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsBool(def);
	}

	array<ref MR_JsonValue> GetArray(string key)
	{
		MR_JsonValue v = Get(key);
		if (!v || v.m_Type != T_ARRAY)
			return null;
		return v.m_Arr;
	}

	// Integer-indexed access used by files that describe a list as
	//   "tracks": { "1": {...}, "2": {...} }   (handier than arrays for humans)
	array<ref MR_JsonValue> CollectValues()
	{
		array<ref MR_JsonValue> outArr = new array<ref MR_JsonValue>();
		if (m_Type == T_ARRAY && m_Arr)
		{
			foreach (MR_JsonValue av : m_Arr)
				outArr.Insert(av);
			return outArr;
		}
		if (m_Type == T_OBJECT && m_Obj)
		{
			foreach (string k, MR_JsonValue ov : m_Obj)
				outArr.Insert(ov);
		}
		return outArr;
	}
}

class MR_Json
{
	protected string m_Src;
	protected int    m_Pos;
	protected int    m_Len;

	// If set, the file was NOT valid JSON and the parser did a best effort pass.
	bool m_HadErrors;

	static MR_JsonValue ParseFile(string path)
	{
		if (!FileExist(path))
		{
			MR.LogWarn("JSON file not found: " + path);
			return null;
		}

		string raw;
		if (!MR_Json.LoadText(path, raw))
			return null;

		return MR_Json.Parse(raw);
	}

	static bool LoadText(string path, out string raw)
	{
		// FileHandle has NO methods: it is "typedef int[] FileHandle", so
		// neither "fh == 0" (Incompatible parameter) nor "fh.IsOpen()"
		// (Undefined function, log 18:00) compiles. The reliable check is to
		// ask the filesystem first - the idiom already used in this project.
		if (!FileExist(path))
		{
			MR.LogWarn("Cannot open file: " + path);
			return false;
		}

		FileHandle fh = OpenFile(path, FileMode.READ);
		if (!fh)
		{
			MR.LogWarn("Cannot open file: " + path);
			return false;
		}

		string line = string.Empty;
		raw = string.Empty;
		while (FGets(fh, line) >= 0)
		{
			raw = raw + line + "\n";
		}
		CloseFile(fh);
		return true;
	}

	static bool SaveText(string path, string data)
	{
		// A write may create the file, so FileExist() is not a precondition
		// here: "if (!fh)" on the returned handle is what the working
		// call sites in this project use.
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (!fh)
		{
			MR.LogWarn("Cannot write file: " + path);
			return false;
		}
		FPrint(fh, data);
		CloseFile(fh);
		return true;
	}

	static MR_JsonValue Parse(string src)
	{
		MR_Json p = new MR_Json();
		p.m_Src = src;
		p.m_Len = src.Length();
		p.m_Pos = 0;
		p.m_HadErrors = false;

		// Skip UTF-8 BOM if present. In a byte string 0xEF 0xBB 0xBF shows up
		// as three characters; "\u00EF" below is the first of them.
		if (p.m_Len > 2 && src.Substring(0, 1) == "\u00EF" && src.Substring(1, 1) == "\u00BB")
			p.m_Pos = 3;

		MR_JsonValue root = p.ParseValue();
		if (!root)
		{
			MR.LogError("JSON: root value is empty");
			return null;
		}

		if (p.m_HadErrors)
			MR.LogWarn("JSON: parsed with recoverable errors");

		return root;
	}

	// ------------------------------------------------------------------ lexer
	protected void SkipWhitespace()
	{
		while (m_Pos < m_Len)
		{
			string ch = m_Src.Substring(m_Pos, 1);

			if (ch == " " || ch == "\t" || ch == "\n" || ch == "\r")
			{
				m_Pos = m_Pos + 1;
				continue;
			}

			// line comments
			if (ch == "/" && m_Pos + 1 < m_Len && m_Src.Substring(m_Pos + 1, 1) == "/")
			{
				m_Pos = m_Pos + 2;
				while (m_Pos < m_Len && m_Src.Substring(m_Pos, 1) != "\n")
					m_Pos = m_Pos + 1;
				continue;
			}

			// hash comments (helpful in files edited by non programmers)
			if (ch == "#")
			{
				while (m_Pos < m_Len && m_Src.Substring(m_Pos, 1) != "\n")
					m_Pos = m_Pos + 1;
				continue;
			}

			break;
		}
	}

	protected string Peek()
	{
		if (m_Pos >= m_Len)
			return string.Empty;
		return m_Src.Substring(m_Pos, 1);
	}

	protected MR_JsonValue ParseValue()
	{
		SkipWhitespace();
		string ch = Peek();
		if (ch == string.Empty)
			return null;

		if (ch == "{") return ParseObject();
		if (ch == "[") return ParseArray();
		if (ch == "\"" || ch == "'") return ParseString(ch);

		// literal / number
		int start = m_Pos;
		while (m_Pos < m_Len)
		{
			string c = m_Src.Substring(m_Pos, 1);
			if (c == "," || c == "}" || c == "]" || c == " " || c == "\n" || c == "\r" || c == "\t" || c == ":")
				break;
			m_Pos = m_Pos + 1;
		}

		string token = m_Src.Substring(start, m_Pos - start);
		MR_JsonValue v = new MR_JsonValue();

		if (token == string.Empty)
		{
			m_HadErrors = true;
			return v;
		}

		string lower = token;
		lower.ToLower();

		if (lower == "null")
		{
			v.m_Type = MR_JsonValue.T_NULL;
		}
		else if (lower == "true" || lower == "false")
		{
			v.m_Type = MR_JsonValue.T_BOOL;
			v.m_Bool = (lower == "true");
		}
		else
		{
			v.m_Type = MR_JsonValue.T_NUMBER;
			v.m_Num = token;
		}

		return v;
	}

	protected MR_JsonValue ParseObject()
	{
		MR_JsonValue obj = new MR_JsonValue();
		obj.m_Type = MR_JsonValue.T_OBJECT;
		obj.m_Obj = new map<string, ref MR_JsonValue>();

		m_Pos = m_Pos + 1;   // {

		while (m_Pos < m_Len)
		{
			SkipWhitespace();
			string ch = Peek();

			if (ch == string.Empty)
				break;

			if (ch == "}")
			{
				m_Pos = m_Pos + 1;
				return obj;
			}

			if (ch == ",")
			{
				m_Pos = m_Pos + 1;
				continue;
			}

			// key
			MR_JsonValue keyVal = null;
			if (ch == "\"" || ch == "'")
				keyVal = ParseString(ch);
			else
			{
				// unquoted key - tolerate it
				int start = m_Pos;
				while (m_Pos < m_Len)
				{
					string c = m_Src.Substring(m_Pos, 1);
					if (c == ":" || c == " " || c == "\n" || c == "\r" || c == "\t")
						break;
					m_Pos = m_Pos + 1;
				}
				keyVal = new MR_JsonValue();
				keyVal.m_Type = MR_JsonValue.T_STRING;
				keyVal.m_Str = m_Src.Substring(start, m_Pos - start);
			}

			if (!keyVal)
				break;

			string key = keyVal.AsString();
			if (key == string.Empty)
			{
				m_HadErrors = true;
			}

			SkipWhitespace();
			if (Peek() == ":")
				m_Pos = m_Pos + 1;

			MR_JsonValue val = ParseValue();
			if (!val)
			{
				m_HadErrors = true;
				val = new MR_JsonValue();
			}

			if (key != string.Empty && !obj.m_Obj.Contains(key))
				obj.m_Obj.Set(key, val);
			else if (key != string.Empty)
				MR.LogWarn("JSON: duplicate key ignored: " + key);
		}

		m_HadErrors = true;
		return obj;
	}

	protected MR_JsonValue ParseArray()
	{
		MR_JsonValue arr = new MR_JsonValue();
		arr.m_Type = MR_JsonValue.T_ARRAY;
		arr.m_Arr = new array<ref MR_JsonValue>();

		m_Pos = m_Pos + 1;   // [

		while (m_Pos < m_Len)
		{
			SkipWhitespace();
			string ch = Peek();

			if (ch == string.Empty)
				break;

			if (ch == "]")
			{
				m_Pos = m_Pos + 1;
				return arr;
			}

			if (ch == ",")
			{
				m_Pos = m_Pos + 1;
				continue;
			}

			MR_JsonValue val = ParseValue();
			if (!val)
			{
				m_HadErrors = true;
				break;
			}
			arr.m_Arr.Insert(val);
		}

		m_HadErrors = true;
		return arr;
	}

	protected MR_JsonValue ParseString(string quote)
	{
		MR_JsonValue v = new MR_JsonValue();
		v.m_Type = MR_JsonValue.T_STRING;

		m_Pos = m_Pos + 1;   // opening quote
		string outStr = string.Empty;

		while (m_Pos < m_Len)
		{
			string ch = m_Src.Substring(m_Pos, 1);

			if (ch == "\\")
			{
				// escape sequence
				if (m_Pos + 1 >= m_Len)
					break;

				string esc = m_Src.Substring(m_Pos + 1, 1);
				if (esc == "n")       outStr = outStr + "\n";
				else if (esc == "t")  outStr = outStr + "\t";
				else if (esc == "r")  outStr = outStr + "\r";
				else if (esc == "\"") outStr = outStr + "\"";
				else if (esc == "'")  outStr = outStr + "'";
				else if (esc == "\\") outStr = outStr + "\\";
				else if (esc == "/")  outStr = outStr + "/";
				else                  outStr = outStr + esc;

				// Enforce strings are byte strings; we cannot synthesise an
				// arbitrary UTF-8 byte from an int. \uXXXX is therefore
				// skipped and replaced - save databases as UTF-8 (no escapes).
				if (esc == "u")
				{
					MR.LogWarn("JSON: \\u escape found, replaced by '?'. Save the file as plain UTF-8.");
					outStr = outStr + "?";
					m_Pos = m_Pos + 6;
					continue;
				}

				m_Pos = m_Pos + 2;
				continue;
			}

			if (ch == quote)
			{
				m_Pos = m_Pos + 1;
				v.m_Str = outStr;
				return v;
			}

			outStr = outStr + ch;
			m_Pos = m_Pos + 1;
		}

		// unterminated string
		m_HadErrors = true;
		v.m_Str = outStr;
		return v;
	}

}
