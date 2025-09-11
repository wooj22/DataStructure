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
            else cout << setw(2) << n;  // �湮�� ĭ�� level ���
        }
        cout << endl;
    }
}


// Move BFS - �ʺ� �켱 Ž��
bool MoveBFS(Position startPos, int& level, queue<Position>& moveQueue, vector<Position>& visit_result)
{
    Position curPos;                        // ���� �湮�� ��ġ
    Position adjacentPos;                   // ������ ��ġ
    bool visited[MAX][MAX] = { false };     // �湮 ����
    int xPos[4] = { -1, 0, 1, 0 };
    int yPos[4] = { 0, 1, 0, -1 };

    // init
    moveQueue.push(startPos);

    // Ž��
    while (!moveQueue.empty())
    {
        // ��ġ �湮
        curPos = moveQueue.front();
        moveQueue.pop();
        visited[curPos.x][curPos.y] = true;
        visit_result.push_back(curPos);
 
        // map�� level ���
        Maze[curPos.x][curPos.y] = level;
        Show();

        // ���� cheak
        if (curPos == goal) return true;

        // ���� ��ġ push
        for (int i = 0; i < 4; i++)
        {
            adjacentPos = { curPos.x + xPos[i], curPos.y + yPos[i] };
            int map = Maze[adjacentPos.x][adjacentPos.y];
            if (map == PATH) moveQueue.push(adjacentPos);
        }

        level++;
    }

    return false;
}

int main()
{
    queue<Position> moveQueue;
    vector<Position> visit;
    int level = 0;

    // start
    Position start = { 1,1, };
    if (MoveBFS(start, level, moveQueue, visit))
    {
        Show();
        cout << "�̷� Ż�⿡ �����ϼ̽��ϴ�." << endl;
        cout << "Level : " << level << endl;
        cout << "�ִ� ��� : " << endl;
        for (auto pos : visit)
        {
            cout << "(" << pos.x << ", " << pos.y << ") ";
        }
    }
    else
    {
        cout << "Ż�� ��θ� ã�� �� �����ϴ�." << endl;
    }

    return 0;
}