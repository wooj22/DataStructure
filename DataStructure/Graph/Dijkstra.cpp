#include <iostream>
#include <vector>
#include <queue>
#include <stack>
using namespace std;

/* ���� ����ü */
struct Vertex {
    int data;                                    // ����
    vector<pair<int, int>> adjacencyList;        // ���� ����Ʈ {���� ���� index, ����ġ}
};

struct Graph {
    vector<Vertex> vertexs;

    explicit Graph(int n) : vertexs(n) {}

    // ���� ����
    void SetNode(int i, int d) 
    {
        vertexs[i].data = d;
    }

    // ���� �߰�
    void AddDirectedEdge(int from, int to, int weight) 
    {
        vertexs[from].adjacencyList.push_back({ to, weight });
    }

    // �׷��� ���
    void PrintGraphNodeAndEdge() const 
    {
        cout << "�׷����� ����մϴ�." << endl;

        for (int i = 1; i < vertexs.size(); ++i) {
            cout << vertexs[i].data << " :";
            for (auto& e : vertexs[i].adjacencyList)
                cout << "(" << e.first << "," << e.second << ") ";
            cout << endl;
        }
    }

	// [���ͽ�Ʈ��(dijkstra) �ִ� ��� �˰���]
    /*  �ִ� ��� �迭 dist[], ��� ������ ���� ���� �迭 prev[], �ּ��� �켱���� ť pq
      
        1. �ִ� �Ÿ� �迭 dist[]�� ���Ѵ�� �ʱ�ȭ�ϰ� ���� ������ �Ÿ��� 0���� �����Ͽ� �켱���� ť�� ���� ������ �߰��Ѵ�.
        2. �켱���� ť���� �� �� ���(���� ����� ����)�� ������. �� ������ ����� �ִ� �Ÿ� �迭�� ����� �� ���� ũ�ٸ� �ǳʶڴ�.
        3. �켱���� ť�� ���� ������ ���� �������� Ž���Ѵ�. ������ �Ÿ� + ���� ���������� �Ÿ����� �ִ� �Ÿ� �迭�� ����Ǿ��ִ� ������ �۴ٸ� �ִ� ��� �������� �迭�� ������ �Ÿ� + ���� ���������� �Ÿ����� ����ϰ� ���ŵ� ���� ������ �켱���� ť�� �߰��Ѵ�.
        4. �켱���� ť�� �������� 2~3�� ������ �ݺ��Ѵ�. �˰����� ����Ǹ� �ִ� �Ÿ� �迭 dist[]���� ���� �������κ��� �� ���������� �ִ� �Ÿ��� ����Ǿ��ִ�.
    */
    void Dijkstra(int start) {
		const int INF = 1e9;

        // 1. start init
        vector<int> dist(vertexs.size(), INF);      // �ִ� �Ÿ� �迭
		vector<int> prev(vertexs.size(), -1);       // ���� ���� �迭 (��� ������)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // �ּ��� ��� �켱����ť (�Ÿ�, ���� ��ȣ)

        dist[start] = 0;
        pq.push({ 0, start});

        // 2~3. �ִ� ��� ����
        while (!pq.empty()) {
            // ���� �������� ���� ����� ���� cur V
            auto curV = pq.top();  pq.pop();
			int vertexIndex = curV.second;        // ���� ��ȣ
			int curDist = curV.first;             // ���������� ���� �Ÿ�

			// (2) ���� ������ �̹� �ִ� �Ÿ� ���ŵ� �����̶�� �ǳʶ�
            if (curDist > dist[vertexIndex]) continue;

			// (3) ���� ���� Ž��
            for (auto& edge : vertexs[vertexIndex].adjacencyList) {
                int nextVIndex = edge.first;
                int weight = edge.second;

                // �ִ� �Ÿ� ����
				// '�������������� �Ÿ� + ������������ �Ÿ�' < '�ִ� �Ÿ� �迭�� ����� ��' �̸� ����
                if (dist[vertexIndex] + weight < dist[nextVIndex]) 
                {
                    dist[nextVIndex] = dist[vertexIndex] + weight;    // �ִ� �Ÿ� ����
					prev[nextVIndex] = vertexIndex;   		          // ���� ���� ���
                    pq.push({ dist[nextVIndex], nextVIndex});         // �켱����ť�� �߰�
                }
            }
        }

        // �ִ� �Ÿ�
        cout << "\n=== �ִ� �Ÿ� ��� ===" << endl;
        for (int i = 1; i < vertexs.size(); ++i)
        {
            if (dist[i] == INF) cout << vertexs[i].data << ": INF\n";
            else cout << vertexs[i].data << ": " << dist[i] << "\n";
        }

        // �ִ� ���
        cout << "\n=== �ִ� ��� ��� ===" << endl;
        for (int goal = 1; goal < vertexs.size(); ++goal)
        {
            cout << vertexs[goal].data << " : ";
            if (dist[goal] == INF) 
            {
                cout << "��� ����\n";
                continue;
            }

            // ���� ���� �迭 ������
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

    // ����
    graph.SetNode(1, 1);
    graph.SetNode(2, 2);
    graph.SetNode(3, 3);
    graph.SetNode(4, 4);
    graph.SetNode(5, 5);

    // ����
    graph.AddDirectedEdge(1, 2, 8);
    graph.AddDirectedEdge(1, 3, 3);
    graph.AddDirectedEdge(2, 4, 4);
    graph.AddDirectedEdge(2, 5, 15);
    graph.AddDirectedEdge(3, 4, 13);
    graph.AddDirectedEdge(4, 5, 2);
    graph.PrintGraphNodeAndEdge();

    // �ִ� ��� �˰���
    graph.Dijkstra(1);

    return 0;
}
