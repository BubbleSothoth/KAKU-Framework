// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "..\Kernal_Runtime\System.h"

#pragma comment(lib,"../x64/Release/Kernal_Runtime.lib")

class Test :
    public RuntimeSystem {
public:
    int sum = 0;
    Test() {
        StartUp(Test);
        //RuntimeSystem::MainProgress<Test>(this);
    }
    void Init(){ }
    void Run(void) {
        //std::cout << "Hello World!\n";
        //RuntimeSystem::setNormalError(FILE_NOT_OPEN_ERROR);
        printf_s("Hello world, loop %d.\r", ++sum);
        std::string data = this->GetNextCommand().Data[0].c_str();
        if (data=="exit")
            this->setRST(ExitFlag);
        if (data != "###NULL###") {
            printf_s("%s\n", data.c_str());
            //system("pause");
        }
        //sprintf_s(data, "Hello world, loop %d.\n", sum);
        //this->RecordInfo(data);
        //system("pause");
    }
    void Exit() { 
        printf_s("Test Moudle: Bye.\n");
        this->RecordCust("Exit");
    }
};

int main()
{
    Test* test=new Test();
    delete test;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
