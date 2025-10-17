/*
* [���� 11779�� : �ּ� ��� ���ϱ�2]
* n(1��n��1,000)���� ���ð� �ִ�. �׸��� �� ���ÿ��� ����Ͽ� �ٸ� ���ÿ� �����ϴ� m(1��m��100,000)���� ������ �ִ�.
  �츮�� A��° ���ÿ��� B��° ���ñ��� ���µ� ��� ���� ����� �ּ�ȭ ��Ű���� �Ѵ�.
  �׷��� A��° ���ÿ��� B��° ���� ���� ���µ� ��� �ּҺ��� ��θ� ����Ͽ���.
  �׻� ���������� ������������ ��ΰ� �����Ѵ�.

  ù° �ٿ� ������ ���� n(1��n��1,000)�� �־����� ��° �ٿ��� ������ ���� m(1��m��100,000)�� �־�����.
  �׸��� ��° �ٺ��� m+2�ٱ��� ������ ���� ������ ������ �־�����.
  ���� ó������ �� ������ ��� ������ ��ȣ�� �־�����.
  �׸��� �� �������� �������� ���� ��ȣ�� �־����� �� �� ���� ����� �־�����.
  ���� ����� 0���� ũ�ų� ����, 100,000���� ���� �����̴�.
  �׸��� m+3° �ٿ��� �츮�� ���ϰ��� �ϴ� ���� ������� ���ù�ȣ�� �������� ���ù�ȣ�� �־�����.

  ù° �ٿ� ��� ���ÿ��� ���� ���ñ��� ���µ� ��� �ּ� ����� ����Ѵ�.
  ��° �ٿ��� �׷��� �ּ� ����� ���� ��ο� ���ԵǾ��ִ� ������ ������ ����Ѵ�. ��� ���ÿ� ���� ���õ� �����Ѵ�.
  ��° �ٿ��� �ּ� ����� ���� ��θ� �湮�ϴ� ���� ������� ����Ѵ�. ��ΰ� ���������� ��� �ƹ��ų� �ϳ� ����Ѵ�.
*/


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

    explicit Graph(int n) : vertexs(n + 1)
    {
        // 0�� �ε��� ��� x
        for (int i = 1; i < vertexs.size(); ++i)
            vertexs[i].data = i;
    }

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

    // �ִ� ��� ã��
    void Dijkstra(int start, int goal) {
        const int INF = 1e9;

        // start init
        vector<int> dist(vertexs.size(), INF);      // �ִ� �Ÿ� �迭
        vector<int> prev(vertexs.size(), -1);       // ���� ���� �迭 (��� ������)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // �ּ��� ��� �켱����ť (�Ÿ�, ���� ��ȣ)

        dist[start] = 0;
        pq.push({ 0, start });

        // �ִ� ��� ����
        while (!pq.empty()) {
            // ���� �������� ���� ����� ���� cur V
            auto curV = pq.top();  pq.pop();
            int vertexIndex = curV.second;
            int curDist = curV.first;

            // ���� ������ �̹� �ִ� �Ÿ� ���ŵ� �����̶�� �ǳʶ�
            if (curDist > dist[vertexIndex]) continue;

            // ���� ���� Ž��
            for (auto& edge : vertexs[vertexIndex].adjacencyList) {
                int nextVIndex = edge.first;
                int weight = edge.second;

                // �ִ� �Ÿ� ����
                if (dist[vertexIndex] + weight < dist[nextVIndex])
                {
                    dist[nextVIndex] = dist[vertexIndex] + weight;    // �ִ� �Ÿ� ����
                    prev[nextVIndex] = vertexIndex;   		          // ���� ���� ���
                    pq.push({ dist[nextVIndex], nextVIndex });         // �켱����ť�� �߰�
                }
            }
        }

        // �ִ� �Ÿ�
        cout << dist[goal] << endl;

        // �ִ� ��� 
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

    // �ִ� ��� �˰���
    graph.Dijkstra(start, goal);

    return 0;
}