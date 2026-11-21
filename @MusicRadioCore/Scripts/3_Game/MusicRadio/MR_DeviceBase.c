// ============================================================================
//  MusicRadio - Core
//  MR_DeviceBase.c : the contract every "host" object must fulfil.
//  Layer: 3_Game
//
//  A host is anything that can emit radio audio in the world:
//    * a handheld radio item   (PersonalRadio, ...)
//    * a vehicle               (CarScript gets this via modded class)
//    * a static speaker        (optional)
//
//  The base keeps the replicated state and the hooks; the thin mixins in
//  4_World implement them for the concrete vanilla classes.
// ============================================================================

class MR_DeviceBase
{
	// --- replicated state (changes are pushed to clients by MR_ClientSync) ---
	protected bool        m_MR_IsPlaying;
	protected string      m_MR_TrackId;         // catalogue id, for the UI
	protected string      m_MR_TrackTitle;      // for the "now playing" widget
	protected string      m_MR_Provider;
	protected int         m_MR_Volume;          // 0..10
	protected float       m_MR_Radius;
	protected int         m_MR_OffsetAtStart;   // seconds
	protected int         m_MR_StartTimeMs;     // client clock when playback began
	protected int         m_MR_Duration;        // seconds, 0 = unknown / live
	protected bool        m_MR_EndReported;     // client already told the server

	// --- client only ---------------------------------------------------------
	// NOT "ref": EffectSound is reference counted by the manager and nulling a
	// ref member while the engine still owns the sound is a crash source.
	protected EffectSound m_MR_Sound;
	protected string      m_MR_SoundSet;
	protected int         m_MR_LastAppliedVolume;
	protected bool        m_MR_AudioBroken;     // soundset did not resolve

	void MR_Init()
	{
		m_MR_IsPlaying     = false;
		m_MR_TrackId       = string.Empty;
		m_MR_TrackTitle    = string.Empty;
		m_MR_Provider      = string.Empty;
		m_MR_Volume        = 6;
		m_MR_Radius        = MR.RADIUS_DEFAULT;
		m_MR_OffsetAtStart = 0;
		m_MR_StartTimeMs   = 0;
		m_MR_Sound         = null;
		m_MR_SoundSet      = string.Empty;
		m_MR_LastAppliedVolume = -1;
		m_MR_AudioBroken    = false;
		m_MR_Duration      = 0;
		m_MR_EndReported   = false;
	}

	// ---------------------------------------------------------------- getters
	bool MR_IsPlaying()
	{
		return m_MR_IsPlaying;
	}

	string MR_GetTrackTitle()
	{
		return m_MR_TrackTitle;
	}

	string MR_GetTrackId()
	{
		return m_MR_TrackId;
	}

	int MR_GetVolume()
	{
		return m_MR_Volume;
	}

	float MR_GetRadius()
	{
		return m_MR_Radius;
	}

	int MR_GetElapsedSeconds()
	{
		if (!m_MR_IsPlaying)
			return 0;
		int elapsed = (GetGame().GetTime() - m_MR_StartTimeMs) / 1000;
		return m_MR_OffsetAtStart + elapsed;
	}

	// ---------------------------------------------------------------- setters
	void MR_SetState(string trackId, string title, string provider, int volume, float radius, int offsetSeconds)
	{
		m_MR_TrackId       = trackId;
		m_MR_TrackTitle    = title;
		m_MR_Provider      = provider;
		m_MR_Volume        = MR.ClampInt(volume, 0, MR.VOLUME_STEPS);
		m_MR_Radius        = MR.ClampFloat(radius, MR.RADIUS_MIN, MR.RADIUS_MAX);
		m_MR_OffsetAtStart = offsetSeconds;
		m_MR_StartTimeMs   = GetGame().GetTime();
		m_MR_IsPlaying     = true;
	}

	void MR_SetDurationClient(int duration)
	{
		m_MR_Duration = duration;
		m_MR_EndReported = false;
	}

	int MR_GetDuration()
	{
		return m_MR_Duration;
	}

	bool MR_WantsEndReport()
	{
		return m_MR_EndReported;
	}

	void MR_MarkEndReported()
	{
		m_MR_EndReported = true;
	}

	// Volume changed by the server on behalf of another player.
	void MR_SetVolumeClient(int volume)
	{
		m_MR_Volume = MR.ClampInt(volume, 0, MR.VOLUME_STEPS);
		MR_AudioApplyVolume();
	}

	void MR_ClearState()
	{
		m_MR_IsPlaying  = false;
		m_MR_TrackId    = string.Empty;
		m_MR_TrackTitle = string.Empty;
	}

	// ------------------------------------------------------------ host checks
	// Overridden by concrete hosts. Default: never usable.
	bool MR_CanPlay()
	{
		return false;
	}

	// Human readable reason why the radio cannot be used (shown to the player).
	string MR_BlockReason()
	{
		return "not available";
	}

	// Where the sound is anchored and how far it travels.
	Object MR_GetAudioObject()
	{
		return null;
	}

	float MR_GetEffectiveRadius()
	{
		return m_MR_Radius;
	}

	// Loudness ceiling of this host relative to the configured volume.
	// Overridden by the concrete hosts (a pocket deck is quieter than a car).
	float MR_GetVolumeCeiling()
	{
		return 1.0;
	}

	// ------------------------------------------------------ client playback
	// Starts audio on this host. Called by MR_ClientAudio (client only).
	bool MR_AudioStart(string soundSet, bool loop, int offsetSeconds)
	{
		MR_AudioStop(true);

		m_MR_SoundSet = soundSet;
		Object anchor = MR_GetAudioObject();
		if (!anchor)
		{
			MR.LogWarn("AudioStart: no anchor object");
			return false;
		}

		if (!MR_MediaResolver.SoundSetExists(soundSet))
		{
			m_MR_AudioBroken = true;
			MR.LogWarn("AudioStart: soundset not present on this client: " + soundSet);
			return false;
		}

		m_MR_AudioBroken = false;

		// Looping sets are started with loop=true; for one-shots we let the
		// engine report the end through Event_OnSoundWaveEnded.
		m_MR_Sound = SEffectManager.PlaySoundOnObject(soundSet, anchor, MR.FADE_IN, MR.FADE_OUT, loop);
		if (!m_MR_Sound)
		{
			MR.LogWarn("AudioStart: engine refused to play " + soundSet);
			return false;
		}

		// Resume inside a track: a tape remembers "2:17", not "track 3".
		//
		// EffectSound has NO SetStartOffset of its own - the method belongs to
		// AbstractWave. OMD_CarRadio had to mod EffectSound just to reach it:
		//     void OmdRadioSetStartOffset(float offset)
		//     { if (m_SoundWaveObject) m_SoundWaveObject.SetStartOffset(offset); }
		// We do the same, but in our own wrapper (MR_SoundMix.c) and with a
		// null check, so a sound that failed to build cannot crash the tick.
		//
		// Unit: SetStartOffset takes a 0..1 FRACTION of the wave, not seconds.
		// That is why this branch needs the track length; without one we start
		// from the beginning instead of guessing (a fraction computed from a
		// zero length would rewind the track to the end and play silence).
		if (offsetSeconds > 0 && m_MR_Duration > 0)
		{
			float fraction = (float)offsetSeconds / (float)m_MR_Duration;
			m_MR_Sound.MR_PlayWithOffset(MR.ClampFloat(fraction, 0, 0.999));
		}
		else if (offsetSeconds > 0)
		{
			MR.LogWarn("AudioStart: resume requested but track length is unknown, starting at 0:00");
		}

		m_MR_Sound.SetAutodestroy(true);

		// Per-host ceiling: a walkman must not be as loud as a car stereo.
		// Set ONCE here instead of multiplying the tick value, so the engine's
		// fade in / fade out keeps working (VOS_Radio's SetMaxVolume idea).
		m_MR_Sound.MR_SetVolumeMax(MR_GetVolumeCeiling());

		m_MR_LastAppliedVolume = -1;
		MR_AudioApplyVolume();
		return true;
	}

	void MR_AudioStop(bool immediate = false)
	{
		if (m_MR_Sound)
		{
			m_MR_Sound.SoundStop();
			m_MR_Sound = null;
		}
		m_MR_SoundSet = string.Empty;
		m_MR_LastAppliedVolume = -1;
	}

	bool MR_AudioIsPlaying()
	{
		return (m_MR_Sound != null && m_MR_Sound.IsPlaying());
	}

	// Volume = user volume x distance falloff, recomputed while the player
	// moves (called every tick by MR_ClientAudio).
	void MR_AudioApplyVolume()
	{
		if (!m_MR_Sound)
			return;

		float gain = MR.VolumeStepToGain(m_MR_Volume);
		float radius = MR_GetEffectiveRadius();

		Object anchor = MR_GetAudioObject();
		Object listener = GetGame().GetPlayer();
		if (anchor && listener)
		{
			float dist = vector.Distance(anchor.GetPosition(), listener.GetPosition());
			gain = gain * MR.DistanceGain(dist, radius);
		}

		int stepped = Math.Round(gain * 100);
		if (stepped == m_MR_LastAppliedVolume)
			return;

		m_MR_LastAppliedVolume = stepped;
		m_MR_Sound.SetSoundVolume(gain);
	}

	// ------------------------------------------------------------------- sync
	// Serialise the current state for the "late join" / periodic sync RPC.
	string MR_StateLine()
	{
		TStringArray parts = new TStringArray();
		parts.Insert(MR_Track.BoolJson(m_MR_IsPlaying));
		parts.Insert(m_MR_TrackId);
		parts.Insert(m_MR_TrackTitle);
		parts.Insert(m_MR_Provider);
		parts.Insert(m_MR_Volume.ToString());
		parts.Insert(m_MR_Radius.ToString());
		parts.Insert(MR_GetElapsedSeconds().ToString());
		return MR_Net.JoinParts(parts);
	}

	static void MR_ParseStateLine(string line, out bool playing, out string trackId, out string title, out string provider, out int volume, out float radius, out int offset)
	{
		playing = false;
		trackId = string.Empty;
		title = string.Empty;
		provider = string.Empty;
		volume = 6;
		radius = MR.RADIUS_DEFAULT;
		offset = 0;

		if (line == string.Empty)
			return;

		TStringArray parts = new TStringArray();
		line.Split("|", parts);
		if (parts.Count() < 7)
			return;

		playing  = MR.ToBool(parts.Get(0));
		trackId  = parts.Get(1);
		title    = parts.Get(2);
		provider = parts.Get(3);
		volume   = parts.Get(4).ToInt();
		radius   = parts.Get(5).ToFloat();
		offset   = parts.Get(6).ToInt();
	}
}
