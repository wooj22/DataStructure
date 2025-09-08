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

/*  // ǥ�� �ǹ�
    0 -> " " : �湮���� ���� ��
    1 -> # : �̵��� �� ���� ��
    2 -> V : �ѹ� �湮�� ��
    3 -> B : �湮�ϰ� �ǵ��ư� ��
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
            case WALL:  cout << "��"; break;
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

    // Ž�� - ��, ��, ��, ��
    int xPos[4] = { -1, 0, 1, 0 };
    int yPos[4] = { 0, 1, 0, -1 };
    for (int i = 0; i < 4; i++)
    {
        Position next = { pos.x + xPos[i], pos.y + yPos[i] };
        if (Move(next, moveStack)) return true;
        Show();
    }

    // Back - 4 ���� ��� �̵� �Ұ�
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
        cout << "�̷� Ż�⿡ �����ϼ̽��ϴ�. �̵� ��θ� ����մϴ�." << endl;
        while (!moveStack.empty())
        {
            cout << "[" << moveStack.top().x << ", " << moveStack.top().y << "] ";
            moveStack.pop();
        }
    }
}