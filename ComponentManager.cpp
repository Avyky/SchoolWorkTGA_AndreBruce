#include "ComponentManager.h"
#include "Entity.h"
#include "Component.h"


void ECS::ComponentManagerBase::AddComponentInternal(Entity* aEntity, Component* aComponent, const std::type_index aTypeIndex)
{
	aComponent->SetOwner(aEntity);
	
	for (size_t i = 0; i < 8; i++) // TODO: Make define for size in entity
	{
		if (aEntity->myComponents[i].myComponent == nullptr)
		{
			aEntity->myComponents[i].myType = aTypeIndex;
			aEntity->myComponents[i].myComponent = (void*)aComponent;
			
			return;
		}
	}
}

void ECS::ComponentManagerBase::RemoveComponentInternal(Entity* aEntity, Component* aComponent)
{
	aComponent->SetOwner(nullptr);
	for (size_t i = 0; i < 8; i++) // TODO: Make define for size in entity
	{
		if (aEntity->myComponents[i].myComponent == (void*)aComponent)
		{
			aEntity->myComponents[i].myComponent = nullptr;
			return;
		}
	}
}
