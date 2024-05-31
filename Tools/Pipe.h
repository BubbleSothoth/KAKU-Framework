#pragma once
/*
    这个文件编写了Pipe模块，该模块主要负责系统的一切对外IO行为。
    在该模块的编写过程中参考了Windows中的消息系统。本模块大体上被分为两个部分：
* 1、自运行部分
* 2、被调部分
    其中自运行部分将在RuntimeSystem被作为Inte的一个部分进行调度，自运行部分的主要任务是根据消息队列的内容对消息进行处理，它位于生产者-消费者模型中的消费者
一方。值得一提的是该部分在运行过程中有可能会产生一些子线程，这些子线程被用于负责各个设备的IO。
    被调部分被用于在
*/

#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

#include <vector>
#include <fstream>
#include <iostream>
#include <queue>
#include <mutex>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

typedef unsigned short DeviceId;
typedef char DeviceType;
typedef char RWMode;

//DeviceId
/*
0x0000-0x0fff为特殊设备
0x1000-0x1fff为文件设备
0x2000-0x2fff为套接字设备
*/

#define SptcialDeviceType   (DeviceType)0x0
#define FileDeviceType      (DeviceType)0x1
#define SocketDeviceType    (DeviceType)0x2
#define DefauleDeviceType   (DeviceType)0xF

#define DefaultDevice       (DeviceId)0xFFFF    //默认值，供初始化用
#define NullDevice          (DeviceId)0x0000    //空设备，所有被写入这里的数据会直接消失
#define StdDevice           (DeviceId)0x0001    //标准IO设备，就是屏幕
#define TempFile            (DeviceId)0x1002    //临时文件，用于贮存临时数据
#define OperationLogFile    (DeviceId)0x1003    //操作日志
#define DebugLogFile        (DeviceId)0x1004    //调试日志
#define InputSocketPipe     (DeviceId)0x2005    //输入套接字
#define OutputSocketPipe    (DeviceId)0x2006    //输出套接字

#define GetDeviceType(dev_id) (DeviceType)((DeviceId)dev_id >> 12)  //通过设备id计算设备类型

//RWFlag
#define Unopreationable     (RWMode)0x0
#define __ReadOnly          (RWMode)0x1
#define __WriteOnly         (RWMode)0x2
#define FreeRW              (RWMode)0x3

#define PipeBufferSize  (size_t)(1024 * sizeof(char))

/* “设备（Device）”是一抽象概念，它包含以下三类涉及到IO操作的对象：
* 1、物理设备（屏幕等）
* 2、文件
* 3、套接字
*/
extern "C" DLLAPI struct Device {
    DeviceId id;
    std::string file;
    union 
    {
        std::fstream* stream;
        SOCKET socket;
    };
    RWMode mode;
    //bool stand_open;
};

//IOProperty
#define InputProperty true      //读模式
#define OutputProperty false    //写模式

//Pipe专用线程锁
#define Pipe_LOCK(mtx)      mtx.lock();		//线程上锁
#define Pipe_UNLOCK(mtx)    mtx.unlock()	//线程解锁
#define PipeDev_LOCK()      this->Pipe::dev_mtx.lock()
#define PipeBuf_LOCK()      this->Pipe::buf_mtx.lock()
#define PipeDev_UNLOCK()    this->Pipe::dev_mtx.unlock()
#define PipeBuf_UNLOCK()    this->Pipe::buf_mtx.unlock()
#define PipeDev_AUTOLOCK()  std::lock_guard<std::mutex> atlck(this->Pipe::dev_mtx)
#define PipeBuf_AUTOLOCK()  std::lock_guard<std::mutex> atlck(this->Pipe::buf_mtx)
#define Pipe_AUTOLOCK()     std::lock(this->Pipe::dev_mtx,this->Pipe::buf_mtx); \
                            std::lock_guard<std::mutex> atlck(this->Pipe::dev_mtx,std::adopt_lock); \
                            std::lock_guard<std::mutex> atlck(this->Pipe::buf_mtx,std::adopt_lock);

typedef struct DataBox {
    unsigned short id;
    bool IOProperty;
    union
    {
        DeviceId aim;
        DeviceId from;
    };
    char* data;
    size_t data_size;
    bool IO_successful;
}IO_MESSAGE;

#define IsReadMessage(msg)  (IO_MESSAGE)msg.IOProperty==InputProperty
#define IsWriteMessage(msg) (IO_MESSAGE)msg.IOProperty==OutputProperty

class DLLAPI Pipe
{
private:
    static std::vector<struct Device> device;
    static std::queue<struct DataBox> msg_queue;
    std::mutex dev_mtx,buf_mtx;
public:
    Pipe(void);
    Pipe(const Pipe&) = delete;
    ~Pipe(void);
    //用于向aim写入数据，需要手动调用(将一个写消息放入msg_queue队列等待主流程自动处理)
    void write(DeviceId aim, const char* data);
    //用于向from中读取数据，需要手动调用(将一个读消息放入msg_queue队列等待主流程自动处理)
    void read(DeviceId from, char* buff, size_t size = PipeBufferSize, size_t buffer_size = PipeBufferSize);
    /*安装设备，需要手动调用*/
    DeviceId installDevice(std::string file, RWMode mode, bool stamd_open = false);
    /*卸载设备，需要手动调用*/
    void uninstallDevice(DeviceId);
private:
    /*自动运行的主流程方法，它负责自动处理msg_queue中的消息*/
    void mainProgress(void);
    void uninstallDevice(std::vector<Device>::iterator);
    struct Device creatDevice(std::string file, RWMode mode, bool stand_open = false);
    void startupDevice(DeviceId);
    void startupDevice(Device&);
    void closeDevice(DeviceId);
    void closeDevice(Device&);
    /*所有的substep级方法都需要在外部手动加锁，他们是被提供给mainProgress方法使用的*/
    bool __substep__Input(struct DataBox&, size_t);
    /*所有的substep级方法都需要在外部手动加锁，他们是被提供给mainProgress方法使用的*/
    bool __substep__Output(struct DataBox&);
    /*所有的substep级方法都需要在外部手动加锁，他们是被提供给mainProgress方法使用的*/
    bool __substep__Output__UserDefinedDevice(struct DataBox&, Device&);
    /*所有的substep级方法都需要在外部手动加锁，他们是被提供给mainProgress方法使用的*/
    void __subfunc_Input_Sptcial(DataBox&);
    /*所有的substep级方法都需要在外部手动加锁，他们是被提供给mainProgress方法使用的*/
    void __subfunc_Input_File(DataBox&);
    /*所有的substep级方法都需要在外部手动加锁，他们是被提供给mainProgress方法使用的*/
    void __subfunc_Input_Socket(DataBox&);
    /*注意：除了__getDevice和__setDevice以外的地方不应该使用这个方法！这个方法是仅为了get/set功能设计的，它涉及到了线程锁*/
    struct Device* __optDevice(char, struct Device* = nullptr, DeviceId = DefaultDevice);
    /*这个方法已被废弃，因为它并未达到预计的效果*/
    const struct Device& __getDevice(DeviceId) = delete;
    /*注意!除了构造函数、析构方法、run()方法以外一切对于device的写操作都应该使用这个方法,因为它内置了线程锁*/
    void __setDevice(struct Device&);
    /*注意：除了__getBuffer和__setBuffer以外的地方不应该使用这个方法！这个方法是仅为了get/set功能设计的，它涉及到了线程锁！*/
    struct DataBox* __optBuffer(char, struct DataBox* = nullptr);
    /*这个方法已被废弃，因为它并未达到预计的效果*/
    const struct DataBox& __getBuffer(void) = delete;
    /*注意!除了构造函数、析构方法、run()方法以外一切对于buffer的写操作都应该使用这个方法,因为它内置了线程锁*/
    void __setBuffer(struct DataBox&);
};