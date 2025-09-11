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
            else cout << setw(2) << n;  // 방문한 칸은 level 출력
        }
        cout << endl;
    }
}


// Move BFS - 너비 우선 탐색
bool MoveBFS(Position startPos, int& level, queue<Position>& moveQueue, vector<Position>& visit_result)
{
    Position curPos;                        // 현재 방문한 위치
    Position adjacentPos;                   // 인접한 위치
    bool visited[MAX][MAX] = { false };     // 방문 여부
    int xPos[4] = { -1, 0, 1, 0 };
    int yPos[4] = { 0, 1, 0, -1 };

    // init
    moveQueue.push(startPos);

    // 탐색
    while (!moveQueue.empty())
    {
        // 위치 방문
        curPos = moveQueue.front();
        moveQueue.pop();
        visited[curPos.x][curPos.y] = true;
        visit_result.push_back(curPos);
 
        // map에 level 기록
        Maze[curPos.x][curPos.y] = level;
        Show();

        // 도착 cheak
        if (curPos == goal) return true;

        // 인접 위치 push
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
        cout << "미로 탈출에 성공하셨습니다." << endl;
        cout << "Level : " << level << endl;
        cout << "최단 경로 : " << endl;
        for (auto pos : visit)
        {
            cout << "(" << pos.x << ", " << pos.y << ") ";
        }
    }
    else
    {
        cout << "탈출 경로를 찾을 수 없습니다." << endl;
    }

    return 0;
}