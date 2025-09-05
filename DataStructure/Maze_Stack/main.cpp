#include <iostream>
#include <stack>
#include <string>
using namespace std;

#define MAX 10

struct Position
{
	int x, y;
};

enum EMAZETYPE { PATH, WALL, VISIT, BACK };
/*
0 -> PATH : 방문하지 않은 위치
1 -> WALL : 이동할 수 없는 벽
2 -> VISIT : 한번 방문한곳
3 -> BACK : 방문하고 되돌아간 곳
*/

// 미로 맵 - 출구 : Maze[9][9] 
int Maze[MAX][MAX] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 2, 0, 0, 1, 0, 1, 1, 0, 1},
{1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
{1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
{1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
{1, 0, 1, 1, 1, 0, 0, 0, 0, 1},
{1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

void Show()
{
	for (int i = 0; i < MAX; i++)
	{
		for (int j = 0; j < MAX; j++)
		{
			int n = Maze[i][j];

			switch (n)
			{
			case PATH:
				cout << "  ";
				break;
			case WALL:
				cout << "■";
				break;
			case VISIT:
				cout << "V ";
				break;
			case BACK:
				cout << "B ";
				break;
			default:
				break;
			}
		}
		cout << endl;
	}
}

bool Move(Position &pos, stack<Position> &s)
{
	// check
	s.push(pos);
	int p = Maze[pos.x][pos.y];

	switch (p)
	{
	case PATH:
		Maze[pos.x][pos.y] = VISIT;
		break;
	case WALL:
		s.pop();
		break;
	case VISIT:
		Maze[pos.x][pos.y] = BACK;
		break;
	case BACK:
		break;
	default:
		break;
	}
	
	// final pos
	pos = s.top();

	// show
	Show();

	// goal
	if (pos.x == 8 && pos.y == 8) return true;
	else return false;
}

int main()
{
	// data
	stack<Position> s;
	bool isGoal = false;
	string input;

	// init
	Position pos = { 1, 1 };
	s.push(pos);
	Show();

	// play
	while (!isGoal)
	{
		// input
		cout << "이동 방향을 입력해주세요 (WASD) : ";
		cin >> input;
		if (input == "w" || input == "W") pos.x--;
		else if (input == "s" || input == "S") pos.x++;
		else if (input == "a" || input == "A") pos.y--;
		else if (input == "d" || input == "D") pos.y++;
		else {
			cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
			continue;
		}

		// move
		isGoal = Move(pos, s);
	}

	// goal
	cout << "미로 탈출에 성공하셨습니다.";
}