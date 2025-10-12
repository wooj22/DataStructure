/*
 [���� 1260��] DFS�� BFS

 �׷����� DFS�� Ž���� ����� BFS�� Ž���� ����� ����ϴ� ���α׷��� �ۼ��Ͻÿ�. 
 ��, �湮�� �� �ִ� ������ ���� ���� ��쿡�� ���� ��ȣ�� ���� ���� ���� �湮�ϰ�,
 �� �̻� �湮�� �� �ִ� ���� ���� ��� �����Ѵ�. ���� ��ȣ�� 1������ N�������̴�.

 �Է�
 ù° �ٿ� ������ ���� N(1 �� N �� 1,000), ������ ���� M(1 �� M �� 10,000), 
 Ž���� ������ ������ ��ȣ V�� �־�����. 
 ���� M���� �ٿ��� ������ �����ϴ� �� ������ ��ȣ�� �־�����. 
 � �� ���� ���̿� ���� ���� ������ ���� �� �ִ�. 
 �Է����� �־����� ������ ������̴�.

 ���
 ù° �ٿ� DFS�� ������ �����, �� ���� �ٿ��� BFS�� ������ ����� ����Ѵ�.
 V���� �湮�� ���� ������� ����ϸ� �ȴ�.
*/

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

int N, M, V;
vector<int> graph[1001];     // �� ������ ���� ����Ʈ
bool visited[1001];          // DFS�� �湮 üũ

// ���� �켱 Ž��
void DFS(int node) {
    visited[node] = true;
    cout << node << ' ';
    for (int next : graph[node]) {
        if (!visited[next]) {
            DFS(next);
        }
    }
}

// �ʺ� �켱 Ž��
void BFS(int start) {
    bool visitedBFS[1001] = { false };
    queue<int> q;
    q.push(start);
    visitedBFS[start] = true;

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        cout << node << ' ';
        for (int next : graph[node]) {
            if (!visitedBFS[next]) {
                visitedBFS[next] = true;
                q.push(next);
            }
        }
    }
}

int main() {
    cin >> N >> M >> V;

    for (int i = 0; i < M; i++) {
        int a, b;
        cin >> a >> b;
        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    // ���� ��ȣ���� �湮�ϱ� ���� �� ���� ����Ʈ ����
    for (int i = 1; i <= N; i++) 
    {
        sort(graph[i].begin(), graph[i].end());
    }

    DFS(V);
    cout << '\n';
    BFS(V);

    return 0;
}
