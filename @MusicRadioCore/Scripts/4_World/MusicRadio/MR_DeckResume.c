// ============================================================================
//  MusicRadio - Core
//  MR_DeckResume.c : the deck reports its position, the server writes the tape.
//  Layer: 4_World
//
//  THE SPLIT WE KEPT FROM OMD
//  --------------------------
//  OMD let the SERVER advance the tape clock (Timer.Run(1, ..., true) on the
//  server, clients never count time). That is the right call: a client cannot
//  be trusted with a timestamp and two clients never accumulate drift, because
//  neither of them is the source of truth.
//
//  What we did not keep: OMD's timer body also re-implemented playlist advance
//  and noise generation inside the timer. Here the clock does ONE thing - write
//  the current position onto the carrier. Everything else (what plays next) is
//  already decided by MR_ServerEngine and is not duplicated.
// ============================================================================

// ============================================================================
//  Deck host: a cassette deck / MD player as a MusicRadio host.
// ----------------------------------------------------------------------------
//  Until now MR_MediaItems registered decks in MR_Registry as HOST_PLAYER, but
//  no MR_DeviceBase existed for them and MR_ClientRPC.ResolveHost() only knew
//  HOST_ITEM and HOST_VEHICLE. A deck therefore registered, was found by the
//  server and then could not be played on the client. This class closes that.
// ============================================================================
class MR_DeckHost extends MR_DeviceBase
{
	protected EntityAI m_Deck;

	void MR_DeckHost(EntityAI deck)
	{
		MR_Init();
		m_Deck = deck;
	}

	EntityAI GetDeck()
	{
		return m_Deck;
	}

	override Object MR_GetAudioObject()
	{
		return m_Deck;
	}

	override bool MR_CanPlay()
	{
		if (!m_Deck)
			return false;
		if (m_Deck.IsRuined())
			return false;

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(m_Deck);
		if (deck)
		{
			string why;
			return deck.MR_IsReady(why);
		}

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(m_Deck);
		if (md)
		{
			string why2;
			return md.MR_IsReady(why2);
		}

		return false;
	}

	override string MR_BlockReason()
	{
		if (!m_Deck)
			return "player missing";

		string why;
		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(m_Deck);
		if (deck)
		{
			if (deck.MR_IsReady(why))
				return string.Empty;
			return why;
		}

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(m_Deck);
		if (md)
		{
			if (md.MR_IsReady(why))
				return string.Empty;
			return why;
		}

		return "not a media player";
	}

	// A pocket deck is quieter than a car stereo. Half of the configured radius,
	// never less than 5 m - the same rule MR_ItemHost uses, so a tape deck and a
	// handheld radio behave consistently.
	override float MR_GetEffectiveRadius()
	{
		float r = super.MR_GetEffectiveRadius() * 0.5;
		return MR.ClampFloat(r, 5, MR.RADIUS_MAX);
	}

	// A pocket deck is a personal device: 50% of the configured volume, the
	// same value VOS_Radio used for its walkman (0.2 vs 0.4 for the desk unit).
	override float MR_GetVolumeCeiling()
	{
		return 0.5;
	}
}

// ============================================================================
//  MR_DeckState : server side bookkeeping of one playing carrier.
// ----------------------------------------------------------------------------
//  Keyed by host, exactly like MR_ServerEngine sessions, so a deck that is
//  destroyed or emptied simply disappears from the map on the next tick.
//  Nothing here runs on a client: MR_TapeSetPosition refuses to write there.
// ============================================================================
class MR_DeckState
{
	protected static ref map<string, ref MR_DeckRuntime> m_Playing;

	static void Ensure()
	{
		if (!m_Playing)
			m_Playing = new map<string, ref MR_DeckRuntime>();
	}

	static string Key(MR_HostType type, int idLow, int idHigh)
	{
		return ((int)type).ToString() + ":" + idLow.ToString() + ":" + idHigh.ToString();
	}

	// Called by the server engine when a play request was accepted. Resolves
	// which physical carrier is spinning so ticks know where to write.
	static void TrackStart(MR_HostType type, int idLow, int idHigh, string trackId, int trackNo, int offset)
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		MR_DeckRuntime rt = new MR_DeckRuntime();
		rt.m_HostType = type;
		rt.m_HostId = idLow;
		rt.m_HostIdHigh = idHigh;
		rt.m_TrackId = trackId;
		rt.m_TrackNo = trackNo;
		rt.m_StartOffset = offset;
		rt.m_StartMs = GetGame().GetTime();
		rt.m_Carrier = FindCarrier(type, idLow, idHigh);

		m_Playing.Set(Key(type, idLow, idHigh), rt);

		// First write happens immediately: if the server dies one second later
		// the tape still knows which track was running.
		Write(rt);
	}

	static void TrackStop(MR_HostType type, int idLow, int idHigh)
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		string key = Key(type, idLow, idHigh);
		MR_DeckRuntime rt = m_Playing.Get(key);
		if (rt)
			Write(rt);           // final position before forgetting
		m_Playing.Remove(key);
	}

	// The carrier physically inside this host ("" carrier -> nothing to write).
	static EntityAI FindCarrier(MR_HostType type, int idLow, int idHigh)
	{
		Object hostObj = MR_Registry.Find(type, idLow, idHigh);
		if (!hostObj)
			return null;

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(hostObj);
		if (deck)
			return deck.MR_GetInsertedCassette();

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(hostObj);
		if (md)
			return md.MR_GetInsertedDisc();

		// Рация - тоже устройство с физическим носителем (20261174), поэтому
		// позиция на "кассете в рации" запоминается так же, как в деке.
		PersonalRadio radio = PersonalRadio.Cast(hostObj);
		if (radio)
			return radio.MR_InsertedCarrier();

		// A car plays through the deck docked on its dash.
		CarScript car = CarScript.Cast(hostObj);
		if (car)
		{
			EntityAI docked = MR_MediaItems.DockedInVehicle(car);
			MR_CassettePlayer_Base cdeck = MR_CassettePlayer_Base.Cast(docked);
			if (cdeck)
				return cdeck.MR_GetInsertedCassette();

			MR_MiniDiscPlayer_Base cmd = MR_MiniDiscPlayer_Base.Cast(docked);
			if (cmd)
				return cmd.MR_GetInsertedDisc();

			// В слоте машины может лежать сама кассета (20261173) - у неё
			// тоже есть память позиции.
			if (MR_MediaItems.IsCarrier(docked))
				return docked;
		}
		return null;
	}

	// Seconds into the current track. The server clock is monotonic enough for
	// this: it is the same value the session uses for late joiners.
	static int Position(MR_DeckRuntime rt, int now)
	{
		if (!rt)
			return 0;
		int elapsed = (now - rt.m_StartMs) / 1000;
		return MR_CarrierState.ClampPosition(rt.m_StartOffset + elapsed, 0);
	}

	static void Write(MR_DeckRuntime rt)
	{
		if (!rt || !rt.m_Carrier)
			return;
		if (rt.m_TrackId == string.Empty)
			return;

		// m_Carrier объявлен EntityAI (так его отдают Cast-ы), а MR_TapeSetPosition
		// живёт в MR_CarrierBase: без приведения компилятор пишет
		//   Undefined function 'EntityAI.MR_TapeSetPosition'   (лог 20:47, 221)
		MR_CarrierBase carrier = MR_CarrierBase.Cast(rt.m_Carrier);
		if (!carrier)
			return;
		carrier.MR_TapeSetPosition(rt.m_TrackId, Position(rt, GetGame().GetTime()), rt.m_TrackNo);
	}

	// Server tick (MR_ServerEngine.Tick -> here). One write per playing host per
	// TAPE_TICK_MS; on a 40-slot server with three tapes running this is six
	// string writes per second, and none of them over the network.
	static void Tick()
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		TStringArray dead = new TStringArray();
		foreach (string key, MR_DeckRuntime rt : m_Playing)
		{
			if (!rt)
			{
				dead.Insert(key);
				continue;
			}

			// The tape may have been swapped while the deck kept playing: write
			// what is still in the slot and drop the entry when it changed.
			EntityAI carrier = FindCarrier(rt.m_HostType, rt.m_HostId, rt.m_HostIdHigh);
			if (!carrier || carrier != rt.m_Carrier)
			{
				Write(rt);
				dead.Insert(key);
				continue;
			}

			Write(rt);
		}

		foreach (string k : dead)
			m_Playing.Remove(k);
	}

	// The head moved inside the current track (rewind / fast-forward / a
	// deliberate seek from the UI). Called from ACT_SEEK.
	static void SeekTo(MR_HostType type, int idLow, int idHigh, int position)
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		string key = Key(type, idLow, idHigh);
		MR_DeckRuntime rt = m_Playing.Get(key);
		if (!rt)
			return;

		if (position < 0)
			position = 0;
		rt.m_StartOffset = position;
		rt.m_StartMs = GetGame().GetTime();
		Write(rt);
	}

	// True when the cassette inside this host is the one playing.
	static bool IsPlayingCarrier(EntityAI carrier)
	{
		if (!carrier)
			return false;
		Ensure();

		foreach (string key, MR_DeckRuntime rt : m_Playing)
		{
			if (rt && rt.m_Carrier == carrier)
				return true;
		}
		return false;
	}

	// Position this carrier remembers for a given album ("" when it does not
	// belong to it). Used when a tape is inserted into a deck.
	static bool ResumePoint(EntityAI carrier, string albumId, out string trackId, out int position)
	{
		trackId = string.Empty;
		position = 0;

		if (!carrier)
			return false;

		MR_CarrierMemory mem;
		MR_CassetteCase_Base tape = MR_CassetteCase_Base.Cast(carrier);
		if (tape)
			mem = tape.MR_TapeMemory();

		MR_MiniDisc_Base disc = MR_MiniDisc_Base.Cast(carrier);
		if (disc)
			mem = disc.MR_TapeMemory();

		if (!mem || mem.IsEmpty())
			return false;

		trackId = mem.m_TrackId;
		position = mem.m_Position;
		return true;
	}
}

class MR_DeckRuntime
{
	MR_HostType m_HostType;
	int         m_HostId;
	int         m_HostIdHigh;
	string      m_TrackId;
	int         m_TrackNo;
	int         m_StartOffset;
	int         m_StartMs;
	EntityAI    m_Carrier;

	void MR_DeckRuntime()
	{
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		m_TrackId = string.Empty;
		m_TrackNo = 0;
		m_StartOffset = 0;
		m_StartMs = 0;
		m_Carrier = null;
	}
}

// ============================================================================
//  Decks: create the host once and hand it out.
// ============================================================================
