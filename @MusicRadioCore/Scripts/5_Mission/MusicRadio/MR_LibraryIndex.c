// ============================================================================
//  MusicRadio - Core
//  MR_LibraryIndex.c : the one place where a media library instance lives.
//  Layer: 5_Mission   (UI + RPC code reads it from here)
// ============================================================================

class MR_LibraryIndex
{
	protected static ref MR_Library m_Library;

	static MR_Library Get()
	{
		if (!m_Library)
			m_Library = new MR_Library();
		return m_Library;
	}

	static void Reset()
	{
		m_Library = new MR_Library();
	}
}
