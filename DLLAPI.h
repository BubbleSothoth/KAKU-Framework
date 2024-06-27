/*此文件用于定义DLLAPI，在需要导出DLL代码的文件头部加入此文件即可使用DLLAPI宏*/
#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif