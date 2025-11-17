#ifndef BASEENTITYCOMPONENTWORLD_H
#define BASEENTITYCOMPONENTWORLD_H
#include "BaseService.h"
#include "Components/Component.h"

namespace rdx
{
	class Entity; // Forward declare is enough for function signature below

	// This class handles error, null, ..., safety checks.
	// The derived class's _Impl functions simply do ONE job, no checks.
	class BaseEntityComponentWorld : public BaseService
	{
	public:
		inline static EntityID GenerateEntityID()
		{
			return ++s_EntityCounter;
		}

	private:
		struct ComponentFactory
		{
			std::function<bool(EntityID const)> HasComponent;
			std::function<void* (EntityID const)> AddComponent;
			std::function<void* (EntityID const)> GetComponent;
		};

	public:
		virtual ~BaseEntityComponentWorld() {};

		virtual Entity CreateEntity() = 0;
		virtual bool HasEntity(EntityID const) = 0;

		// This base class assumes that the following functions have been templated appropriately.
		//virtual bool HasComponentImpl(ComponentID const, EntityID const) = 0;
		//virtual void* const AddComponentImpl(ComponentID const, EntityID const) = 0;
		//virtual void* const GetComponentImpl(ComponentID const, EntityID const) = 0;

		template <typename T>
		bool HasComponent(EntityID const& eid)
		{
			ComponentID const cid = GetComponentID<T>();

			if (!HasEntity(eid))
				return false;

			auto it = m_ComponentFactories.find(cid);
			if (it == m_ComponentFactories.end())
				return false;

			ComponentFactory& cf = it->second;
			return cf.HasComponent(eid);
		}

		template <typename T>
		std::optional<std::reference_wrapper<T>> AddComponent(EntityID const& eid)
		{
			ComponentID const cid = GetComponentID<T>();

			if (!HasEntity(eid))
				return std::nullopt;

			auto it = m_ComponentFactories.find(cid);
			if (it == m_ComponentFactories.end())
				return std::nullopt;

			ComponentFactory& cf = it->second;
			if (cf.HasComponent(eid))
			{
				T* pComp = static_cast<T*>(cf.GetComponent(eid));
				return pComp ? std::optional<std::reference_wrapper<T>>{*pComp} : std::nullopt;
			}

			T* pComp = static_cast<T*>(cf.AddComponent(eid));
			return pComp ? std::optional<std::reference_wrapper<T>>{*pComp} : std::nullopt;
		}

		template <typename T>
		std::optional<std::reference_wrapper<T>> GetComponent(EntityID const& eid)
		{
			ComponentID const cid = GetComponentID<T>();

			if (!HasEntity(eid))
				return std::nullopt;

			auto it = m_ComponentFactories.find(cid);
			if (it == m_ComponentFactories.end())
				return std::nullopt;

			ComponentFactory& cf = it->second;
			if (!cf.HasComponent(eid))
				return std::nullopt;

			T* pComp = static_cast<T*>(cf.GetComponent(eid));
			return pComp ? std::optional<std::reference_wrapper<T>>{*pComp} : std::nullopt;
		}

	protected:
		inline static EntityID s_EntityCounter{ 0 };
		std::map<ComponentID, ComponentFactory> m_ComponentFactories{};
	};

	template <typename DerivedT>
	class BaseEntityComponentWorldT : public BaseEntityComponentWorld
	{
	public:
		virtual bool InitWorld() = 0; // Second phase of InitImpl. I want BaseWorld to have its own InitImpl, so bopian
		bool InitImpl() override final
		{
			// Register Components
			RegisterComponent<TransformComponent>();

			return InitWorld();
		}

	private:
		// Not really greate because this has a double indirection, and also has to assume that AddComponentImplT exists in the derived class.
		// But to some degree, it is the same logic as TranslateKey, where each derived implicitly has to do that one way or another.
		template <typename T>
		void RegisterComponent()
		{
			auto& compFactory = m_ComponentFactories[GetComponentID<T>()];
			compFactory.HasComponent =
				[this](EntityID const eid) -> bool
				{
					return static_cast<DerivedT*>(this)->template HasComponentImpl<T>(eid);
				};
			compFactory.AddComponent = 
				[this](EntityID const eid) -> void*
				{
					return static_cast<DerivedT*>(this)->template AddComponentImpl<T>(eid);
				};
			compFactory.GetComponent = 
				[this](EntityID const eid) -> void*
				{
					return static_cast<DerivedT*>(this)->template GetComponentImpl<T>(eid);
				};
		}
	};
}

#endif