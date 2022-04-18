#pragma once
#include "MemoryPool.h"
#include "Entity.h"

#ifdef _DEBUG
#include "ErrorHandler.h"
#endif

#pragma region wtf
#include <type_traits>

// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.

template<typename, typename T>
struct HasUpdateFunction {
	static_assert(
		std::integral_constant<T, false>::value,
		"Second template parameter needs to be of function type.");
};

// specialization that does the checking

template<typename C, typename Ret, typename... Args>
struct HasUpdateFunction<C, Ret(Args...)> {
private:
	template<typename T>
	static constexpr auto check(T*)
		-> typename
		std::is_same<
		decltype(std::declval<T>().Update(std::declval<Args>()...)),
		Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		>::type;  // attempt to call it and see if the return type is correct

	template<typename>
	static constexpr std::false_type check(...);

	typedef decltype(check<C>(0)) type;

public:
	static constexpr bool value = type::value;
};

template<typename, typename T>
struct HasInitFunction {
	static_assert(
		std::integral_constant<T, false>::value,
		"Second template parameter needs to be of function type.");
};
template<typename C, typename Ret, typename... Args>
struct HasInitFunction<C, Ret(Args...)> {
private:
	template<typename T>
	static constexpr auto check(T*)
		-> typename
		std::is_same<
		decltype(std::declval<T>().Init(std::declval<Args>()...)),
		Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		>::type;  // attempt to call it and see if the return type is correct

	template<typename>
	static constexpr std::false_type check(...);

	typedef decltype(check<C>(0)) type;

public:
	static constexpr bool value = type::value;
};

#pragma endregion

namespace ECS
{
	class Entity;
	class Component;
	class ComponentManagerBase {
	public:
		virtual ~ComponentManagerBase() = default;
		virtual void AddComponent(Entity* aEntity) = 0;
		virtual void RemoveComponent(Entity* aEntity) = 0;
		virtual void UpdateComponents(const float aDt) = 0;
		virtual bool HasUpdate() = 0;
		virtual void ClearCmp() = 0;
	protected:
		void AddComponentInternal(Entity* aEntity, Component* aComponent, const std::type_index aTypeIndex);
		void RemoveComponentInternal(Entity* aEntity, Component* aComponent);

	};

	template<class ComponentT, size_t numComponents>
	class ComponentManager : public ComponentManagerBase
	{
	public:
		void UpdateComponents(const float aDt) override;
		void AddComponent(Entity* aEntity) override;
		void RemoveComponent(Entity* aEntity) override;
		bool HasUpdate() override;
		void ClearCmp() override;
	private:
		MemoryPool<ComponentT, numComponents> myComponents;
	};


	template<class ComponentT, size_t numComponents>
	inline void ComponentManager<ComponentT, numComponents>::UpdateComponents(const float aDt)
	{
		if constexpr (HasUpdateFunction<ComponentT, void(const float)>::value)
		{
			if (myComponents.Size() == 0)
			{
				return;
			}
			for (auto& component : myComponents)
			{
				component.Update(aDt);
			}
			
		}
	}
	template<class ComponentT, size_t numComponents>
	inline void ComponentManager<ComponentT, numComponents>::AddComponent(Entity* aEntity)
	{
#ifdef _DEBUG
		if (aEntity->GetComponent<ComponentT>() != nullptr)
		{
			ErrorSystem::ErrorHandler::GetInstance()->ThrowErrorException("ERROR: Dont try and add the same component twice...");
		}
#endif
		ComponentT* const component = myComponents.Create();
		AddComponentInternal(aEntity, (Component*)component, std::type_index(typeid(ComponentT)));
		if constexpr (HasInitFunction<ComponentT, void()>::value)
		{
			component->Init();
		}
	}
	template<class ComponentT, size_t numComponents>
	inline void ComponentManager<ComponentT, numComponents>::RemoveComponent(Entity* aEntity)
	{
		ComponentT* comp = aEntity->GetComponent<ComponentT>();
#ifdef _DEBUG
		if (nullptr == comp) // the yoda condition
		{
			ErrorSystem::ErrorHandler::GetInstance()->ThrowErrorException("ERROR: Dont try and remove a component that was never added to the entity ;(");
		}
#endif //_DEBUG

		RemoveComponentInternal(aEntity, (Component*)comp);
		myComponents.Remove(comp);
		//myComponents.Clear();
	}
	template<class ComponentT, size_t numComponents>
	inline bool ComponentManager<ComponentT, numComponents>::HasUpdate()
	{
		return HasUpdateFunction<ComponentT, void(const float)>::value;
	}
	template<class ComponentT, size_t numComponents>
	inline void ComponentManager<ComponentT, numComponents>::ClearCmp()
	{
		myComponents.Clear();
	}
}


