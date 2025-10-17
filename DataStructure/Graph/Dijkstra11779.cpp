/*
* [백준 11779번 : 최소 비용 구하기2]
* n(1≤n≤1,000)개의 도시가 있다. 그리고 한 도시에서 출발하여 다른 도시에 도착하는 m(1≤m≤100,000)개의 버스가 있다.
  우리는 A번째 도시에서 B번째 도시까지 가는데 드는 버스 비용을 최소화 시키려고 한다.
  그러면 A번째 도시에서 B번째 도시 까지 가는데 드는 최소비용과 경로를 출력하여라.
  항상 시작점에서 도착점으로의 경로가 존재한다.

  첫째 줄에 도시의 개수 n(1≤n≤1,000)이 주어지고 둘째 줄에는 버스의 개수 m(1≤m≤100,000)이 주어진다.
  그리고 셋째 줄부터 m+2줄까지 다음과 같은 버스의 정보가 주어진다.
  먼저 처음에는 그 버스의 출발 도시의 번호가 주어진다.
  그리고 그 다음에는 도착지의 도시 번호가 주어지고 또 그 버스 비용이 주어진다.
  버스 비용은 0보다 크거나 같고, 100,000보다 작은 정수이다.
  그리고 m+3째 줄에는 우리가 구하고자 하는 구간 출발점의 도시번호와 도착점의 도시번호가 주어진다.

  첫째 줄에 출발 도시에서 도착 도시까지 가는데 드는 최소 비용을 출력한다.
  둘째 줄에는 그러한 최소 비용을 갖는 경로에 포함되어있는 도시의 개수를 출력한다. 출발 도시와 도착 도시도 포함한다.
  셋째 줄에는 최소 비용을 갖는 경로를 방문하는 도시 순서대로 출력한다. 경로가 여러가지인 경우 아무거나 하나 출력한다.
*/


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

    explicit Graph(int n) : vertexs(n + 1)
    {
        // 0번 인덱스 사용 x
        for (int i = 1; i < vertexs.size(); ++i)
            vertexs[i].data = i;
    }

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

    // 최단 경로 찾기
    void Dijkstra(int start, int goal) {
        const int INF = 1e9;

        // start init
        vector<int> dist(vertexs.size(), INF);      // 최단 거리 배열
        vector<int> prev(vertexs.size(), -1);       // 이전 정점 배열 (경로 추적용)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // 최소힙 기반 우선순위큐 (거리, 정점 번호)

        dist[start] = 0;
        pq.push({ 0, start });

        // 최단 경로 갱신
        while (!pq.empty()) {
            // 현재 시점에서 가장 가까운 정점 cur V
            auto curV = pq.top();  pq.pop();
            int vertexIndex = curV.second;
            int curDist = curV.first;

            // 꺼낸 정점이 이미 최단 거리 갱신된 정점이라면 건너뜀
            if (curDist > dist[vertexIndex]) continue;

            // 인접 정점 탐색
            for (auto& edge : vertexs[vertexIndex].adjacencyList) {
                int nextVIndex = edge.first;
                int weight = edge.second;

                // 최단 거리 갱신
                if (dist[vertexIndex] + weight < dist[nextVIndex])
                {
                    dist[nextVIndex] = dist[vertexIndex] + weight;    // 최단 거리 갱신
                    prev[nextVIndex] = vertexIndex;   		          // 이전 정점 기록
                    pq.push({ dist[nextVIndex], nextVIndex });         // 우선순위큐에 추가
                }
            }
        }

        // 최단 거리
        cout << dist[goal] << endl;

        // 최단 경로 
        stack<int> path;
        for (int v = goal; v != -1; v = prev[v]) path.push(v);
        
		cout << path.size() << endl;

        while (!path.empty())
        {
            cout << vertexs[path.top()].data << " ";
            path.pop();
        }
    }
};

int main()
{
    // input
    int V, E;
    cin >> V >> E;

    // vertex
    Graph graph(V);

    // edge
    for (int i = 0; i < E; i++)
    {
        int from, to, weight;
        cin >> from >> to >> weight;
        graph.AddDirectedEdge(from, to, weight);
    }

    // start, goal
    int start, goal;
    cin >> start >> goal;

    // 최단 경로 알고리즘
    graph.Dijkstra(start, goal);

    return 0;
}