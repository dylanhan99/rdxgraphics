#pragma once

#define RX_SINGLETON_DECLARATION_NOCTOR(Foo)		\
	private:										\
		friend class BaseSingleton<Foo>;			\
	protected:										\
		~Foo() override = default;

#define RX_SINGLETON_DECLARATION(Foo)	\
	RX_SINGLETON_DECLARATION_NOCTOR(Foo)\
	protected:							\
		Foo() = default;				\
	private:							\
		inline static Foo& g{ Foo::GetInstance() };

#define RX_SINGLETON_EXPLICIT(Foo)\
	template class BaseSingleton<Foo>;

template <typename T>
class BaseSingleton
{
public:
	static T& GetInstance()
	{
		static T s_Instance{};
		return s_Instance;
	}

protected:
	BaseSingleton() = default;
	virtual ~BaseSingleton() = default;

	BaseSingleton(const BaseSingleton&) = delete;
	BaseSingleton& operator=(const BaseSingleton&) = delete;
	BaseSingleton(BaseSingleton&&) noexcept = delete;
	BaseSingleton& operator=(const BaseSingleton&&) = delete;
};