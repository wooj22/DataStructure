#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

/* 
    자연수 N(1 ≤ N ≤ 10,000,000)에 대해서,
    연속된 자연수의 합 N이 되는 경우의 수를 계산하는 프로그램
*/


// 1. 완전 탐색 방식 - O(N²)
int BruteForce(int N) {
    int result_count = 0;   // n이 되는 경우의 수

    // 1부터 n까지 모든 경우의 수를 직접 탐색
    for (int start = 1; start <= N; start++) {
        int total = 0;

        for (int end = start; end <= N; end++) {
            total += end;

            // 구간합이 n이 되는 경우 발견
            if (total == N) 
            {
                result_count++;
                break;
            }

            if (total > N) break;
        }
    }

    return result_count;
}

// 2. 투 포인터 방식 - O(N)
int TwoPointers(int N) {
    int start = 1, end = 1;         // 구간의 시작과 끝 index
    int total = 1;                  // start ~ end의 구간합
    int result_count = 0;           // n이 되는 경우의 수

    // start와 end를 한 방향으로 옮겨가며 탐색
    // start와 end의 이동에 따라 total에 +,-하여 total의 중복된 구간을 재연산하지 않도록 함
    while (start <= N) {
        // 구간합이 n이 되는 경우 발견
        if (total == N) {
            result_count++;
            total -= start;
            start++;
        }
        // 구간 확대
        else if (total < N) {
            end++;
            if (end > N) break;
            total += end;
        }
        // 구간 축소
        else { // total > N
            total -= start;
            start++;
        }
    }

    return result_count;
}

int main() {
    // time
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // input
    int n = 10000000;

    // 완전 탐색
    auto start1 = high_resolution_clock::now();
    int result1 = BruteForce(n);
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(end1 - start1).count();

    // 투 포인터
    auto start2 = high_resolution_clock::now();
    int result2 = TwoPointers(n);
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<milliseconds>(end2 - start2).count();

    cout << "완전 탐색과 투 포인터의 연산 시간을 비교합니다. (입력값 10,000,000)" << endl;
    cout << "완전 탐색 : " << result1 << "가지 (경과 시간 : " << duration1 << " ms)\n";
    cout << "투 포인터 : " << result2 << "가지 (경과 시간 : " << duration2 << " ms)\n";

    return 0;
}
