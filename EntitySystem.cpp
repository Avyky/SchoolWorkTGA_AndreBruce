#include "EntitySystem.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "AudioTriggerComponent.h"
#include "AudioListenerComponent.h"
#include "AudioSourceComponent.h"
#include "EnemyComponent.h"
#include "HurtBoxComponent.h"
#include "ControllerComponent.h"
#include "HealthComponent.h"
#include "Entity.h"
#include "WarningLightComponent.h"
#include "PlayerComponent.h"
#include "Application.h"
#include "SceneManager.h"
#include "TextComponent.h"
#include "ButtonComponent.h"
#include "WeaponComponent.h"
#include "KeycardComponent.h"
#include "DoorComponent.h"
#include "ObjectiveComponent.h"
#include "PoweredDoorComponent.h"
#include "PickupComponent.h"
#include "ModelComponent.h"
#include "FlickeringLight.h"
#include "ThreadPool.h"
#include "SettingsButtonsComponent.h"
#include "../Game/MeleeComponent.h"
#include "../Game/PlayerWeaponComponent.h"
#include "../Game/PowerBoxComponent.h"
#include "../Game/InteractComponent.h"



ECS::EntitySystem::~EntitySystem()
{
	for (auto [key, value] : myComponentManagerMap)
	{
		delete value;
	}
	myComponentManagerMap.clear();
}
void ECS::EntitySystem::Init(Application* const aApp)
{
	//Alla component managers ska läggas till här //Denk
	myApp = aApp;
	InitComponentManager<WarningLightComponent, 32>(true);
	InitComponentManager<TransformComponent, NUMBER_OF_ENTITIES>();
	InitComponentManager<SpriteComponent, 1024>();
	InitComponentManager<AudioTriggerComponent, 128>();
	InitComponentManager<AudioSourceComponent, 128>();
	InitComponentManager<AudioListenerComponent, 5>();
	InitComponentManager<KeycardComponent, 20>();
	InitComponentManager<DoorComponent, 20>();
	InitComponentManager<ObjectiveComponent, 10>();
	InitComponentManager<PoweredDoorComponent, 10>();
	InitComponentManager<PowerBoxComponent, 10>();
	InitComponentManager<PickupComponent, 30>();
	InitComponentManager<InteractComponent, 128>();
	InitComponentManager<FlickeringLight, 128>();


	InitComponentManager<PlayerWeaponComponent, 2>();
	InitComponentManager<PlayerComponent, 4>();
	InitComponentManager<EnemyComponent, 128>();
	InitComponentManager<ControllerComponent, 128>();
	InitComponentManager<HurtBoxComponent, 256>();
	InitComponentManager<HealthComponent, 128>();
	InitComponentManager<TextComponent, 512>();
	InitComponentManager<SettingsButtonsComponent, 64>();

	InitComponentManager<ButtonComponent, 512>();
	InitComponentManager<WeaponComponent, 128>();
	InitComponentManager<SettingsButtonsComponent, 128>();
	InitComponentManager<MeleeComponent, 64>();
	InitComponentManager<ModelComponent, 64>();


}
std::mutex ecsGuard;
void ECS::EntitySystem::Update(const float aDt)
{
	if (myApp->GetSceneManager()->GetIsLoadingNewScene()) return;

	std::atomic<int> jobs = myThreadedComponentManagers.size();
	for (size_t i = 0; i < myThreadedComponentManagers.size(); i++)
	{
		ThreadPool::GetInstance().QueueWork([this, aDt, &jobs,i]() {
			std::lock_guard<std::mutex> guard(ecsGuard);
			myThreadedComponentManagers[i]->UpdateComponents(aDt);
			jobs--;
			});

	}

	for (size_t i = 0; i < myComponentManagers.size(); i++)
	{
		myComponentManagers[i]->UpdateComponents(aDt);
	}
	while (jobs > 0)
	{
		ThreadPool::GetInstance().TakeWork();
	}
}
ECS::Entity* ECS::EntitySystem::CreateEntity()
{
	auto* entity = myEntitys.Create();
	entity->myES = this;
	entity->AddComponent<TransformComponent>();
	return entity;
}
void ECS::EntitySystem::RemoveEntity(Entity* aEntity)
{
	//aEntity->RemoveAllComponents();
	myEntitys.Remove(aEntity);
}
void ECS::EntitySystem::ClearAllEntities()
{
	for (size_t i = 0; i < myComponentManagers.size(); i++)
	{
		myComponentManagers[i]->ClearCmp();
	}
	myEntitys.Clear();
}
void ECS::EntitySystem::RemoveComponent(Entity* aEntity, std::type_index aTypeIndex)
{
	myComponentManagerMap.at(aTypeIndex)->RemoveComponent(aEntity);
}

void ECS::EntitySystem::AddComponent(Entity* aEntity, std::type_index aTypeIndex)
{
	myComponentManagerMap.at(aTypeIndex)->AddComponent(aEntity);
}