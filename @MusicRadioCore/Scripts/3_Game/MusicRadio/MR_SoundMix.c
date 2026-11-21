// ============================================================================
//  MusicRadio - Core
//  MR_SoundMix.c : per-sound ceiling on top of the global mix.
//  Layer: 3_Game
//
//  Straight from VOS_Radio (scripts/3_Game/EffectSound.c) - three lines that
//  replace what we were doing wrong. Until now MR_DeviceBase.MR_AudioApplyVolume
//  recomputed gain and called SetSoundVolume() every 250 ms tick, which:
//    * fights with the engine's own fade in / fade out,
//    * overwrites any distance curve a SoundSet declares in config,
//    * writes a new value even when nothing moved (the "stepped" guard helps,
//      but the value still walks around zero).
//
//  Two instruments instead of one:
//    m_SoundWaveVolumeMax  -> the ceiling of ONE sound (walkman quieter than a
//                             car stereo). Set once when playback starts.
//    SetSoundVolume(0..1)  -> the live distance attenuation, still per tick,
//                             but now it scales a ceiling instead of ignoring it.
// ============================================================================

modded class EffectSound
{
	// Ceiling for this sound only. 0..1 of the configured volumeFactor.
	// Kept as a separate method (not an override) so vanilla behaviour and
	// other mods that patch EffectSound are untouched.
	void MR_SetVolumeMax(float volume)
	{
		m_SoundWaveVolumeMax = MR.Clamp01(volume);
	}

	float MR_GetVolumeMax()
	{
		return m_SoundWaveVolumeMax;
	}

	// ---------------------------------------------------------- resume / seek
	// EffectSound is a WRAPPER: it exposes SoundPlay/SoundStop but not the
	// AbstractWave methods. SetStartOffset lives on m_SoundWaveObject and is
	// reachable only from here - effectSound.c keeps it protected and gives no
	// accessor. OMD_CarRadio patched EffectSound for exactly this reason.
	// Cost of not doing it: "Undefined function 'EffectSound.SetStartOffset'"
	// at compile time, which is what the 17:46 log showed.
	bool MR_HasWave()
	{
		return (m_SoundWaveObject != null);
	}

	// fraction: 0..1 of the wave. 0 = start, 0.5 = middle.
	// Returns false when the wave is not built yet (nothing to seek in).
	bool MR_SetStartOffset(float fraction)
	{
		if (!m_SoundWaveObject)
			return false;

		m_SoundWaveObject.SetStartOffset(MR.ClampFloat(fraction, 0, 0.999));
		return true;
	}

	// Offset must be applied after the wave exists and before it is audible,
	// otherwise the listener hears the head of the track and then a jump.
	// Order matters: SoundPlay() -> SetStartOffset() -> volume.
	bool MR_PlayWithOffset(float fraction)
	{
		bool started = SoundPlay();
		if (!started)
			return false;

		MR_SetStartOffset(fraction);
		return true;
	}

	// Kept for parity with the OMD helper: same guard, same meaning.
	void MR_SetSoundVolume(float volume)
	{
		if (!m_SoundWaveObject)
			return;

		m_SoundWaveObject.SetVolume(MR.Clamp01(volume));
	}
}
