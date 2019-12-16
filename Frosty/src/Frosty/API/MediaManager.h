#ifndef MEDIA_MANAGER_H
#define MEDIA_MANAGER_H

#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

using namespace irrklang;

namespace Frosty
{
	class MediaManager
	{
	private:
		ISoundEngine* m_SoundEngine;
		ISound* m_Music;
		ISound* m_Sound;
		//std::array<Frosty::ECS::CMediaManager*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Music;
		//std::array<Frosty::ECS::CMediaManager*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Sound;
		vec3df m_Position;
		//std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform; // Might be able to replace m_Position?
		int deviceNumber; // This is what you use to configure a specific sound

	public:
		enum SoundEffects {
			None,
			Chorus,
			Compressor,
			Distortion,
			Echo,
			Flanger,
			Gargle,
			Reverb,
			ParamEq,
			WavesReverb,
			Count
		} m_SoundEffects;
		
		MediaManager();
		~MediaManager();

		ISound* playMusic(const char* source, bool loop);
		ISound* play2DSound(const char* fileName, bool loop, bool startPaused, bool track, bool enableSoundEffects, SoundEffects soundEffect);
		ISound* play3DSound(const char* fileName, vec3df position, bool loop, bool startPaused, bool track, bool enableSoundEffects, SoundEffects soundEffect);
		ISound* setSoundEffect(SoundEffects soundEffect); // Doesn't work, null pointer
		ISound* removeSoundEffect(SoundEffects soundEffect); // Doesn't work, null pointer
		ISound* setSoundDistance(ik_f32 minDist, ik_f32 maxDist); // Doesn't work, null pointer
		ISound* setMusicVolume(ik_f32 volume); // Doesn't work, null pointer
		ISound* setSoundVolume(ik_f32 volume); // Doesn't work, null pointer
		void stopAllSounds(); // This includes music
	};
}

#endif

