#include "mcspch.hpp"
#include "MediaSystem.h"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string MediaSystem::NAME = "MediaManager";

	void MCS::MediaSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMediaManager>(), true);

		m_World = Frosty::Application::Get().GetWorld().get();
		m_SoundEngine = Frosty::AssetManager::GetSoundEngine();

		playMusic("assets/music/Atmo.mp3", 1.0f);
		playMusic("assets/music/menusong.mp3", 1.0f);
	}

	void MCS::MediaSystem::OnUpdate()
	{

	}

	void MCS::MediaSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::PlayMedia:
			OnPlayMediaEvent(static_cast<Frosty::PlayMediaEvent&>(e));
			break;
		case Frosty::EventType::PlayMediaEntity:
			OnPlayMediaEntityEvent(static_cast<Frosty::PlayMediaEntityEvent&>(e));
			break;
		case Frosty::EventType::PlayMusic:
			OnPlayMusicEvent(static_cast<Frosty::PlayMusicEvent&>(e));
			break;
		case Frosty::EventType::StopMedia:
			OnStopMediaEvent(static_cast<Frosty::StopMediaEvent&>(e));
			break;
		default:
			break;
		}
	}

	void MCS::MediaSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_MediaManager[p_Total] = &world->GetComponent<Frosty::ECS::CMediaManager>(entity);

			p_Total++;
		}
	}

	void MCS::MediaSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_MediaManager[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void MediaSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CMediaManager* mediaManagerPtr = world->GetComponentAddress<Frosty::ECS::CMediaManager>(entity);

			m_Transform[it->second] = transformPtr;
			m_MediaManager[it->second] = mediaManagerPtr;
		}
	}

	std::string MCS::MediaSystem::GetInfo() const
	{
		std::stringstream retInfo;
		retInfo << "\t-----------" << NAME << " System Info-----------\n";
		retInfo << "\t\t---------Entity Map---------\n";
		retInfo << "\t\tEntity Id\tEntity Address\t\tEntity Refs\tArray Index\n";
		for (auto& em : p_EntityMap)
		{
			retInfo << "\t\t" << em.first->Id << "\t\t" << em.first << "\t\t" << em.first.use_count() << "\t" << em.second << "\n";
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t\t------Component Array(s)------\n";
		retInfo << "\n\t\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < p_Total; i++)
		{
			retInfo << "\t\t" << i << "\t" << m_Transform[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_Transform[i]->EntityPtr << "\t\t" << m_Transform[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_MediaManager[i] << "\t" << m_MediaManager[i]->EntityPtr->Id << "\t\t" << m_MediaManager[i]->EntityPtr << "\t\t" << m_MediaManager[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void MediaSystem::OnPlayMusicEvent(Frosty::PlayMusicEvent& e)
	{
		const char* filepath = e.GetFilepath();
		playMusic(filepath, true);
	}

	void MCS::MediaSystem::OnPlayMediaEvent(Frosty::PlayMediaEvent& e)
	{
		const char* filepath = e.GetFilepath();
		int effectID = e.GetEffectID();
		float volume = e.GetVolume();
		bool enableSoundEffects = e.IsSoundEffectsEnabled();
		play2DSound(filepath, false, false, false, enableSoundEffects, volume, effectID);
	}

	void MediaSystem::OnPlayMediaEntityEvent(Frosty::PlayMediaEntityEvent& e)
	{
		auto& componentPos = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetEntity()).Position;
		const char* filepath = e.GetFilepath();
		float volume = e.GetVolume(); float minDist = e.GetMinDistance(); float maxDist = e.GetMaxDistance();
		int effectID = e.GetEffectID();
		play3DSound(filepath, vec3df(componentPos.x, componentPos.y, componentPos.z), false, false, false, true, volume, minDist, maxDist, effectID);
	}

	void MediaSystem::OnStopMediaEvent(Frosty::StopMediaEvent& e)
	{
		stopAllSounds();
	}

	void MediaSystem::playMusic(const char* source, bool loop)
	{
		m_SoundEngine->play2D(source, loop, false, false, ESM_AUTO_DETECT, false);
	}

	void MediaSystem::play2DSound(const char* source, bool loop, bool startPaused, bool track, bool enableSoundEffects, float volume, int effectID)
	{
		m_SoundEngine->setSoundVolume(volume);
		ISound* sound2D = m_SoundEngine->play2D(source, loop, startPaused, track, ESM_AUTO_DETECT, enableSoundEffects);
		
		if (enableSoundEffects)
		{
			if (sound2D)
			{
				ISoundEffectControl* fx = sound2D->getSoundEffectControl();
				if (fx)
				{
					if (effectID == 1) fx->enableChorusSoundEffect();
					if (effectID == 2) fx->enableCompressorSoundEffect();
					if (effectID == 3) fx->enableDistortionSoundEffect();
					if (effectID == 4) fx->enableEchoSoundEffect();
					if (effectID == 5) fx->enableFlangerSoundEffect();
					if (effectID == 6) fx->enableGargleSoundEffect();
					if (effectID == 7) fx->enableI3DL2ReverbSoundEffect();
					if (effectID == 8) fx->enableParamEqSoundEffect();
					if (effectID == 9) fx->enableWavesReverbSoundEffect();
				}
			}
		}
	}

	void MediaSystem::play3DSound(const char* source, vec3df position, bool loop, bool startPaused, bool track, bool enableSoundEffects, float volume, float minDist, float maxDist, int effectID)
	{
		m_SoundEngine->setDefault3DSoundMinDistance(minDist);
		m_SoundEngine->setDefault3DSoundMaxDistance(maxDist); // So the sound reaches the camera, else its really quiet by default
		m_SoundEngine->setSoundVolume(volume);
		ISound* sound3D = m_SoundEngine->play3D(source, position, loop, startPaused, track, ESM_AUTO_DETECT, enableSoundEffects);

		if (enableSoundEffects)
		{
			if (sound3D)
			{
				ISoundEffectControl* fx = sound3D->getSoundEffectControl();
				if (fx)
				{
					if (effectID == 1) fx->enableChorusSoundEffect();
					if (effectID == 2) fx->enableCompressorSoundEffect();
					if (effectID == 3) fx->enableDistortionSoundEffect();
					if (effectID == 4) fx->enableEchoSoundEffect();
					if (effectID == 5) fx->enableFlangerSoundEffect();
					if (effectID == 6) fx->enableGargleSoundEffect();
					if (effectID == 7) fx->enableI3DL2ReverbSoundEffect();
					if (effectID == 8) fx->enableParamEqSoundEffect();
					if (effectID == 9) fx->enableWavesReverbSoundEffect();
				}
			}
		}
	}

	void MediaSystem::stopAllSounds()
	{
		m_SoundEngine->stopAllSounds();
	}
}