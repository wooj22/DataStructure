#include <iostream>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

/* 정점 구조체 */
struct Vertex {
    int data;                                    // 정점
    vector<pair<int, int>> adjacencyList;        // 인접 리스트 {목적 정점 index, 가중치}
};

struct Graph {
    vector<Vertex> vertexs;

    explicit Graph(int n) : vertexs(n) {}

    // 정점 설정
    void SetNode(int i, int d) 
    {
        vertexs[i].data = d;
    }

    // 간선 추가
    void AddDirectedEdge(int from, int to, int weight) 
    {
        vertexs[from].adjacencyList.push_back({ to, weight });
    }

    // 그래프 출력
    void PrintGraphNodeAndEdge() const 
    {
        cout << "그래프를 출력합니다." << endl;

        for (int i = 1; i < vertexs.size(); ++i) {
            cout << vertexs[i].data << " :";
            for (auto& e : vertexs[i].adjacencyList)
                cout << "(" << e.first << "," << e.second << ") ";
            cout << endl;
        }
    }

	// [다익스트라(dijkstra) 최단 경로 알고리즘]
    /*  최단 경로 배열 dist[], 경로 추적용 이전 정점 배열 prev[], 최소힙 우선순위 큐 pq
      
        1. 최단 거리 배열 dist[]를 무한대로 초기화하고 시작 정점의 거리를 0으로 지정하여 우선순위 큐에 시작 정점을 추가한다.
        2. 우선순위 큐에서 맨 앞 노드(가장 가까운 정점)을 꺼낸다. 이 정점의 비용이 최단 거리 배열에 저장된 값 보다 크다면 건너뛴다.
        3. 우선순위 큐에 꺼낸 정점의 인접 정점들을 탐색한다. ‘현재 거리 + 인접 정점까지의 거리’가 최단 거리 배열에 저장되어있는 값보다 작다면 최단 경로 갱신으로 배열에 ‘현재 거리 + 인접 정점까지의 거리’를 기록하고 갱신된 인접 정점을 우선순위 큐에 추가한다.
        4. 우선순위 큐가 빌때까지 2~3번 과정을 반복한다. 알고리즘이 종료되면 최단 거리 배열 dist[]에는 시작 정점으로부터 각 정점까지의 최단 거리가 저장되어있다.
    */
    void Dijkstra(int start) {
		const int INF = 1e9;

        // 1. start init
        vector<int> dist(vertexs.size(), INF);      // 최단 거리 배열
		vector<int> prev(vertexs.size(), -1);       // 이전 정점 배열 (경로 추적용)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // 최소힙 기반 우선순위큐 (거리, 정점 번호)

        dist[start] = 0;
        pq.push({ 0, start});

        // 2~3. 최단 경로 갱신
        while (!pq.empty()) {
            // 현재 시점에서 가장 가까운 정점 cur V
            auto curV = pq.top();  pq.pop();
			int vertexIndex = curV.second;        // 정점 번호
			int curDist = curV.first;             // 정점까지의 누적 거리

			// (2) 꺼낸 정점이 이미 최단 거리 갱신된 정점이라면 건너뜀
            if (curDist > dist[vertexIndex]) continue;

			// (3) 인접 정점 탐색
            for (auto& edge : vertexs[vertexIndex].adjacencyList) {
                int nextVIndex = edge.first;
                int weight = edge.second;

                // 최단 거리 갱신
				// '현재정점까지의 거리 + 인접노드까지의 거리' < '최단 거리 배열에 저장된 값' 이면 갱신
                if (dist[vertexIndex] + weight < dist[nextVIndex]) 
                {
                    dist[nextVIndex] = dist[vertexIndex] + weight;    // 최단 거리 갱신
					prev[nextVIndex] = vertexIndex;   		          // 이전 정점 기록
                    pq.push({ dist[nextVIndex], nextVIndex});         // 우선순위큐에 추가
                }
            }
        }

        // 최단 거리
        cout << "\n=== 최단 거리 출력 ===" << endl;
        for (int i = 1; i < vertexs.size(); ++i)
        {
            if (dist[i] == INF) cout << vertexs[i].data << ": INF\n";
            else cout << vertexs[i].data << ": " << dist[i] << "\n";
        }

        // 최단 경로
        cout << "\n=== 최단 경로 출력 ===" << endl;
        for (int goal = 1; goal < vertexs.size(); ++goal)
        {
            cout << vertexs[goal].data << " : ";
            if (dist[goal] == INF) 
            {
                cout << "경로 없음\n";
                continue;
            }

            // 이전 정점 배열 역추적
            stack<int> path;
            for (int v = goal; v != -1; v = prev[v])
                path.push(v);

            while (!path.empty()) 
            {
                cout << vertexs[path.top()].data << "(" << dist[path.top()] << ") -> ";
                path.pop();
            }

            cout << endl;
        }
    }
};

int main() {
    Graph graph(6); // 0~5

    // 정점
    graph.SetNode(1, 1);
    graph.SetNode(2, 2);
    graph.SetNode(3, 3);
    graph.SetNode(4, 4);
    graph.SetNode(5, 5);

    // 간선
    graph.AddDirectedEdge(1, 2, 8);
    graph.AddDirectedEdge(1, 3, 3);
    graph.AddDirectedEdge(2, 4, 4);
    graph.AddDirectedEdge(2, 5, 15);
    graph.AddDirectedEdge(3, 4, 13);
    graph.AddDirectedEdge(4, 5, 2);
    graph.PrintGraphNodeAndEdge();

    // 최단 경로 알고리즘
    graph.Dijkstra(1);

    return 0;
}
