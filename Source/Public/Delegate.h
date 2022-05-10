#pragma once
#include <map>
#include <functional>

template<typename TRet, typename... TArgs>
struct TBaseDelegate
{
	using TFunctor = std::function<TRet(TArgs...)>;
public:
	TBaseDelegate() = default;

	template<typename T>
	TBaseDelegate(T&& F)
		: Functor(std::forward<T>(F))
	{
	}

	template<typename TLambda>
	static TBaseDelegate CreateLambda(TLambda&& Lambda)
	{
		return TBaseDelegate(std::forward<TLambda>(Lambda));
	}

	void Bind(TFunctor&& InFunctor)
	{
		Functor = std::forward<TFunctor>(InFunctor);
	}

	TRet ExecuteIfBound(TArgs... Args)
	{
		if (Functor)
			return Functor(std::forward<TArgs>(Args)...);
		return TRet();
	}

	TRet Execute(TArgs... Args)
	{
		return Functor(std::forward<TArgs>(Args)...);
	}

	bool IsBound()
	{
		return Functor;
	}

	void Unbind()
	{
		Functor = nullptr;
	}

private:
	TFunctor Functor;
};

using FSimpleDelegate = TBaseDelegate<void>;

#define DECLARE_DELEGATE(DelegateName, ...) using DelegateName = TBaseDelegate<void, __VA_OPT__(,) #__VA_ARGS__>;

struct FDelegateHandle
{
	int32_t Id;
};

template <size_t>
struct TPlaceholders {};

namespace std
{
	template<int N>
	struct is_placeholder<TPlaceholders<N>>
		: public integral_constant<size_t, N + 1>
	{};
}

template<typename TRet, typename... TArgs>
struct TMulticastDelegate
{
	using TFunctor = std::function<TRet(TArgs...)>;

public:

	void Add(TFunctor&& Functor)
	{
		int32_t Id = ++AutoIncrementCounter;
		FunctorMap.insert(std::make_pair(Id, std::forward<TFunctor>(Functor)));
	}

	template<typename TObject, size_t... TIndex>
	TFunctor TMemberFunctionBind(TObject* Object, TRet(TObject::* MemberFunctor)(TArgs...), std::index_sequence<TIndex...>)
	{
		return std::bind(MemberFunctor, Object, TPlaceholders<TIndex>{}...);
	}

	template<typename TObject>
	void AddMemberFunction(TObject* Object, TRet(TObject::* MemberFunctor)(TArgs...))
	{
		int32_t Id = ++AutoIncrementCounter;
		FunctorMap.insert(
			std::make_pair(
				Id,
				TMemberFunctionBind(Object, MemberFunctor, std::make_index_sequence<sizeof...(TArgs)>())
			)
		);
	}

	void Broadcast(TArgs... Args)
	{
		for (auto& Functor : FunctorMap)
		{
			Functor.second(Args...);
		}
	}

	void Clear()
	{
		FunctorMap.clear();
	}

	void Contains(FDelegateHandle& DelegateHandle)
	{
		return FunctorMap.contains(DelegateHandle.Id);
	}

	bool IsBound()
	{
		return !FunctorMap.empty();
	}

	void Remove(FDelegateHandle& DelegateHandle)
	{
		FunctorMap.erase(DelegateHandle.Id);
	}

private:
	int32_t AutoIncrementCounter = 0;
	std::map<int32_t, TFunctor> FunctorMap;
};

using FSimpleMulticastDelegate = TMulticastDelegate<void>;

#define DECLARE_MULTICAST_DELEGATE_WITH_RETVAL(DelegateName, RetVal, ...) using DelegateName = TMulticastDelegate<RetVal, __VA_ARGS__>;
#define DECLARE_MULTICAST_DELEGATE(DelegateName, ...) using DelegateName = TMulticastDelegate<void, __VA_ARGS__>;
