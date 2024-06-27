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
* ģ�壺
* CodeType		Key�������
* ReturnType	Value�������
* InstanceType	ʹ�ø�����������ʵ�������ͣ���ѡ��
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
