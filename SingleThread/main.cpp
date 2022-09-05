#include <iostream>	
#include <process.h>
#include <Windows.h>

#pragma comment(lib, "Winmm.lib")

using namespace std;
int Money = 0;

//user 모드에서 하는 것

CRITICAL_SECTION CalculatorCS;

HANDLE MutexHandle;

unsigned WINAPI ThreadFunctionPlus(void* Parameter)
{
	WaitForSingleObject(MutexHandle, INFINITE); // kernel 모드에서의 enter와 같다.
	
	for (int i = 0; i < 50; ++i)
	{
		
		//EnterCriticalSection(&CalculatorCS); //Lock한다.
		++Money; // 임계영역 = 동시에 못하는 작업.
		//cout << "Worker1 Thread: "<< Money << endl;
		//LeaveCriticalSection(&CalculatorCS);
	}
	ReleaseMutex(MutexHandle);
	return 0;
}

unsigned WINAPI ThreadFunctionMinus(void* Parameter)
{
	WaitForSingleObject(MutexHandle, INFINITE); // 하드웨어까지 왔다갔다
	
	for (int i = 0; i < 50; ++i)
	{
		//EnterCriticalSection(&CalculatorCS);  //유저모드에서만 왓다갔다
		--Money; // 임계영역
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
	
	InitializeCriticalSection(&CalculatorCS); // 가상의 자물쇠 생성

	MutexHandle = CreateMutexA(nullptr, TRUE, "남욱이돈"); // L:와일드 코드를 유니코드로 바꿔줌. // TRUE 하면 시작하자마자 문 닫아버리는 것이어서 함수가 동작을 안함.
	// 같은 이름으로 만들면 안 만들어짐.

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		cout << "이미 실행중이다 새꺄" << endl;
		system("pause");
		exit(-1);
	}

	while (1);

	// c++ API 사용했을 때 안 깨지고 잘 동작한다.(beginthreadex)
	ThreadHandles[0] = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunctionPlus, nullptr, 0, nullptr); // createthread 로 하면 iostream과 충돌이 일어나서 다른 것으로 써야한다.
	ThreadHandles[1] = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunctionMinus, nullptr, 0, nullptr);

	//TerminateThread -> 쓰레드 강제로 지움
	//WaitForSingleObject(ThreadHandle1, INFINITE); // 위에 함수 동작을 기다려라. 메인을 돌리고 함수를 동작
	//WaitForSingleObject(ThreadHandle2, INFINITE);

	WaitForMultipleObjects(2, ThreadHandles, true, INFINITE);

	cout << "Main Thread   " << Money << endl;

	//CloseHandle(ThreadHandles); // 쓰레드 만들면 닫아준다.

	DeleteCriticalSection(&CalculatorCS); // 자물쇠 풀어줌.

	CloseHandle(MutexHandle);

	DWORD EndTime = timeGetTime();
	cout << "Elapesed time: " << EndTime - StartTime << endl;
	
	return 0;
}