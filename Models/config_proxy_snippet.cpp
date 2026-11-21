// =============================================================================
//  MusicRadio - proxy snippet for VISIBLE docking
//
//  Only needed once the .p3d models from this folder exist and the car / radio
//  models have the matching selection names. Without it everything still works
//  logically (docked deck plays music), the deck is just not drawn on the dash.
//
//  How to use: paste the blocks below into @MusicRadioCore/config.cpp
//  (inside the existing CfgVehicles / CfgSlots / CfgNonAIVehicles sections),
//  then uncomment and fix the paths.
// =============================================================================

/* ---------------------------------------------------------------------------
   1) Slot points at a selection on the vehicle model.
      Add the selection "MR_DashSlot" to each supported car .p3d first, then
      the slot can tell the engine where to draw the proxy.
--------------------------------------------------------------------------- */
/*
class CfgSlots
{
	class Slot_MR_DashSlot
	{
		name = "MR_DashSlot";
		displayName = "Music player";
		selection = "MR_DashSlot";     // named selection on the car model
		ghostIcon = "missing";
	};

	class Slot_MR_MemoryCard
	{
		name = "MR_MemoryCard";
		displayName = "Memory card";
		selection = "MR_MemoryCard";   // named selection on PersonalRadio.p3d
		ghostIcon = "missing";
	};
};
*/

/* ---------------------------------------------------------------------------
   2) Proxy: what the engine renders inside the slot.
      inventorySlot[] on the proxy must match the slot name.
--------------------------------------------------------------------------- */
/*
class CfgNonAIVehicles
{
	class ProxyAttachment;

	class ProxyMR_MemoryCard : ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = { "MR_MemoryCard" };
		model = "MusicRadioCore\data\mr_memorycard.p3d";
	};

	class ProxyMR_DashPlayer : ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = { "MR_DashSlot" };
		model = "MusicRadioCore\data\mr_mdplayer.p3d";
	};

	class ProxyMR_DashDeck : ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = { "MR_DashSlot" };
		model = "MusicRadioCore\data\mr_cassettedeck.p3d";
	};
};
*/

/* ---------------------------------------------------------------------------
   3) Items that are drawn through the proxy need the slot on their side too
      (already set for the memory card in the main config).
--------------------------------------------------------------------------- */
/*
class CfgVehicles
{
	class MR_MemoryCard_Base;
	class MR_MemoryCard_01 : MR_MemoryCard_Base
	{
		inventorySlot[] = { "MR_MemoryCard" };
	};
};
*/
