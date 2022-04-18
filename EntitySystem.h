#pragma once
#include "MemoryPool.h"
#include <typeindex>
#include <unordered_map>
#include "ComponentManager.h"
#include <map>


class Application;

namespace ECS 
{
	class Entity;	
	class EntitySystem
	{
	public:
		static constexpr size_t NUMBER_OF_ENTITIES = 2048;
		EntitySystem() = default;
		~EntitySystem();
		
		void Init(Application* const aApp);
		void Update(const float aDt);
		Entity* CreateEntity();
		void RemoveEntity(Entity*);

		template<class ComponentT, size_t numComponents>
		void InitComponentManager(bool isThreaded = false);

		void ClearAllEntities();


	private:
		friend Entity;
		
		void RemoveComponent(Entity* aEntity, std::type_index aTypeIndex);
		void AddComponent(Entity* aEntity, std::type_index aTypeIndex);

	private:
		Application* myApp = nullptr;
		std::vector<ComponentManagerBase*> myComponentManagers;
		std::vector<ComponentManagerBase*> myThreadedComponentManagers;

		std::unordered_map< std::type_index, ComponentManagerBase* > myComponentManagerMap;
		MemoryPool<Entity, NUMBER_OF_ENTITIES> myEntitys;
	};
	
	template<class ComponentT, size_t numComponents>
	inline void EntitySystem::InitComponentManager(bool isThreaded)
	{
		ComponentManagerBase* cmb = new ComponentManager<ComponentT, numComponents>();
		if (isThreaded)
		{
			if (cmb->HasUpdate())
			{
				myThreadedComponentManagers.push_back(cmb);
			}
			myComponentManagerMap[std::type_index(typeid(ComponentT))] = cmb;
			return;
		}

		if (cmb->HasUpdate())
		{
			myComponentManagers.push_back(cmb);
		}
		myComponentManagerMap[std::type_index(typeid(ComponentT))] = cmb;

	}
}
