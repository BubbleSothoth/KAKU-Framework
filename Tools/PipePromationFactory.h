#pragma once
#include "Pipe.h"
#include "PromationFactory.h"
class PipePromationFactory :
    public PromationFactory<unsigned int,void*>
{
public:
    PipePromationFactory(void);
    ~PipePromationFactory(void);
    //����һ���Ѿ��������ķ������������̳��Ը��ࣩ����ʹ��create_new()��������
    inline volatile void* create(unsigned int) = delete;
    void create_new(unsigned int);
};

