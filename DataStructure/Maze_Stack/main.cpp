#include <iostream>
#include <stack>
#include <vector>
using namespace std;

#define MAX 10

// position
struct Position 
{ 
    int x, y; 
};

/*  // 표시 의미
    0 -> " " : 방문하지 않은 곳
    1 -> # : 이동할 수 없는 벽
    2 -> V : 한번 방문한 곳
    3 -> B : 방문하고 되돌아간 곳
*/
enum EMAZETYPE { PATH, WALL, VISIT, BACK };

// map
int Maze[MAX][MAX] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 0, 0, 0, 1, 0, 1, 1, 0, 1},
{1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
{1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
{1, 0, 1, 1, 1, 0, 0, 0, 0, 1},
{1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Show
void Show()
{
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            int n = Maze[i][j];
            switch (n)
            {
            case PATH:  cout << "  "; break;
            case WALL:  cout << "■"; break;
            case VISIT: cout << "V "; break;
            case BACK:  cout << "B "; break;
            default: break;
            }
        }
        cout << endl;
    }
}

// Move
bool Move(Position& pos, stack<Position>& moveStack)
{
    int go = Maze[pos.x][pos.y];

    // Back
    if (go == WALL || go == VISIT || go == BACK) return false;

    // move
    moveStack.push(pos);
    Maze[pos.x][pos.y] = VISIT;

    // goal
    if (pos.x == 8 && pos.y == 8) return true;

    // 탐색 - 상, 우, 하, 좌
    int xPos[4] = { -1, 0, 1, 0 };
    int yPos[4] = { 0, 1, 0, -1 };
    for (int i = 0; i < 4; i++)
    {
        Position next = { pos.x + xPos[i], pos.y + yPos[i] };
        if (Move(next, moveStack)) return true;
        Show();
    }

    // Back - 4 방향 모두 이동 불가
    Maze[pos.x][pos.y] = BACK;
    moveStack.pop();
    return false;
}

int main()
{
    stack<Position> moveStack;
    Position start = { 1, 1 };

    if (Move(start, moveStack))
    {
        Show();
        cout << "미로 탈출에 성공하셨습니다. 이동 경로를 출력합니다." << endl;
        while (!moveStack.empty())
        {
            cout << "[" << moveStack.top().x << ", " << moveStack.top().y << "] ";
            moveStack.pop();
        }
    }
}