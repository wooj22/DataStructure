#include <windows.h>
#include <process.h>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <queue>
using namespace std;


/*
	[위상 정렬, 병렬 처리를 활용한 Task 스케줄링]

	TaskGrap(DAG)
	- 의존관계(간선)을 추가할때마다 사이클이 발생하지 않는지 검사하여
	  DAG 구조의 TaskGrap를 생성합니다.

	위상 정렬
	- 칸 알고리즘으로 진입차수가 0인 Task를 차례대로 처리하여
	  의존성을 해치지 않으면서 순서대로 작업을 처리합니다.
	
	병렬 처리
	- Critical Section, Manual-reset Event, Interlocked, volatile을 이용하여
      스레드 간 동기화를 수행합니다.
    - 여러 스레드가 동시에 큐에서 Task를 꺼내 병렬로 처리합니다.
	  
	  1) Critical Section
         - 큐 버퍼에 Task를 추가하거나 제거할 때 단일 스레드만 접근하도록 보호합니다.

      2) Event (Manual-reset, 수동 리셋)
         - 큐가 비어있을 경우 모든 스레드가 WaitForSingleObject()로 대기하도록 합니다.
         - 새로운 Task가 큐에 추가되면 SetEvent()를 호출해 스레드를 깨웁니다.
         - 큐가 다시 비게 되면 ResetEvent()를 호출해 스레드들을 다시 대기 상태로 만듭니다.

      3) Interlocked
         - inDegree 감소나 남은 Task 수(tasksRemaining) 감소 등의 연산을
           원자적으로 수행하여 경쟁 상태(Race Condition)를 방지합니다.

      4) volatile
         - 컴파일러의 최적화로 인한 값 캐싱을 방지하여,
           다른 스레드가 갱신한 변수를 항상 메모리에서 읽도록 보장합니다.
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

		// 사이클 발생 검사
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
		visited[node] = 1;    // 현재 경로에 있음

		for (int next : nodes[node].next)
		{
			// 1) node가 방문중인 노드라면 사이클 발생
			if (visited[next] == 1)
				return true;

			// 2) node가 아직 미방문 노드라면 후속 노드 탐색 시작
			if (visited[next] == 0 && HasCycleDFS(next, nodes, visited))
				return true;
		}

		visited[node] = 2; // 탐색 완료
		return false;
	}

	// 단일 스레드 스케줄링
	void SigleThreadScheduling()
	{
		// 초기 task 추가
		for (int i = 0; i < nodes.size(); ++i)
			if (nodes[i].inDegree == 0) ready_queue.push(i);

		while (!ready_queue.empty())
		{
			// 큐에서 task 추출, 실행
			int nodeIndex = ready_queue.front();  ready_queue.pop();
			nodes[nodeIndex].task();

			// 후속노드 진입차수 감소
			for (int next : nodes[nodeIndex].next)
			{
				nodes[next].inDegree--;

				// 만약 진입차수가 0이라면 큐에 추가
				if (nodes[next].inDegree == 0)
					ready_queue.push(next);
			}
		}
	}

	// 멀티 스레드 스케줄링을 위한 초기화
	void MultiThreadSchedulingInit()
	{
		// 처리해야할 task 개수
		tasksRemaining = nodes.size();

		// 초기 task 추가
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
		// 남은 task 개수가 0개인지 원자적 연산을 통해 확인
		// task가 모두 처리되어있다면 다른 스레드를 깨우고 종료
		if (InterlockedCompareExchange(&taskGraph->tasksRemaining, 0, 0) == 0)
		{
			SetEvent(taskEvent);
			break;
		}

		// 큐 데이터 생산 이벤트가 발생할때까지 대기
		WaitForSingleObject(taskEvent, INFINITE);

		// 남은 task 개수가 0개인지 원자적 연산을 통해 확인
		// 깨어났을때 task가 모두 처리되어있다면 다른 스레드를 깨우고 종료
		if (InterlockedCompareExchange(&taskGraph->tasksRemaining, 0, 0) == 0)
		{
			SetEvent(taskEvent);
			break;
		}

		// [소비] task 추출을 위해 큐에 접근 - 임계구역
		// 만약 큐가 비어있다면 이벤트를 비활성화하고 continue
		// 데이터가 있다면 큐에서 task 추출
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
		// Interlocked 함수를 통해 원자적 연산으로 남은 task 개수 감소
		taskGraph->nodes[nodeIndex].task();
		InterlockedDecrement(&taskGraph->tasksRemaining);

		// 후속 노드 처리
		// Interlocked 함수를 통해 원자적 연산으로 후속 노드의 진입 차수 감소
		for (int nextIndex : taskGraph->nodes[nodeIndex].next) {
			LONG newInDegree = InterlockedDecrement(&taskGraph->nodes[nextIndex].inDegree);

			// [생산] task 추가를 위해 큐에 접근 - 임계구역
			// 의존성이 없는 task 큐에 추가
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