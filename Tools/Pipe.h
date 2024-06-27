#pragma once
/*
    ����ļ���д��Pipeģ�飬��ģ����Ҫ����ϵͳ��һ�ж���IO��Ϊ��
    �ڸ�ģ��ı�д�����вο���Windows�е���Ϣϵͳ����ģ������ϱ���Ϊ�������֣�
* 1�������в���
* 2����������
    ���������в��ֽ���RuntimeSystem����ΪInte��һ�����ֽ��е��ȣ������в��ֵ���Ҫ�����Ǹ�����Ϣ���е����ݶ���Ϣ���д�����λ��������-������ģ���е�������
һ����ֵ��һ����Ǹò��������й������п��ܻ����һЩ���̣߳���Щ���̱߳����ڸ�������豸��IO��
    �������ֱ�������
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
0x0000-0x0fffΪ�����豸
0x1000-0x1fffΪ�ļ��豸
0x2000-0x2fffΪ�׽����豸
*/

#define SptcialDeviceType   (DeviceType)0x0
#define FileDeviceType      (DeviceType)0x1
#define SocketDeviceType    (DeviceType)0x2
#define DefauleDeviceType   (DeviceType)0xF

#define DefaultDevice       (DeviceId)0xFFFF    //Ĭ��ֵ������ʼ����
#define NullDevice          (DeviceId)0x0000    //���豸�����б�д����������ݻ�ֱ����ʧ
#define StdDevice           (DeviceId)0x0001    //��׼IO�豸��������Ļ
#define TempFile            (DeviceId)0x1002    //��ʱ�ļ�������������ʱ����
#define OperationLogFile    (DeviceId)0x1003    //������־
#define DebugLogFile        (DeviceId)0x1004    //������־
#define InputSocketPipe     (DeviceId)0x2005    //�����׽���
#define OutputSocketPipe    (DeviceId)0x2006    //����׽���

#define GetDeviceType(dev_id) (DeviceType)((DeviceId)dev_id >> 12)  //ͨ���豸id�����豸����

//RWFlag
#define Unopreationable     (RWMode)0x0
#define __ReadOnly          (RWMode)0x1
#define __WriteOnly         (RWMode)0x2
#define FreeRW              (RWMode)0x3

#define PipeBufferSize  (size_t)(1024 * sizeof(char))

/* ���豸��Device������һ���������������������漰��IO�����Ķ���
* 1�������豸����Ļ�ȣ�
* 2���ļ�
* 3���׽���
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
#define InputProperty true      //��ģʽ
#define OutputProperty false    //дģʽ

//Pipeר���߳���
#define Pipe_LOCK(mtx)      mtx.lock();		//�߳�����
#define Pipe_UNLOCK(mtx)    mtx.unlock()	//�߳̽���
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
    //������aimд�����ݣ���Ҫ�ֶ�����(��һ��д��Ϣ����msg_queue���еȴ��������Զ�����)
    void write(DeviceId aim, const char* data);
    //������from�ж�ȡ���ݣ���Ҫ�ֶ�����(��һ������Ϣ����msg_queue���еȴ��������Զ�����)
    void read(DeviceId from, char* buff, size_t size = PipeBufferSize, size_t buffer_size = PipeBufferSize);
    /*��װ�豸����Ҫ�ֶ�����*/
    DeviceId installDevice(std::string file, RWMode mode, bool stamd_open = false);
    /*ж���豸����Ҫ�ֶ�����*/
    void uninstallDevice(DeviceId);
private:
    /*�Զ����е������̷������������Զ�����msg_queue�е���Ϣ*/
    void mainProgress(void);
    void uninstallDevice(std::vector<Device>::iterator);
    struct Device creatDevice(std::string file, RWMode mode, bool stand_open = false);
    void startupDevice(DeviceId);
    void startupDevice(Device&);
    void closeDevice(DeviceId);
    void closeDevice(Device&);
    /*���е�substep����������Ҫ���ⲿ�ֶ������������Ǳ��ṩ��mainProgress����ʹ�õ�*/
    bool __substep__Input(struct DataBox&, size_t);
    /*���е�substep����������Ҫ���ⲿ�ֶ������������Ǳ��ṩ��mainProgress����ʹ�õ�*/
    bool __substep__Output(struct DataBox&);
    /*���е�substep����������Ҫ���ⲿ�ֶ������������Ǳ��ṩ��mainProgress����ʹ�õ�*/
    bool __substep__Output__UserDefinedDevice(struct DataBox&, Device&);
    /*���е�substep����������Ҫ���ⲿ�ֶ������������Ǳ��ṩ��mainProgress����ʹ�õ�*/
    void __subfunc_Input_Sptcial(DataBox&);
    /*���е�substep����������Ҫ���ⲿ�ֶ������������Ǳ��ṩ��mainProgress����ʹ�õ�*/
    void __subfunc_Input_File(DataBox&);
    /*���е�substep����������Ҫ���ⲿ�ֶ������������Ǳ��ṩ��mainProgress����ʹ�õ�*/
    void __subfunc_Input_Socket(DataBox&);
    /*ע�⣺����__getDevice��__setDevice����ĵط���Ӧ��ʹ�������������������ǽ�Ϊ��get/set������Ƶģ����漰�����߳���*/
    struct Device* __optDevice(char, struct Device* = nullptr, DeviceId = DefaultDevice);
    /*��������ѱ���������Ϊ����δ�ﵽԤ�Ƶ�Ч��*/
    const struct Device& __getDevice(DeviceId) = delete;
    /*ע��!���˹��캯��������������run()��������һ�ж���device��д������Ӧ��ʹ���������,��Ϊ���������߳���*/
    void __setDevice(struct Device&);
    /*ע�⣺����__getBuffer��__setBuffer����ĵط���Ӧ��ʹ�������������������ǽ�Ϊ��get/set������Ƶģ����漰�����߳�����*/
    struct DataBox* __optBuffer(char, struct DataBox* = nullptr);
    /*��������ѱ���������Ϊ����δ�ﵽԤ�Ƶ�Ч��*/
    const struct DataBox& __getBuffer(void) = delete;
    /*ע��!���˹��캯��������������run()��������һ�ж���buffer��д������Ӧ��ʹ���������,��Ϊ���������߳���*/
    void __setBuffer(struct DataBox&);
};