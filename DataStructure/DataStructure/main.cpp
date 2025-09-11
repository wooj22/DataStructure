#include <iostream>
#include <stack>
#include <vector>
using namespace std;

int main() {
	// input
	int n; cin >> n;
	
	vector<int> inputs(n);
	for (int& n : inputs) cin >> n;

	// process
	stack<int> stack;
	vector<int> result;
	int curIndex = 0;
	int curValue;
	int curLeftMax = -1;
	
	for (int i = 0; i < n; i++)
	{
		curValue = inputs[curIndex];

		// ������ ū �� ��� ���
		for (int j = i+1; j < n; j++)
		{
			if (curValue < inputs[j]) stack.push(inputs[j]);
		}

		// ū ���� ���� ���� �� ���
		while (!stack.empty())
		{
			curLeftMax = stack.top();
			stack.pop();
		}

		// result
		result.push_back(curLeftMax);
		curLeftMax = -1;
	}

	for (auto n : result)
	{
		cout << n << " ";
	}
}
