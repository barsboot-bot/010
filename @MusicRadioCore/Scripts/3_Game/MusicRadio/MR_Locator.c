// ============================================================================
//  MusicRadio - Core
//  MR_Locator.c : the "LCT" track locator.
//
//  WHY THIS EXISTS
//  ---------------
//  A media pack (the 3rd mod) is distributed as a PBO whose prefix may change
//  between releases (@MyMusic v1 -> @MyMusicPack v2 ...). If the server hard
//  coded the PBO prefix, every rename would break every saved playlist.
//
//  So the server never sends a raw file path. It sends a *locator*:
//      provider | providerVersion | trackId | volume(out) | seed
//  and the CLIENT resolves  provider -> concrete SoundSet name  using its own
//  copy of the media pack. Nothing about the pack layout is hard coded on the
//  server, and the client validates everything before playing.
//
//  Wire format (the one string that travels over RPC):
//      LCT|1|<provider>|<trackId>|<volume>|<seed>
//  Audio (file / url / duration) is sent separately in the PLAY RPC.
// ============================================================================

class MR_Locator
{
	int    m_Protocol;
	string m_Provider;      // e.g. "MusicRadioDB"
	string m_TrackId;       // stable id inside that provider
	int    m_Volume;        // 0..10
	int    m_Seed;          // random start offset seed (client derives offset)

	void MR_Locator()
	{
		m_Protocol = MR.PROTOCOL;
		m_Provider = string.Empty;
		m_TrackId  = string.Empty;
		m_Volume   = MR.VOLUME_STEPS;
		m_Seed     = 0;
	}

	static MR_Locator FromString(string raw)
	{
		MR_Locator loc = new MR_Locator();
		if (raw == string.Empty)
			return loc;

		TStringArray parts = new TStringArray();
		raw.Split("|", parts);
		if (parts.Count() < 6)
		{
			MR.LogWarn("Bad locator: " + raw);
			return loc;
		}

		loc.m_Protocol = parts.Get(1).ToInt();
		loc.m_Provider = parts.Get(2);
		loc.m_TrackId  = parts.Get(3);
		loc.m_Volume   = parts.Get(4).ToInt();
		loc.m_Seed     = parts.Get(5).ToInt();
		return loc;
	}

	// НЕ ToString(): такое имя уже есть у базового Class, движок отвечает
	// "Multiple declaration of function 'ToString'". Своё имя - Serialize().
	string Serialize()
	{
		return "LCT|1|" + m_Provider + "|" + m_TrackId + "|" + m_Volume.ToString() + "|" + m_Seed.ToString();
	}

	bool IsValid()
	{
		return (m_Protocol == MR.PROTOCOL && m_Provider != string.Empty && m_TrackId != string.Empty);
	}
}
