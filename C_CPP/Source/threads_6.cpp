#include "stuff.h"
#include "externals.h"


void Thread_Prepare()
{
	int		i;
	
	for (i = 0; i < MAX_THREADS; i++)
	{
		threads[i].tHandle = NULL;
		threads[i].type = NONE;
	}
}


int Thread_Add(thread_type type)
{
	int		i;

	for (i = 0; i < MAX_THREADS; i++)
		if (threads[i].tHandle == NULL)
			break;

	if (i == MAX_THREADS)
		i = -1;
	else
		threads[i].type = type;

#ifdef DEBUG
	printf("Adding thread number: %d\n", i);
#endif

	return i;
}


int Thread_Check(thread_type type)
{
	int		i, 
			k = 0;

	for (i = 0; i < MAX_THREADS; i++)
		if (threads[i].type == type)
			k++;

	return k;
}


void Thread_Clear(int num)
{
	threads[num].tHandle = NULL;
	threads[num].type = NONE;
}


int Thread_Kill(thread_type type)
{
	int		i,
			k = 0;

	for (i = 0; i < MAX_THREADS; i++)
	{
		if (threads[i].type == type)
		{
			fTerminateThread(threads[i].tHandle, 0);
			Thread_Clear(i);
			k++;
		}
	}

	return k;
}


HANDLE Thread_Start(LPTHREAD_START_ROUTINE function, LPVOID param)
{
	DWORD		id = 0;
	HANDLE		tHandle;

	tHandle = fCreateThread(NULL, 0, function, (LPVOID)param, 0, &id);

	Sleep(THREAD_WAIT_TIME);

	return tHandle;
}