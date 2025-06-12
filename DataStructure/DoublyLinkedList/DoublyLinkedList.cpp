#include <iostream>
using namespace std;

/* 이중연결리스트 과제 */
// 삽입, 검색, 삭제, 출력 구현
// iterator 구현

/* Node */
template<typename T>
struct Node {
	T data;							// 노드 데이터
	Node<T>* next = nullptr;		// 다음 노드 주소
	Node<T>* prev = nullptr;		// 이전 노드 주소
};

/* DoubleLinkedList */
template<typename T>
struct DoubleLinkedList {
	/// head, tail
	Node<T> head;
	Node<T> tail;

	/// Iterator
	struct Iterator
	{
		Node<T>* curNode = nullptr;
		Iterator(Node<T>* p) { curNode = p; }

		Iterator& operator++()
		{
			curNode = curNode->next;
			return *this;
		}
		T& operator*() { return curNode->data; }
		bool operator==(const Iterator& ref) { return  curNode == ref.curNode; }
		bool operator!=(const Iterator& ref) { return  curNode != ref.curNode; }
		void operator=(Node<T>* p) { curNode = p; }
	};

	Iterator begin() { return Iterator(head.next); }
	Iterator end() { return Iterator(&tail); }


	/// 중간 삽입 : prev노드 뒤에 노드를 생성하고 리턴
	Node<T>* insert(Node<T>* prevNode, const T& data)
	{
		// 1. 노드 생성
		Node<T>* newNode = new Node<T>();
		newNode->data = data;

		// 2. 리스트에 삽입
		// 1) 리스트가 비어있을 경우
		if (head.next == nullptr) {
			newNode->prev = &head;
			newNode->next = &tail;

			head.next = newNode;
			tail.prev = newNode;
		}
		// 2) 리스트가 비어있지 않을 경우
		else {
			// new node의 prev, next set
			newNode->prev = prevNode;
			newNode->next = prevNode->next;

			// next node의 prev set
			if (prevNode->next != &tail) prevNode->next->prev = newNode;
			else tail.prev = newNode;

			// prev node의 next set
			prevNode->next = newNode;
		}

		// 3. 노드 리턴
		return newNode;
	}

	/// 맨 뒤에 삽입
	void push_back(const T& data)
	{
		// 1. 노드 생성
		Node<T>* newNode = new Node<T>();
		newNode->data = data;

		// 2. 삽입
		// 1) 리스트가 비어있을 경우
		if (head.next == nullptr) {
			newNode->prev = &head;
			newNode->next = &tail;

			head.next = newNode;
			tail.prev = newNode;
		}
		// 2) 리스트가 비어있지 않을 경우
		else {
			// new node의 prev, next set
			newNode->prev = tail.prev;
			newNode->next = &tail;

			// prev node의 next set
			tail.prev->next = newNode;

			// next node의 prev set
			tail.prev = newNode;
		}
	}

	/// 검색 : 처음 발견한 노드 리턴
	Node<T>* find(const T& data)
	{
		for (Node<T>* findNode = head.next; findNode != &tail; findNode = findNode->next)
			if (findNode->data == data) return findNode;

		return nullptr;
	}

	/// 노드 삭제 : 노드를 지우고 다음 노드 리턴
	Node<T>* erase(Node<T>* removeNode)
	{
		removeNode->prev->next = removeNode->next;
		removeNode->next->prev = removeNode->prev;

		Node<T>* returnNode = removeNode->next;
		delete removeNode;
		return returnNode;
	}

	/// 삭제 : 전체노드를 순회하면서 같은 데이터면 해당 노드주소를 erase로 지운다.
	void remove(const T& data)
	{
		Node<T>* findNode = head.next;
		while (findNode != &tail)
		{
			if (findNode->data == data)
				findNode = erase(findNode);  // 삭제하고 다음 노드로
			else
				findNode = findNode->next;   // 다음 노드로
		}
	}
};

int main() {
	DoubleLinkedList<int> MyList;
	MyList.push_back(10);
	MyList.push_back(30);
	MyList.push_back(40);    // 추가 테스트

	Node<int>* pFind = MyList.find(30);
	MyList.erase(pFind);     // 삭제 테스트

	// 전체 목록을 출력
	DoubleLinkedList<int>::Iterator iter = MyList.begin();
	for (; iter != MyList.end(); ++iter)
	{
		cout << *iter << endl;
	}
}