/*���ļ����ڶ���DLLAPI������Ҫ����DLL������ļ�ͷ��������ļ�����ʹ��DLLAPI��*/
#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif