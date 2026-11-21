// ============================================================================
//  MusicRadio - Database
//  MR_DB_Placeholder.c
//
//  The media pack needs no logic: all it does is declare CfgSoundShaders /
//  CfgSoundSets in config.cpp. This file exists only so the Scripts folder in
//  the workshop item is not empty (some tools warn about that) and to give
//  future per-pack extensions a home.
//
//  Do not add gameplay code here - it would be installed on every client and
//  the DB mod is meant to stay a pure content pack.
// ============================================================================

class MR_DB_Info
{
	static const string PROVIDER = "MusicRadioDB";
	static const string VERSION  = "0.1.0";
	static const int    SLOTS    = 8;

	static string Describe()
	{
		return "MusicRadio media pack, provider=" + PROVIDER + " v" + VERSION;
	}
}
