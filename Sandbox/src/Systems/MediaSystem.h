#ifndef MEDIA_SYSTEM_H
#define MEDIA_SYSTEM_H

namespace Frosty {
	class PlayMediaEvent;
	class PlayMediaEntityEvent;
	class PlayMusicEvent;
	class StopMediaEvent;
}

namespace MCS
{
	class MediaSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		MediaSystem() = default;
		virtual ~MediaSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CMediaManager*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_MediaManager;
		Frosty::World* m_World{ nullptr };
		irrklang::ISoundEngine* m_SoundEngine;

		void OnPlayMusicEvent(Frosty::PlayMusicEvent& e);
		void OnPlayMediaEvent(Frosty::PlayMediaEvent& e);
		void OnPlayMediaEntityEvent(Frosty::PlayMediaEntityEvent& e);
		void OnStopMediaEvent(Frosty::StopMediaEvent& e);
		void playMusic(const char* source, bool loop);
		void play2DSound(const char* source, bool loop, bool startPaused, bool track, bool enableSoundEffects, float volume, float pan, int effectID);
		void play3DSound(const char* source, vec3df position, bool loop, bool startPaused, bool track, bool enableSoundEffects, float volume, float minDist, float maxDist, int effectID);
		void stopAllSounds(); // This includes music
	};
}

#endif MEDIA_SYSTEM_H
