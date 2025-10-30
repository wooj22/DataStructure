#include <windows.h>
#include <process.h>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <queue>
using namespace std;


/*
	[���� ����, ���� ó���� Ȱ���� Task �����ٸ�]

	TaskGrap(DAG)
	- ��������(����)�� �߰��Ҷ����� ����Ŭ�� �߻����� �ʴ��� �˻��Ͽ�
	  DAG ������ TaskGrap�� �����մϴ�.

	���� ����
	- ĭ �˰������� ���������� 0�� Task�� ���ʴ�� ó���Ͽ�
	  �������� ��ġ�� �����鼭 ������� �۾��� ó���մϴ�.
	
	���� ó��
	- Critical Section, Manual-reset Event, Interlocked, volatile�� �̿��Ͽ�
      ������ �� ����ȭ�� �����մϴ�.
    - ���� �����尡 ���ÿ� ť���� Task�� ���� ���ķ� ó���մϴ�.
	  
	  1) Critical Section
         - ť ���ۿ� Task�� �߰��ϰų� ������ �� ���� �����常 �����ϵ��� ��ȣ�մϴ�.

      2) Event (Manual-reset, ���� ����)
         - ť�� ������� ��� ��� �����尡 WaitForSingleObject()�� ����ϵ��� �մϴ�.
         - ���ο� Task�� ť�� �߰��Ǹ� SetEvent()�� ȣ���� �����带 ����ϴ�.
         - ť�� �ٽ� ��� �Ǹ� ResetEvent()�� ȣ���� ��������� �ٽ� ��� ���·� ����ϴ�.

      3) Interlocked
         - inDegree ���ҳ� ���� Task ��(tasksRemaining) ���� ���� ������
           ���������� �����Ͽ� ���� ����(Race Condition)�� �����մϴ�.

      4) volatile
         - �����Ϸ��� ����ȭ�� ���� �� ĳ���� �����Ͽ�,
           �ٸ� �����尡 ������ ������ �׻� �޸𸮿��� �е��� �����մϴ�.
*/


/* ------------ Synchronization Tools---------------*/
CRITICAL_SECTION cs;	// Critical section
HANDLE taskEvent;		// Event(����)



/* ------------------ Task Graph -------------------*/
struct TaskNode
{
	function<void()> task;    // ó���� Task
	vector<int> next;         // ���� ����Ʈ
	volatile LONG inDegree;	  // ���� ����(���� ��)
};

struct TaskGraph
{
	vector<TaskNode> nodes;		    // Task Node
	queue<int> ready_queue;		    // ������ ���ŵǾ� ������ ������ Task
	volatile LONG  tasksRemaining;	// ������ ���Ḧ ���� ó���Ǿ���� ���� Task ����

	// ��� �߰�
	int AddTask(function<void()> fn)
	{
		nodes.push_back(TaskNode{ std::move(fn), {}, 0 });
		return (int)nodes.size() - 1;
	}

	// ����(������) �߰�
	void AddDependency(int u, int v)
	{
		nodes[u].next.push_back(v);	
		nodes[v].inDegree++;	

		// ����Ŭ �߻� �˻�
		vector<int> visited(nodes.size(), 0);
		for (int i = 0; i < (int)nodes.size(); ++i)
		{
			if (visited[i] == 0)
			{
				if (HasCycleDFS(i, nodes, visited))
				{
					std::cout << "����Ŭ�� �����մϴ�. �۾� �׷����� �ٽ� �����ϼ���.\n";
					return;
				}
			}
		}
	}

	// Cycle Cheak (DFS)
	bool HasCycleDFS(int node, vector<TaskNode>& nodes, vector<int>& visited)
	{
		visited[node] = 1;    // ���� ��ο� ����

		for (int next : nodes[node].next)
		{
			// 1) node�� �湮���� ����� ����Ŭ �߻�
			if (visited[next] == 1)
				return true;

			// 2) node�� ���� �̹湮 ����� �ļ� ��� Ž�� ����
			if (visited[next] == 0 && HasCycleDFS(next, nodes, visited))
				return true;
		}

		visited[node] = 2; // Ž�� �Ϸ�
		return false;
	}

	// ���� ������ �����ٸ�
	void SigleThreadScheduling()
	{
		// �ʱ� task �߰�
		for (int i = 0; i < nodes.size(); ++i)
			if (nodes[i].inDegree == 0) ready_queue.push(i);

		while (!ready_queue.empty())
		{
			// ť���� task ����, ����
			int nodeIndex = ready_queue.front();  ready_queue.pop();
			nodes[nodeIndex].task();

			// �ļӳ�� �������� ����
			for (int next : nodes[nodeIndex].next)
			{
				nodes[next].inDegree--;

				// ���� ���������� 0�̶�� ť�� �߰�
				if (nodes[next].inDegree == 0)
					ready_queue.push(next);
			}
		}
	}

	// ��Ƽ ������ �����ٸ��� ���� �ʱ�ȭ
	void MultiThreadSchedulingInit()
	{
		// ó���ؾ��� task ����
		tasksRemaining = nodes.size();

		// �ʱ� task �߰�
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].inDegree == 0) ready_queue.push(i);
			SetEvent(taskEvent);
		}
	}
};



/* ------------ Thread ---------------*/
// Thread data
struct ThreadData
{
	int threadId;
	TaskGraph* taskGraph;
};

// Thread�� ������ ������ �Լ�
// ť ���ۿ��� Task�� ������ ó��
static unsigned __stdcall TaskProcessing(void* param)
{
	ThreadData* threadData = static_cast<ThreadData*>(param);
	TaskGraph* taskGraph = threadData->taskGraph;
	std::cout << threadData->threadId << "�� ������ ����.\n";

	while (true)
	{
		// ���� task ������ 0������ ������ ������ ���� Ȯ��
		// task�� ��� ó���Ǿ��ִٸ� �ٸ� �����带 ����� ����
		if (InterlockedCompareExchange(&taskGraph->tasksRemaining, 0, 0) == 0)
		{
			SetEvent(taskEvent);
			break;
		}

		// ť ������ ���� �̺�Ʈ�� �߻��Ҷ����� ���
		WaitForSingleObject(taskEvent, INFINITE);

		// ���� task ������ 0������ ������ ������ ���� Ȯ��
		// ������� task�� ��� ó���Ǿ��ִٸ� �ٸ� �����带 ����� ����
		if (InterlockedCompareExchange(&taskGraph->tasksRemaining, 0, 0) == 0)
		{
			SetEvent(taskEvent);
			break;
		}

		// [�Һ�] task ������ ���� ť�� ���� - �Ӱ豸��
		// ���� ť�� ����ִٸ� �̺�Ʈ�� ��Ȱ��ȭ�ϰ� continue
		// �����Ͱ� �ִٸ� ť���� task ����
		EnterCriticalSection(&cs);
		if (taskGraph->ready_queue.empty()) {
			ResetEvent(taskEvent);
			LeaveCriticalSection(&cs);
			continue;
		}
		int nodeIndex = taskGraph->ready_queue.front();
		taskGraph->ready_queue.pop();
		LeaveCriticalSection(&cs);

		// Task ����
		// Interlocked �Լ��� ���� ������ �������� ���� task ���� ����
		taskGraph->nodes[nodeIndex].task();
		InterlockedDecrement(&taskGraph->tasksRemaining);

		// �ļ� ��� ó��
		// Interlocked �Լ��� ���� ������ �������� �ļ� ����� ���� ���� ����
		for (int nextIndex : taskGraph->nodes[nodeIndex].next) {
			LONG newInDegree = InterlockedDecrement(&taskGraph->nodes[nextIndex].inDegree);

			// [����] task �߰��� ���� ť�� ���� - �Ӱ豸��
			// �������� ���� task ť�� �߰�
			if (newInDegree == 0)
			{
				EnterCriticalSection(&cs);
				taskGraph->ready_queue.push(nextIndex);
				SetEvent(taskEvent);
				LeaveCriticalSection(&cs);
			}
		}
	}

	std::cout << threadData->threadId << "�� ������ ����.\n";
	return 0;
}



/* -------------------------------------------------------------------------------*/
void SingleRun(TaskGraph* taskGraph)
{
	cout << "5�� �۾��� �ϳ��� ������� ���� �����Ͽ� ó���մϴ�." << endl;
	taskGraph->SigleThreadScheduling();
}

int MultiRun(TaskGraph* taskGraph)
{
	cout << "5�� �۾��� 3���� ������� ���� �����Ͽ� ó���մϴ�." << endl;

	// critical section
	InitializeCriticalSection(&cs);

	// event(����)
	taskEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!taskEvent) {
		cout << "�̺�Ʈ ���� ����";
		return -1;
	}

	// worker thread create
	ThreadData threadDatas[3] = {
		{1, taskGraph}, {2, taskGraph}, {3, taskGraph}
	};

	HANDLE hThreads[3];
	for (int i = 0; i < 3; i++)
	{
		hThreads[i] = reinterpret_cast<HANDLE>(_beginthreadex(
			NULL,
			0,
			TaskProcessing,			// ������ �Լ�
			&threadDatas[i],		// ������ �Ű�����
			0,
			NULL
		));

		if (hThreads[i] == NULL) {
			std::cout << "������ " << i << " ���� ����" << endl;
			return -1;
		}
	}

	// �ʱ� ť init
	taskGraph->MultiThreadSchedulingInit();

	// ��� �����尡 ����ɶ����� ���
	WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
	std::cout << "��� Task ó�� �Ϸ�." << endl;

	// ������, ����ȭ ���� ����
	for (int i = 0; i < 3; i++) CloseHandle(hThreads[i]);
	DeleteCriticalSection(&cs);
	CloseHandle(taskEvent);
}


int main()
{
	// task graph
	TaskGraph taskGraph;

	// add tase(node)
	int t1 = taskGraph.AddTask([]() { Sleep(1000); std::cout << "Task1 End\n"; });
	int t2 = taskGraph.AddTask([]() { Sleep(1000); std::cout << "Task2 End\n"; });
	int t3 = taskGraph.AddTask([]() { Sleep(1000); std::cout << "Task3 End\n"; });
	int t4 = taskGraph.AddTask([]() { Sleep(1000); std::cout << "Task4 End\n"; });
	int t5 = taskGraph.AddTask([]() { Sleep(1000); std::cout << "Task5 End\n"; });

	/* add depends(egde)
	   ������ �������� ����
		t2 �� t1 �� t4 �� t3 �� t5
		t2 �� t4 �� t3 �� t1 �� t5
		t2 �� t4 �� t1 �� t3 �� t5
		t2 �� t1 �� t4 �� t3 �� t5
	*/
	taskGraph.AddDependency(t2, t1); 
	taskGraph.AddDependency(t4, t3); 
	taskGraph.AddDependency(t1, t5); 
	taskGraph.AddDependency(t3, t5); 
	taskGraph.AddDependency(t2, t4); 


	// 1. ���� ������ �۾� �����ٸ�
	//SingleRun(&taskGraph);

	// 2. ��Ƽ ������ �۾� �����ٸ�
	if (MultiRun(&taskGraph) == -1) return -1;;

	return 0;
}