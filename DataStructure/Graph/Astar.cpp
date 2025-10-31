/*
    [A* 알고리즘] 
    A* 알고리즘을 사용하여 맨해튼 방식 거리계산으로 미로를 탈출하는 프로젝트입니다.

    A* 알고리즘을 구현하면서 맨해튼 방식 거리계산으로 휴리스틱 함수를 만들어 미로의 출구까지 최단거리 탈출을 합니다.
    탈출구에 도착하면 시작부터 출구까지의 최단 탈출경로 x,y좌표를 출력합니다.
*/

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

#define MAX 10

// Map
enum MAZETYPE { PATH, WALL };
int Maze[MAX][MAX] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 1, 0, 1},
{1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
{1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
{1, 0, 1, 1, 1, 0, 0, 0, 0, 1},
{1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Position
struct Position
{
    int x, y;
    bool operator==(Position& p) { return (x == p.x && y == p.y); }
    friend  ostream& operator<<(ostream& os, const Position& p)
    {
        os << "{" << p.x << ", " << p.y << "}";
        return os;
    }
};

// Node
struct Vertex {
    Position data;                               // 정점
    vector<pair<int, int>> adjacencyList;        // 인접 리스트 {목적 정점 index, 가중치}
};

// Graph
struct Graph {
    vector<Vertex> vertexs;

    // index 0번부터 사용
    Graph() { AddNode(); }
    explicit Graph(int n) : vertexs(n + 1) {}

    // 정점 추가
    void AddNode()
    {
        vertexs.push_back(Vertex());
    }

    // 정점 설정
    void SetNodeData(int i, Position p)
    {
        vertexs[i].data = p;
    }

    // 인접 노드 추가
    void AddDirectedEdge(int from, int to, int weight)
    {
        vertexs[from].adjacencyList.push_back({ to, weight });
    }

    int GetNodeCount() { return vertexs.size() - 1; }

    // 그래프 출력
    void PrintGraphNodeAndEdge() const
    {
        cout << "그래프를 출력합니다." << endl;

        for (int i = 1; i < vertexs.size(); ++i) {
            cout << vertexs[i].data << " :";
            for (auto& e : vertexs[i].adjacencyList)
                cout << " - " << vertexs[e.first].data << "," << e.second << " ";
            cout << endl;
        }
    }

    // 휴리스틱 맨해튼 거리 계산
    int HeuristicManhattanDistance(int fromIndex, int toIndex)
    {
        Position from = vertexs[fromIndex].data;
        Position to = vertexs[toIndex].data;

        int x = abs(from.x - to.x);
        int y = abs(from.y - to.y);

        return x + y;
    }
   

    /*  
    * [A* 알고리즘]
       다익스트라의 변형으로 목표 위치까지의 예상 거리를 고려해서 탐색 방향을 조정한다.
       다익스트라는 모든 경로를 탐색하여 느리지만 완전하고, 
       에이스타는 목표 방향으로 집중하여 더 빠르게 목적지까지의 최단 경로를 알아낼 수 있다.
       두 알고리즘의 차이는 목표 위치의 여부와, 우선순위큐에 추가하는 값이다.
      
      - 다익스트라 : 가장 가까운 노드부터 최단 거리를 갱신하며 탐색
        우선 순위 큐에 들어가는 값 f(n)은?
        >> g(n) 
           시작점에서 현재 노드까지의 누적 거리
      
      - 에이스타 : 가장 가까운 노드부터 목표 노드 방향으로의 최단 거리를 갱신하며 탐색
        우선 순위 큐에 들어가는 값 f(n)은?
        >> g(n) + h(n)
           시작점에서 현재 노드까지의 누적 거리 + 현재 노드에서 목표까지 예상 비용(heuristic)
    */
    void Astar(int startNodeIndex, int goalNodeIndex) {
        const int INF = 1e9;

        // 1. start init
        vector<int> dist(vertexs.size(), INF);      // 최단 거리 배열
        vector<int> prev(vertexs.size(), -1);       // 이전 정점 배열 (경로 추적용)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // 최소힙 기반 우선순위큐 (거리, 정점 번호)

        dist[startNodeIndex] = 0;
        pq.push({ 0, startNodeIndex });

        // 2~3. 최단 경로 갱신
        while (!pq.empty()) {
            auto currentVertex = pq.top();  pq.pop();
            int vertexIndex = currentVertex.second;   // 정점 번호
            int curFDist = currentVertex.first;       // 누적거리 + 예상 거리

            // 목표 노드 도착시 종료
            if (vertexIndex == goalNodeIndex) break;

            // (2) 꺼낸 정점이 이미 기록된것보다 오래걸린다면 무시
            if (curFDist - HeuristicManhattanDistance(vertexIndex, goalNodeIndex) > dist[vertexIndex]) continue;

            // (3) 인접 정점 탐색
            for (auto& edge : vertexs[vertexIndex].adjacencyList) {
                int nextVIndex = edge.first;       // 목적 정점
                int weight = edge.second;          // 가중치

                // 최단 거리 갱신 g(n)
                if (dist[vertexIndex] + weight < dist[nextVIndex])
                {
                    dist[nextVIndex] = dist[vertexIndex] + weight;    // 최단 거리 갱신
                    prev[nextVIndex] = vertexIndex;   		          // 이전 정점 기록

                    // 우선순위 큐 추가 g(n) + h(n)
                    int gn = dist[nextVIndex];
                    int hn = HeuristicManhattanDistance(nextVIndex, goalNodeIndex);
                    pq.push({ gn + hn, nextVIndex });         // 우선순위큐에 추가
                }
            }
        }

        // 결과 출력
        cout << "\n=== 결과 ===" << endl;
        cout << "시작 지점 : " << vertexs[startNodeIndex].data << "   도착 지점 " << vertexs[goalNodeIndex].data << endl;
        cout << "최단 거리 : " << dist[goalNodeIndex] << endl;
        cout << "최단 경로" << endl;

        stack<int> path;
        for (int v = goalNodeIndex; v != -1; v = prev[v]) path.push(v);

        while (!path.empty())
        {
            cout << vertexs[path.top()].data << " ";
            path.pop();
        }
    }
};

// 그래프 데이터 생성
void MakeGraphFormMaze(Graph& graph)
{
    int index = 1;                      // graph node index
    int nodeIndex[MAX][MAX] = {};       // maze 맵 구조에 graph node index 저장

    // node
    for (int y = 0; y < MAX; y++)
    {
        for (int x = 0; x < MAX; x++)
        {
            if (Maze[y][x] == WALL) continue;

            // position data set
            graph.AddNode();
            graph.SetNodeData(index, { x, y });
            nodeIndex[y][x] = index;
            index++;
        }
    }

    int dx[4] = { 1, -1, 0, 0 };
    int dy[4] = { 0, 0, 1, -1 };

    // edge (인접 리스트)
    for (int y = 0; y < MAX; y++)
    {
        for (int x = 0; x < MAX; x++)
        {
            if (nodeIndex[y][x] == 0) continue;

            // fromIndex를 기준으로 상하좌우 탐색
            int fromIndex = nodeIndex[y][x];
            for (int dir = 0; dir < 4; dir++)
            {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx < 0 || nx >= MAX || ny < 0 || ny >= MAX) continue;
                if (nodeIndex[ny][nx] == 0) continue;

                // 인접 노드 추가
                int toIndex = nodeIndex[ny][nx];
                graph.AddDirectedEdge(fromIndex, toIndex, 1);
            }
        }
    }
}


int main() {
    Graph graph;
    MakeGraphFormMaze(graph);
    graph.PrintGraphNodeAndEdge();

	// A* 알고리즘 실행 (시작노드 index, 도착노드 index)
    graph.Astar(1, graph.GetNodeCount());

    return 0;
}
