#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;

/*
*   [삽입 정렬 (Insertion Sort)]
  배열을 정렬된 부분과 정렬되지 않은 부분으로 나누고,
  정렬되지 않은 부분에서 하나씩 꺼내서 정렬된 부분의 적절한 위치에 삽입하는 방식.
  첫번째 원소를 정렬된 부분으로 보고 두번째 원소부터 정렬을 시작한다.

  - 모든 원소의 삽입 위치를 순차적으로 찾아야 하므로 시간복잡도는 O(n^2)
  - 같은 값들의 순서가 정렬 후에도 같은 안정 정렬 방식
  - 거의 정렬된 데이터의 경우 이동(shift)가 거의 없으므로 빠르다.

*/
void InsertionSort(vector<int>& arr)
{
    // 두번쨰 원소부터 시작
    for (int i = 1; i < arr.size(); i++)
    {
        int key = arr[i];   // 정렬할 원소 arr[i]
        int j = i - 1;      // 정렬된 부분의 마지막 인덱스

        // arr[i] 삽입 위치 탐색
        while (j >= 0 && arr[j] > key)
        {
            // 이동(shift)
            arr[j + 1] = arr[j];    
            j--;
        }

        // arr[i] 삽입
        arr[j + 1] = key;
    }
}

// 시간 측정
long CheckSortingTime(vector<int>& arr)
{
    auto start = high_resolution_clock::now();

    InsertionSort(arr);

    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<microseconds>(end - start).count();

    return elapsed;
}

int main()
{
    const int N = 100;

    // 1. 역순 배열
    cout << "1. 역순 배열" << endl;
    vector<int> reverseArr(N);
    for (int i = 0; i < N; i++)
    {
        reverseArr[i] = N - i;
        cout << reverseArr[i] << ' ';
    }
        

    // 2. 부분 정렬 배열
    cout << endl << "2. 부분 정렬 배열" << endl;
    vector<int> halfSortedArr(N);
    for (int i = 0; i < N / 2; i++)
    {
        halfSortedArr[i] = i + 1;
        cout << halfSortedArr[i] << ' ';
    }
        
    for (int i = N / 2; i < N; i++)
    {
        halfSortedArr[i] = N - i;
        cout << halfSortedArr[i] << ' ';
    } 

    // 결과(us, 마이크로초 단위)
    cout << endl << endl << "---- 정렬 시간 -----" << endl;
    cout << "역순 배열 정렬 시간: " << CheckSortingTime(reverseArr) << "us" << endl;
    cout << "부분 정렬 배열 정렬 시간: " << CheckSortingTime(halfSortedArr) << "us" << endl;

    return 0;
}
