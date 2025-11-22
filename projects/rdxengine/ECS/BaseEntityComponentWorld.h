#ifndef BASEENTITYCOMPONENTWORLD_H
#define BASEENTITYCOMPONENTWORLD_H
#include "rxdebug.h"
#include "BaseService.h"
#include "Components/Component.h"

namespace rdx
{
	inline ViewSetID GenerateViewSetID()
	{
		static ViewSetID counter = 0;
		return counter++;
	}

	template <typename ...Cs>
	ViewSetID GetViewSetID()
	{
		static ViewSetID viewSetID = GenerateViewSetID();
		return viewSetID;
	}

	class Entity; // Forward declare is enough for function signature below

	// For-each alias
	template <typename ...Cs>
	using ViewEachFn = std::function<void(EntityID, Cs...)>;

	// Factory function containing the CRTP casting
	template <typename ...Cs>
	using ViewFactoryFn = std::function<void(ViewEachFn<Cs...>)>;

	// Variant being used by the BaseECSWorld
	using ViewFactoryVariant = std::variant<
		ViewFactoryFn<>,
		ViewFactoryFn<TransformComponent&>
	>;

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
			RX_ASSERT_MSG(it != m_ComponentFactories.end(), "Component T not registered.");

			ComponentFactory& cf = it->second;
			return cf.HasComponent(eid);
		}

		template <typename T>
		T* AddComponent(EntityID const& eid)
		{
			ComponentID const cid = GetComponentID<T>();

			if (!HasEntity(eid))
				return nullptr;

			auto it = m_ComponentFactories.find(cid);
			RX_ASSERT_MSG(it != m_ComponentFactories.end(), "Component T not registered.");

			ComponentFactory& cf = it->second;
			return cf.HasComponent(eid) ?
				static_cast<T*>(cf.GetComponent(eid)) :
				static_cast<T*>(cf.AddComponent(eid));
		}

		template <typename T>
		T* GetComponent(EntityID const& eid)
		{
			ComponentID const cid = GetComponentID<T>();

			if (!HasEntity(eid))
				return nullptr;

			auto it = m_ComponentFactories.find(cid);
			RX_ASSERT_MSG(it != m_ComponentFactories.end(), "Component T not registered.");

			ComponentFactory& cf = it->second;
			return cf.HasComponent(eid) ?
				static_cast<T*>(cf.GetComponent(eid)) :
				nullptr;
		}

		template <typename ...Cs>
		auto View(std::function<void(EntityID, Cs...)> fnEach)
		{
			//RX_ASSERT(GetDispatch());
			//return GetDispatch()->View<Cs...>();

			//static AutoRegister<Cs...> s_reg{};

			ViewSetID const vid = GetViewSetID<Cs...>();
			
			auto it = m_ViewFactories.find(vid);
			RX_ASSERT_MSG(it != m_ViewFactories.end(), "View Cs... not registered.");
			ViewFactoryVariant& vfVariant = it->second;

			return std::visit(
				[&](auto& factory) -> void
				{
					using F = std::decay_t<decltype(factory)>;
					using ExpectedF = ViewFactoryFn<Cs...>;
					if constexpr (std::is_same_v<F, ExpectedF>)
					{
						factory(fnEach);
					}
					else
					{
						RX_ASSERT_MSG(false, "View signature mismatch.");
					}
				}, vfVariant);
		}

	protected:
		inline static EntityID s_EntityCounter{ 0 };
		std::map<ComponentID, ComponentFactory> m_ComponentFactories{};
		std::map<ViewSetID, ViewFactoryVariant> m_ViewFactories{};
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

			// Register Variants
			RegisterViewSet<>();
			RegisterViewSet<TransformComponent&>();

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

		template <typename ...Cs>
		void RegisterViewSet()
		{
			auto& viewFactory = m_ViewFactories[GetViewSetID<Cs...>()];
			viewFactory = ViewFactoryFn<Cs...>{
				[this](ViewEachFn<Cs...> const& fnEach) -> void
				{
					static_cast<DerivedT*>(this)->template ViewImpl<Cs...>(fnEach);
				}
			};
		}
	};
}

#endif