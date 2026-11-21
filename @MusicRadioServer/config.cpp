// =============================================================================
//  MusicRadio - Server (mod #1 of 3)
//  config.cpp   -   version 1.0.0
//
//  PBO layout (source folder for AddonBuilder):
//      config.cpp
//      Scripts/4_World/MusicRadio/*.c
//      Scripts/5_Mission/MusicRadio/*.c
//
//  Build: python3 tools/stage_pbo.py  ->  tools\build_pbo.bat
//  PBO prefix must be "MusicRadioServer". NEVER publish this mod.
//
//  INSTALL ON THE SERVER ONLY - clients must not have this mod.
//
//  Why a separate mod at all, if the core already runs server side?
//  ---------------------------------------------------------------
//  * the authoritative rules (who may play what, radii, cooldowns, audit log)
//    never reach a client machine, so a cheater cannot read or patch them;
//  * a server admin can update the rules without asking 60 players to
//    re-download anything;
//  * the client install stays small: only the core (logic) + the media pack.
//
//  This mod depends on the core mod but never the other way round.
// =============================================================================

class CfgPatches
{
	class MusicRadioServer
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = { "DZ_Data", "DZ_Sounds_Effects", "MusicRadioCore" };
	};
};

class CfgMods
{
	class MusicRadioServer
	{
		dir = "MusicRadioServer";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "MusicRadio - Server";
		credits = "MusicRadio project";
		author = "MusicRadio project";
		authorID = "0";
		version = "1.0.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "World", "Mission" };

		class defs
		{
			// The engine loads world/mission modules on clients too if the mod is
			// present there - that is exactly why this mod is installed on the
			// server only: serverScriptModule is not a thing, so we rely on the
			// mod not being in the client's mod list.
			class worldScriptModule
			{
				value = "";
				files[] = { "MusicRadioServer/Scripts/4_World" };
			};
			class missionScriptModule
			{
				value = "";
				files[] = { "MusicRadioServer/Scripts/5_Mission" };
			};
		};
	};
};
