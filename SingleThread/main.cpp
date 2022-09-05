#include <iostream>	
#include <process.h>
#include <Windows.h>

#pragma comment(lib, "Winmm.lib")

using namespace std;
int Money = 0;

//user ��忡�� �ϴ� ��

CRITICAL_SECTION CalculatorCS;

HANDLE MutexHandle;

unsigned WINAPI ThreadFunctionPlus(void* Parameter)
{
	WaitForSingleObject(MutexHandle, INFINITE); // kernel ��忡���� enter�� ����.
	
	for (int i = 0; i < 50; ++i)
	{
		
		//EnterCriticalSection(&CalculatorCS); //Lock�Ѵ�.
		++Money; // �Ӱ迵�� = ���ÿ� ���ϴ� �۾�.
		//cout << "Worker1 Thread: "<< Money << endl;
		//LeaveCriticalSection(&CalculatorCS);
	}
	ReleaseMutex(MutexHandle);
	return 0;
}

unsigned WINAPI ThreadFunctionMinus(void* Parameter)
{
	WaitForSingleObject(MutexHandle, INFINITE); // �ϵ������� �Դٰ���
	
	for (int i = 0; i < 50; ++i)
	{
		//EnterCriticalSection(&CalculatorCS);  //������忡���� �Ӵٰ���
		--Money; // �Ӱ迵��
		//cout << "Worker2 Thread: "<< Money << endl;
		//LeaveCriticalSection(&CalculatorCS);
	}
	ReleaseMutex(MutexHandle);
	return 0;
}

int main()
{
	HANDLE ThreadHandles[2];

	DWORD StartTime = timeGetTime();
	
	InitializeCriticalSection(&CalculatorCS); // ������ �ڹ��� ����

	MutexHandle = CreateMutexA(nullptr, TRUE, "�����̵�"); // L:���ϵ� �ڵ带 �����ڵ�� �ٲ���. // TRUE �ϸ� �������ڸ��� �� �ݾƹ����� ���̾ �Լ��� ������ ����.
	// ���� �̸����� ����� �� �������.

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		cout << "�̹� �������̴� ����" << endl;
		system("pause");
		exit(-1);
	}

	while (1);

	// c++ API ������� �� �� ������ �� �����Ѵ�.(beginthreadex)
	ThreadHandles[0] = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunctionPlus, nullptr, 0, nullptr); // createthread �� �ϸ� iostream�� �浹�� �Ͼ�� �ٸ� ������ ����Ѵ�.
	ThreadHandles[1] = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunctionMinus, nullptr, 0, nullptr);

	//TerminateThread -> ������ ������ ����
	//WaitForSingleObject(ThreadHandle1, INFINITE); // ���� �Լ� ������ ��ٷ���. ������ ������ �Լ��� ����
	//WaitForSingleObject(ThreadHandle2, INFINITE);

	WaitForMultipleObjects(2, ThreadHandles, true, INFINITE);

	cout << "Main Thread   " << Money << endl;

	//CloseHandle(ThreadHandles); // ������ ����� �ݾ��ش�.

	DeleteCriticalSection(&CalculatorCS); // �ڹ��� Ǯ����.

	CloseHandle(MutexHandle);

	DWORD EndTime = timeGetTime();
	cout << "Elapesed time: " << EndTime - StartTime << endl;
	
	return 0;
}