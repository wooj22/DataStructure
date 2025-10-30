#include <windows.h>
#include <process.h>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <queue>
using namespace std;


/*
	[위상 정렬, 멀티스레드를 활용한 Task 스케줄링]
	
	Critical Section, Event(수동), Interlocked를 사용해 
	큐 버퍼와 inDegree, tasksRemaining를 동기화하여
	3개의 스레드로 Task를 병렬처리합니다.
*/


/* ------------ Synchronization Tools---------------*/
CRITICAL_SECTION cs;	// Critical section
HANDLE taskEvent;		// Event(수동)



/* ------------------ Task Graph -------------------*/
struct TaskNode
{
	function<void()> task;    // 처리할 Task
	vector<int> next;         // 인접 리스트
	volatile LONG inDegree;	  // 진입 차수(의존 수)
};

struct TaskGraph
{
	vector<TaskNode> nodes;		    // Task Node
	queue<int> ready_queue;		    // 의존이 제거되어 실행이 가능한 Task
	volatile LONG  tasksRemaining;	// 스레드 종료를 위한 처리되어야할 남은 Task 개수

	// 노드 추가
	int AddTask(function<void()> fn)
	{
		nodes.push_back(TaskNode{ std::move(fn), {}, 0 });
		return (int)nodes.size() - 1;
	}

	// 간선(의존성) 추가
	void AddDependency(int u, int v)
	{
		nodes[u].next.push_back(v);	
		nodes[v].inDegree++;	

		// Cycle Cheak
		vector<int> visited(nodes.size(), 0);
		for (int i = 0; i < (int)nodes.size(); ++i)
		{
			if (visited[i] == 0)
			{
				if (HasCycleDFS(i, nodes, visited))
				{
					std::cout << "사이클이 존재합니다. 작업 그래프를 다시 설정하세요.\n";
					return;
				}
			}
		}
	}

	// Cycle Cheak (DFS)
	bool HasCycleDFS(int node, vector<TaskNode>& nodes, vector<int>& visited)
	{
		visited[node] = 1; // 현재 경로에 있음

		for (int next : nodes[node].next)
		{
			if (visited[next] == 1)
				return true; // 현재 경로 안에서 다시 방문됨 → 사이클
			if (visited[next] == 0 && HasCycleDFS(next, nodes, visited))
				return true;
		}

		visited[node] = 2; // 탐색 완료
		return false;
	}

	// 단일 스레드 스케줄링
	void SigleThreadScheduling()
	{
		for (int i = 0; i < nodes.size(); ++i)
			if (nodes[i].inDegree == 0) ready_queue.push(i);

		while (!ready_queue.empty())
		{
			int nodeIndex = ready_queue.front();  ready_queue.pop();
			nodes[nodeIndex].task();

			for (int next : nodes[nodeIndex].next)
			{
				nodes[next].inDegree--;
				if (nodes[next].inDegree == 0)
					ready_queue.push(next);
			}
		}
	}

	// 멀티 스레드 스케줄링을 위한 초기화
	void MultiThreadSchedulingInit()
	{
		tasksRemaining = nodes.size();

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

// Thread가 실행할 스레드 함수
// 큐 버퍼에서 Task를 꺼내와 처리
static unsigned __stdcall TaskProcessing(void* param)
{
	ThreadData* threadData = static_cast<ThreadData*>(param);
	TaskGraph* taskGraph = threadData->taskGraph;
	std::cout << threadData->threadId << "번 스레드 시작.\n";

	while (true)
	{
		// task가 모두 처리되어있다면 다른 스레드를 깨우고 종료
		if (InterlockedCompareExchange(&taskGraph->tasksRemaining, 0, 0) == 0)
		{
			SetEvent(taskEvent);
			break;
		}

		// 데이터 생산 이벤트가 발생할때까지 대기
		WaitForSingleObject(taskEvent, INFINITE);

		// 깨어났을때 task가 모두 처리되어있다면 다른 스레드를 깨우고 종료
		if (InterlockedCompareExchange(&taskGraph->tasksRemaining, 0, 0) == 0)
		{
			SetEvent(taskEvent);
			break;
		}

		// [소비] 큐에서 task 추출
		EnterCriticalSection(&cs);
		if (taskGraph->ready_queue.empty()) {
			ResetEvent(taskEvent);
			LeaveCriticalSection(&cs);
			continue;
		}
		int nodeIndex = taskGraph->ready_queue.front();
		taskGraph->ready_queue.pop();
		LeaveCriticalSection(&cs);

		// Task 실행
		taskGraph->nodes[nodeIndex].task();
		InterlockedDecrement(&taskGraph->tasksRemaining);

		// 후속 노드 처리
		for (int nextIndex : taskGraph->nodes[nodeIndex].next) {
			// 진입 차수 원자적 감소
			LONG newInDegree = InterlockedDecrement(&taskGraph->nodes[nextIndex].inDegree);

			// [생산] 의존성이 없는 task 큐에 추가
			if (newInDegree == 0)
			{
				EnterCriticalSection(&cs);
				taskGraph->ready_queue.push(nextIndex);
				SetEvent(taskEvent);
				LeaveCriticalSection(&cs);
			}
		}
	}

	std::cout << threadData->threadId << "번 스레드 종료.\n";
	return 0;
}



/* -------------------------------------------------------------------------------*/
void SingleRun(TaskGraph* taskGraph)
{
	cout << "5개 작업을 하나의 스레드로 위상 정렬하여 처리합니다." << endl;
	taskGraph->SigleThreadScheduling();
}

int MultiRun(TaskGraph* taskGraph)
{
	cout << "5개 작업을 3개의 스레드로 위상 정렬하여 처리합니다." << endl;

	// critical section
	InitializeCriticalSection(&cs);

	// event(수동)
	taskEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!taskEvent) {
		cout << "이벤트 생성 실패";
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
			TaskProcessing,			// 스레드 함수
			&threadDatas[i],		// 스레드 매개변수
			0,
			NULL
		));

		if (hThreads[i] == NULL) {
			std::cout << "스레드 " << i << " 생성 실패" << endl;
			return -1;
		}
	}

	// 초기 큐 init
	taskGraph->MultiThreadSchedulingInit();

	// 모든 스레드가 종료될때까지 대기
	WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
	std::cout << "모든 Task 처리 완료." << endl;

	// 스레드, 동기화 도구 정리
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
	   가능한 위상정렬 순서
		t2 → t1 → t4 → t3 → t5
		t2 → t4 → t3 → t1 → t5
		t2 → t4 → t1 → t3 → t5
		t2 → t1 → t4 → t3 → t5
	*/
	taskGraph.AddDependency(t2, t1); 
	taskGraph.AddDependency(t4, t3); 
	taskGraph.AddDependency(t1, t5); 
	taskGraph.AddDependency(t3, t5); 
	taskGraph.AddDependency(t2, t4); 


	// 1. 단일 스레드 작업 스케줄링
	//SingleRun(&taskGraph);

	// 2. 멀티 스레드 작업 스케줄링
	if (MultiRun(&taskGraph) == -1) return -1;;

	return 0;
}