#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int main() {
	std::priority_queue<int, std::vector<int>, std::greater<int>> max_priority_queue;

	max_priority_queue.push(13);
	max_priority_queue.push(6);
	max_priority_queue.push(1);
	max_priority_queue.push(24);
	max_priority_queue.push(50);

	cout << max_priority_queue.top();

	vector<int> test(10);
	for (auto n : test)
	{
		cout << n;
	}

	cout << test[2];
}
