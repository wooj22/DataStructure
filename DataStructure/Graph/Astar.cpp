/*
    [A* �˰���] 
    A* �˰����� ����Ͽ� ����ư ��� �Ÿ�������� �̷θ� Ż���ϴ� ������Ʈ�Դϴ�.

    A* �˰����� �����ϸ鼭 ����ư ��� �Ÿ�������� �޸���ƽ �Լ��� ����� �̷��� �ⱸ���� �ִܰŸ� Ż���� �մϴ�.
    Ż�ⱸ�� �����ϸ� ���ۺ��� �ⱸ������ �ִ� Ż���� x,y��ǥ�� ����մϴ�.
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
    Position data;                               // ����
    vector<pair<int, int>> adjacencyList;        // ���� ����Ʈ {���� ���� index, ����ġ}
};

// Graph
struct Graph {
    vector<Vertex> vertexs;

    // index 0������ ���
    Graph() { AddNode(); }
    explicit Graph(int n) : vertexs(n + 1) {}

    // ���� �߰�
    void AddNode()
    {
        vertexs.push_back(Vertex());
    }

    // ���� ����
    void SetNodeData(int i, Position p)
    {
        vertexs[i].data = p;
    }

    // ���� ��� �߰�
    void AddDirectedEdge(int from, int to, int weight)
    {
        vertexs[from].adjacencyList.push_back({ to, weight });
    }

    int GetNodeCount() { return vertexs.size() - 1; }

    // �׷��� ���
    void PrintGraphNodeAndEdge() const
    {
        cout << "�׷����� ����մϴ�." << endl;

        for (int i = 1; i < vertexs.size(); ++i) {
            cout << vertexs[i].data << " :";
            for (auto& e : vertexs[i].adjacencyList)
                cout << " - " << vertexs[e.first].data << "," << e.second << " ";
            cout << endl;
        }
    }

    // �޸���ƽ ����ư �Ÿ� ���
    int HeuristicManhattanDistance(int fromIndex, int toIndex)
    {
        Position from = vertexs[fromIndex].data;
        Position to = vertexs[toIndex].data;

        int x = abs(from.x - to.x);
        int y = abs(from.y - to.y);

        return x + y;
    }
   

    /*  
    * [A* �˰���]
       ���ͽ�Ʈ���� �������� ��ǥ ��ġ������ ���� �Ÿ��� ����ؼ� Ž�� ������ �����Ѵ�.
       ���ͽ�Ʈ��� ��� ��θ� Ž���Ͽ� �������� �����ϰ�, 
       ���̽�Ÿ�� ��ǥ �������� �����Ͽ� �� ������ ������������ �ִ� ��θ� �˾Ƴ� �� �ִ�.
       �� �˰����� ���̴� ��ǥ ��ġ�� ���ο�, �켱����ť�� �߰��ϴ� ���̴�.
      
      - ���ͽ�Ʈ�� : ���� ����� ������ �ִ� �Ÿ��� �����ϸ� Ž��
        �켱 ���� ť�� ���� �� f(n)��?
        >> g(n) 
           ���������� ���� �������� ���� �Ÿ�
      
      - ���̽�Ÿ : ���� ����� ������ ��ǥ ��� ���������� �ִ� �Ÿ��� �����ϸ� Ž��
        �켱 ���� ť�� ���� �� f(n)��?
        >> g(n) + h(n)
           ���������� ���� �������� ���� �Ÿ� + ���� ��忡�� ��ǥ���� ���� ���(heuristic)
    */
    void Astar(int startNodeIndex, int goalNodeIndex) {
        const int INF = 1e9;

        // 1. start init
        vector<int> dist(vertexs.size(), INF);      // �ִ� �Ÿ� �迭
        vector<int> prev(vertexs.size(), -1);       // ���� ���� �迭 (��� ������)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // �ּ��� ��� �켱����ť (�Ÿ�, ���� ��ȣ)

        dist[startNodeIndex] = 0;
        pq.push({ 0, startNodeIndex });

        // 2~3. �ִ� ��� ����
        while (!pq.empty()) {
            auto currentVertex = pq.top();  pq.pop();
            int vertexIndex = currentVertex.second;   // ���� ��ȣ
            int curFDist = currentVertex.first;       // �����Ÿ� + ���� �Ÿ�

            // ��ǥ ��� ������ ����
            if (vertexIndex == goalNodeIndex) break;

            // (2) ���� ������ �̹� ��ϵȰͺ��� �����ɸ��ٸ� ����
            if (curFDist - HeuristicManhattanDistance(vertexIndex, goalNodeIndex) > dist[vertexIndex]) continue;

            // (3) ���� ���� Ž��
            for (auto& edge : vertexs[vertexIndex].adjacencyList) {
                int nextVIndex = edge.first;       // ���� ����
                int weight = edge.second;          // ����ġ

                // �ִ� �Ÿ� ���� g(n)
                if (dist[vertexIndex] + weight < dist[nextVIndex])
                {
                    dist[nextVIndex] = dist[vertexIndex] + weight;    // �ִ� �Ÿ� ����
                    prev[nextVIndex] = vertexIndex;   		          // ���� ���� ���

                    // �켱���� ť �߰� g(n) + h(n)
                    int gn = dist[nextVIndex];
                    int hn = HeuristicManhattanDistance(nextVIndex, goalNodeIndex);
                    pq.push({ gn + hn, nextVIndex });         // �켱����ť�� �߰�
                }
            }
        }

        // ��� ���
        cout << "\n=== ��� ===" << endl;
        cout << "���� ���� : " << vertexs[startNodeIndex].data << "   ���� ���� " << vertexs[goalNodeIndex].data << endl;
        cout << "�ִ� �Ÿ� : " << dist[goalNodeIndex] << endl;
        cout << "�ִ� ���" << endl;

        stack<int> path;
        for (int v = goalNodeIndex; v != -1; v = prev[v]) path.push(v);

        while (!path.empty())
        {
            cout << vertexs[path.top()].data << " ";
            path.pop();
        }
    }
};

// �׷��� ������ ����
void MakeGraphFormMaze(Graph& graph)
{
    int index = 1;                      // graph node index
    int nodeIndex[MAX][MAX] = {};       // maze �� ������ graph node index ����

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

    // edge (���� ����Ʈ)
    for (int y = 0; y < MAX; y++)
    {
        for (int x = 0; x < MAX; x++)
        {
            if (nodeIndex[y][x] == 0) continue;

            // fromIndex�� �������� �����¿� Ž��
            int fromIndex = nodeIndex[y][x];
            for (int dir = 0; dir < 4; dir++)
            {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx < 0 || nx >= MAX || ny < 0 || ny >= MAX) continue;
                if (nodeIndex[ny][nx] == 0) continue;

                // ���� ��� �߰�
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

	// A* �˰��� ���� (���۳�� index, ������� index)
    graph.Astar(1, graph.GetNodeCount());

    return 0;
}
