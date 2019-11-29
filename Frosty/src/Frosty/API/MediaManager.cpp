#include <fypch.hpp>
#include "MediaManager.h"

Frosty::MediaManager::MediaManager()
{
	m_SoundEngine = createIrrKlangDevice();
}

Frosty::MediaManager::~MediaManager()
{
	m_SoundEngine->drop();
}

ISound* Frosty::MediaManager::playMusic(const char* source, bool loop)
{
	m_Music = m_SoundEngine->play2D(source, loop);
	//m_Music->setVolume(0.5f);

	return m_Music;
}

ISound* Frosty::MediaManager::play2DSound(const char* fileName, bool loop, bool startPaused, bool track, bool enableSoundEffects, SoundEffects soundEffect)
{
	m_Sound = m_SoundEngine->play2D(fileName, loop, startPaused, track, ESM_AUTO_DETECT, enableSoundEffects);
	//setSoundEffect(m_Sound, soundEffect);
	return m_Sound;
}

ISound* Frosty::MediaManager::play3DSound(const char* fileName, vec3df position, bool loop, bool startPaused, bool track, bool enableSoundEffects, SoundEffects soundEffect)
{
	/*ISound* snd = m_SoundEngine->play3D(fileName, position, loop, startPaused, track, ESM_AUTO_DETECT, enableSoundEffects);
	if (snd) {
		ISoundEffectControl* fx = m_Sound->getSoundEffectControl();
		if (fx) {
			fx->enableEchoSoundEffect();
		}
	}

	return snd;*/
	
	m_Sound = m_SoundEngine->play3D(fileName, position, loop, startPaused, track, ESM_AUTO_DETECT, enableSoundEffects);
	//setSoundEffect(m_Sound, soundEffect);
	return m_Sound;
}

ISound* Frosty::MediaManager::setSoundEffect(SoundEffects soundEffect)
{
	/*ISoundEffectControl* fx = m_Sound->getSoundEffectControl();
	if (fx)
	{
		switch (m_SoundEffects) {
		case Chorus:
			fx->enableChorusSoundEffect();
			break;
		case Compressor:
			fx->enableCompressorSoundEffect();
			break;
		case Distortion:
			fx->enableDistortionSoundEffect();
			break;
		case Echo:
			fx->enableEchoSoundEffect();
			break;
		case Flanger:
			fx->enableFlangerSoundEffect();
			break;
		case Gargle:
			fx->enableGargleSoundEffect();
			break;
		case Reverb:
			fx->enableI3DL2ReverbSoundEffect();
			break;
		case ParamEq:
			fx->enableParamEqSoundEffect();
			break;
		case WavesReverb:
			fx->enableWavesReverbSoundEffect();
			break;
		}
	}*/

	return m_Sound;
}

ISound* Frosty::MediaManager::removeSoundEffect(SoundEffects soundEffect)
{
	/*ISoundEffectControl* fx = m_Sound->getSoundEffectControl();
	if (fx)
	{
		switch (m_SoundEffects) {
		case Chorus:
			fx->disableChorusSoundEffect();
			break;
		case Compressor:
			fx->disableCompressorSoundEffect();
			break;
		case Distortion:
			fx->disableDistortionSoundEffect();
			break;
		case Echo:
			fx->disableEchoSoundEffect();
			break;
		case Flanger:
			fx->disableFlangerSoundEffect();
			break;
		case Gargle:
			fx->disableGargleSoundEffect();
			break;
		case Reverb:
			fx->disableI3DL2ReverbSoundEffect();
			break;
		case ParamEq:
			fx->disableParamEqSoundEffect();
			break;
		case WavesReverb:
			fx->disableWavesReverbSoundEffect();
			break;
		}
	}*/

	return m_Sound;
}

ISound* Frosty::MediaManager::setSoundDistance(ik_f32 minDist, ik_f32 maxDist)
{
	/*m_Sound->setMinDistance(minDist);
	m_Sound->setMaxDistance(maxDist);*/
	return m_Sound;
}

ISound* Frosty::MediaManager::setMusicVolume(ik_f32 volume)
{
	/*m_Music->setVolume(volume);*/
	return m_Music;
}

ISound* Frosty::MediaManager::setSoundVolume(ik_f32 volume)
{
	/*m_Sound->setVolume(volume);*/
	return m_Sound;
}

void Frosty::MediaManager::stopAllSounds()
{
	m_SoundEngine->stopAllSounds();
}