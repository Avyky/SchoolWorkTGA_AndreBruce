#pragma once
#include <Vector3.hpp>
#include <typeindex>
typedef CU::Vector3<float> Vector3f;
#define NUM_COMPONENTS 16
class Application;
namespace ECS 
{
	class EntitySystem;

	class Entity
	{
	public:
		Entity() = default;
		~Entity();
		Application* GetApplication() const;
		
		template<class ComponentT>
		void AddComponent();
		template<class ComponentT>
		void RemoveComponent();
		template<class ComponentT>
		ComponentT* GetComponent();
	private:
		friend class ComponentManagerBase;
		friend EntitySystem;
		void RemoveAllComponents();
		void AddComponent(const std::type_index aType); // DO NOT TOUCH / Andre Bruce
		void RemoveComponent(const std::type_index aType);
		
		struct ComponentInfo 
		{
			void* myComponent = nullptr;
			std::type_index myType = std::type_index(typeid(void*));
		};

	private:
		EntitySystem* myES = nullptr;
		ComponentInfo myComponents[NUM_COMPONENTS];
	};
	template<class ComponentT>
	inline void Entity::AddComponent()
	{
		AddComponent(std::type_index(typeid(ComponentT)));
	}
	template<class ComponentT>
	inline void Entity::RemoveComponent()
	{
		RemoveComponent(std::type_index(typeid(ComponentT)));
	}
	template<class ComponentT>
	inline ComponentT* Entity::GetComponent()
	{
		const std::type_index type = std::type_index(typeid(ComponentT));
		
		for (size_t i = 0; i < NUM_COMPONENTS; i++)
		{
			if (myComponents[i].myComponent == nullptr)	continue;
			
			if (myComponents[i].myType == type)
			{
				return (ComponentT*)myComponents[i].myComponent;
			}	
		}
		return nullptr;
	}
}

