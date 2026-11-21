// ============================================================================
//  MusicRadio - Core
//  MR_Animations.c : how a media player looks in the player's hands.
//  Layer: 4_World
//
//  Without this the deck is held like a pickaxe: the engine has no IK profile
//  for our classes, so it falls back to the default one-handed pose. VOS_Radio
//  shipped exactly this file (scripts/4_World/Animations.c) for its four radios.
//
//  Two strings per item:
//    .asi  - the workspace (which hand, how the arm is posed)
//    .anm  - the IK target for that item class
//
//  Profiles used, all vanilla, so nothing has to be exported with our mod:
//    props/player_main_1h_radio.asi + ik/gear/Radio.anm          big desk unit
//    player_main_1h.asi             + ik/gear/PersonalRadio.anm  pocket sized
//    player_main_1h.asi             + ik/gear/cassette.anm       a bare cassette
//
//  Keep the list in one place: MR_Registry/check_project.py do not see these
//  strings, so a typo here is only visible in game.
// ============================================================================

modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);

		// --- cassette deck -------------------------------------------------
		// A walkman-sized deck uses the PersonalRadio pose: the big radio pose
		// pushes it away from the chest and the animations start to clip.
		pType.AddItemInHandsProfileIK("MR_CassettePlayer",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");

		pType.AddItemInHandsProfileIK("MR_CassettePlayer_Black",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");

		// --- mini disc -----------------------------------------------------
		pType.AddItemInHandsProfileIK("MR_MiniDiscPlayer",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");

		// --- carriers ------------------------------------------------------
		// Only the base classes are registered: every generated album class
		// (MR_Cassette_Rock01, ...) inherits the pose, so a media pack with 400
		// albums does not need 400 lines here.
		pType.AddItemInHandsProfileIK("MR_CassetteCase_Base",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");

		pType.AddItemInHandsProfileIK("MR_MiniDisc_Base",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");

		pType.AddItemInHandsProfileIK("MR_MemoryCard_Base",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");
	}
}
