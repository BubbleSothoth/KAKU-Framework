#pragma once

#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

#include <map>

#define FuncPoint(func) void (*func)(void)
/*
* 模板：
* CodeType		Key项的类型
* ReturnType	Value项的类型
* InstanceType	使用该类的派生类的实例的类型（可选）
*/
template <typename CodeType, typename ReturnType, typename InstanceType = void>
class DLLAPI PromationFactory
{
protected:
	InstanceType* instance = nullptr;
	std::map<CodeType,ReturnType> switch_list;
public:
	PromationFactory(void) = default;
	PromationFactory(InstanceType* lp) :
		instance(lp)
	{

	}
	PromationFactory(const PromationFactory&) = delete;
	~PromationFactory(void) = default;
	inline ReturnType create(CodeType);
protected:
	inline void Add(CodeType, ReturnType);
};

template<typename CodeType, typename ReturnType, typename InstanceType>
inline ReturnType PromationFactory<CodeType, ReturnType, InstanceType>::create(CodeType code)
{
	typename std::map<CodeType, ReturnType>::iterator it = switch_list.find(code);
	return it == switch_list.end() ? ReturnType() : it->second;
}

template<typename CodeType, typename ReturnType, typename InstanceType>
inline void PromationFactory<CodeType, ReturnType, typename InstanceType>::Add(CodeType code, ReturnType value)
{
	switch_list.insert(std::pair<CodeType, ReturnType>(code, value));
	return;
}
