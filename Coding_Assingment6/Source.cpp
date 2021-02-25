#include <windows.h>
#include <stdio.h>
#include <string>
#include<iostream>
using namespace std;

HANDLE ghWriteEvent;
HANDLE ghThreads;
string buff="";

//Thread Procedure invoked when reading thread is created
DWORD WINAPI ThreadProc(LPVOID lpParam)
{

    UNREFERENCED_PARAMETER(lpParam);

    DWORD dwWaitResult=0;
    int result = 1;
    printf("Thread %d waiting for write event...\n", GetCurrentThreadId());

    dwWaitResult = WaitForSingleObject(
        ghWriteEvent, // event handle
        INFINITE);    // indefinite wait

    switch (dwWaitResult)
    {
    case WAIT_OBJECT_0:

        printf("Thread %d reading from buffer\n",
            GetCurrentThreadId());
        cout<<"Content of buffer read:"<<buff<<"\n";
        break;


    default:
        printf("Wait error (%d)\n", GetLastError());
        result= 0;
    }

    printf("Reading Thread %d exiting\n", GetCurrentThreadId());
    return result;
}

//Function to create writing event and reading thread
void CreateEventsAndThreads(void)
{
    
    DWORD dwThreadID;

    ghWriteEvent = CreateEvent(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("WriteEvent")  // object name
    );

    if (ghWriteEvent == NULL)
    {
        printf("CreateEvent failed (%d)\n", GetLastError());
        return;
    }

    ghThreads = CreateThread(
            NULL,              // default security
            0,                 // default stack size
            ThreadProc,        // name of the thread function
            NULL,              // no thread parameters
            0,                 // default startup flags
            &dwThreadID);

    if (ghThreads == NULL)
    {
            printf("CreateThread failed (%d)\n", GetLastError());
            return;
    }
}

//Function to write into the buffer
void WriteToBuffer(VOID)
{
   
    printf("Main thread writing to the shared buffer...\n");
    buff = "Hello All!!!";
    if (!SetEvent(ghWriteEvent))
    {
        printf("SetEvent failed (%d)\n", GetLastError());
        return;
    }
}

//Resource cleanup
void CloseEvents()
{
    CloseHandle(ghWriteEvent);
}

//Main function
int main(void)
{
    DWORD dwWaitResult;
    DWORD result = 0;

    CreateEventsAndThreads();

    WriteToBuffer();

    printf("Main thread waiting for reading thread to exit...\n");

    dwWaitResult = WaitForSingleObject(ghThreads, INFINITE);

    switch (dwWaitResult)
    {
    case WAIT_OBJECT_0:
        printf("\nAll threads ended, cleaning up for application exit...\n");
        break;

    default:
        printf("WaitForSingleObject failed (%d)\n", GetLastError());
        result = 1;
    }
    // Have single exit with resource cleanup
    CloseEvents();

    return result;
}

