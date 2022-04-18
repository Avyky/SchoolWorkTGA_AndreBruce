#include "Entity.h"
#include "EntitySystem.h"
ECS::Entity::~Entity()
{
	for (size_t i = 0; i < NUM_COMPONENTS; i++)
	{
		if (myComponents[i].myComponent != nullptr)
		{
			myES->RemoveComponent(this, myComponents[i].myType);
		}
	}
}
Application* ECS::Entity::GetApplication() const
{
	return myES->myApp;
}
void ECS::Entity::RemoveAllComponents()
{
	for (size_t i = 0; i < NUM_COMPONENTS; i++)
	{
		if (myComponents[i].myComponent != nullptr)
		{
			myES->RemoveComponent(this, myComponents[i].myType);
		}
	}
}
void ECS::Entity::AddComponent(const std::type_index aType)
{
	myES->AddComponent(this, aType);
}

void ECS::Entity::RemoveComponent(const std::type_index aType)
{
	myES->RemoveComponent(this, aType);
}

