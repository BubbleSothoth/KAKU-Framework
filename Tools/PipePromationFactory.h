#pragma once
#include "Pipe.h"
#include "PromationFactory.h"
class PipePromationFactory :
    public PromationFactory<unsigned int,void*>
{
public:
    PipePromationFactory(void);
    ~PipePromationFactory(void);
    //这是一个已经被放弃的方法（尽管它继承自父类），请使用create_new()来代替它
    inline volatile void* create(unsigned int) = delete;
    void create_new(unsigned int);
};

