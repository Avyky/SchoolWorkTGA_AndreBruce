#pragma once

namespace ECS
{
	class Entity;
	class Component
	{
	public:

		virtual ~Component() = default;
		__forceinline Entity* GetEntity() { return myOwner; }
		__forceinline void SetOwner(Entity* aOwner) { myOwner = aOwner; }
	private:
		
		friend class ComponentManagerBase;
		Entity* myOwner = nullptr;
	};
}
