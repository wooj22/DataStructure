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

/*  // 표시 의미
    0 -> " " : 방문하지 않은 곳
    1 -> # : 이동할 수 없는 벽
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
            else if (n == WALL) cout << "■";
            else cout << setw(2) << n;          // 방문한 칸은 level 출력
        }
        cout << endl;
    }
}

// Move BFS - 너비 우선 탐색
int MoveBFS(Position startPos, int startlevel, queue<Node>& moveQueue, vector<Node>& visit_result)
{
    bool visited[MAX][MAX] = { false };     // 방문여부
    int xPos[4] = { -1, 0, 1, 0 };          // 인접 노드 좌표
    int yPos[4] = { 0, 1, 0, -1 };

    // start init
    moveQueue.push({ startPos.x, startPos.y, 0 });
    visited[startPos.x][startPos.y] = true;

    // 탐색
    while (!moveQueue.empty())
    {
        // 방문
        Node cur = moveQueue.front();
        moveQueue.pop();
        visit_result.push_back(cur);

        // map에 level 기록
        Maze[cur.position.x][cur.position.y] = cur.level+ startlevel;  // map에서 0, 1 사용중이라서
        Show();

        // 도착 check
        if (cur.position == goal) return cur.level;

        // 인접 위치 push
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
        cout << "미로 탈출에 성공하셨습니다." << endl;
        cout << "Level : " << level << endl;
        cout << "최단 경로 : " << endl;
        for (auto pos : visit_result)
        {
            cout << "(" << pos.position.x << ", " << pos.position.y << ") ";
        }
    }
    else
    {
        cout << "탈출 경로를 찾을 수 없습니다." << endl;
    }

    return 0;
}