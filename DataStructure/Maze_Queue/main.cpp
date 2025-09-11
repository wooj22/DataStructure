#include <iostream>
#include <queue>
#include <vector>
#include <iomanip>
using namespace std;

#define MAX 10

// position
struct Position
{
    int x, y;
    bool operator==(Position& p) { return (x == p.x && y == p.y); }
};

// Node
struct Node
{
    Position position;
    int level;
};

/*  // ǥ�� �ǹ�
    0 -> " " : �湮���� ���� ��
    1 -> # : �̵��� �� ���� ��
*/
enum EMAZETYPE { PATH, WALL};

// map
int Maze[MAX][MAX] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 1, 0, 1},
{1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
{1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
{1, 0, 1, 1, 1, 0, 0, 0, 0, 1},
{1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};
Position goal = { 8, 6 };

// Show
void Show()
{
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            int n = Maze[i][j];
            if (n == PATH) cout << "  ";
            else if (n == WALL) cout << "��";
            else cout << setw(2) << n;          // �湮�� ĭ�� level ���
        }
        cout << endl;
    }
}

// Move BFS - �ʺ� �켱 Ž��
int MoveBFS(Position startPos, int startlevel, queue<Node>& moveQueue, vector<Node>& visit_result)
{
    bool visited[MAX][MAX] = { false };     // �湮����
    int xPos[4] = { -1, 0, 1, 0 };          // ���� ��� ��ǥ
    int yPos[4] = { 0, 1, 0, -1 };

    // start init
    moveQueue.push({ startPos.x, startPos.y, 0 });
    visited[startPos.x][startPos.y] = true;

    // Ž��
    while (!moveQueue.empty())
    {
        // �湮
        Node cur = moveQueue.front();
        moveQueue.pop();
        visit_result.push_back(cur);

        // map�� level ���
        Maze[cur.position.x][cur.position.y] = cur.level+ startlevel;  // map���� 0, 1 ������̶�
        Show();

        // ���� check
        if (cur.position == goal) return cur.level;

        // ���� ��ġ push
        for (int i = 0; i < 4; i++)
        {
            int nx = cur.position.x + xPos[i];
            int ny = cur.position.y + yPos[i];

            if (!visited[nx][ny] && Maze[nx][ny] == PATH) 
            {
                visited[nx][ny] = true;
                moveQueue.push({ nx, ny, cur.level + 1 });
            }
        }
    }

    return -1;
}

int main()
{
    queue<Node> moveQueue;
    vector<Node> visit_result;
    int startLevel = 10;
    int level;

    // start
    Position start = { 1,1, };
    level = MoveBFS(start, startLevel, moveQueue, visit_result);
    if (level != -1)
    {
        Show();
        cout << "�̷� Ż�⿡ �����ϼ̽��ϴ�." << endl;
        cout << "Level : " << level << endl;
        cout << "�ִ� ��� : " << endl;
        for (auto pos : visit_result)
        {
            cout << "(" << pos.position.x << ", " << pos.position.y << ") ";
        }
    }
    else
    {
        cout << "Ż�� ��θ� ã�� �� �����ϴ�." << endl;
    }

    return 0;
}