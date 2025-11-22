/*
	1.다수의 AABB 배열을 준비하고 배열 기반의 정적 BVH 트리를 구성합니다. (BVH2)
	2. 트리 빌드방법은 오브젝트 수 기반 하향식 빌드를 사용합니다.
	3. 큐브의 카메라 컬링을 단순 전체 루프(Brute-force) 와 BVH 트리를 이용한 컬링 을
	선택 작동 하게하고 걸린시간과 FPS를 출력합니다.
	
	#include <DirectXCollision.h> 파일에 AABB, Frustum 클래스를 사용합니다.
*/
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <stack>
#include <cstdint>

using namespace std;
using namespace DirectX;
using Clock = std::chrono::high_resolution_clock;
using ms = std::chrono::duration<double, std::milli>;


/* Frustum */ 
struct Frustum
{
    // 평면 6개: ax + by + cz + d = 0
    XMVECTOR planes[6];

    // ViewProj 매트릭스로부터 프러스텀 평면 계산
    void BuildFromMatrix(FXMMATRIX viewProj)
    {
        XMMATRIX m = XMMatrixTranspose(viewProj);
        XMVECTOR row0 = m.r[0];
        XMVECTOR row1 = m.r[1];
        XMVECTOR row2 = m.r[2];
        XMVECTOR row3 = m.r[3];

        // 6개 평면 추출
        planes[0] = row3 + row0;  // Left
        planes[1] = row3 - row0;  // Right
        planes[2] = row3 + row1;  // Bottom
        planes[3] = row3 - row1;  // Top
        planes[4] = row2;         // Near
        planes[5] = row3 - row2;  // Far

        // Normalized plane
        for (int i = 0; i < 6; ++i)
        {
            XMVECTOR normal = XMVectorSetW(planes[i], 0.0f);
            XMVECTOR len = XMVector3Length(normal);
            planes[i] = XMVectorDivide(planes[i], len);
        }
    }

    // AABB와 프러스텀 교차 여부 체크
    bool Intersects(const BoundingBox& box) const
    {
        XMVECTOR center = XMLoadFloat3(&box.Center);
        XMVECTOR extents = XMLoadFloat3(&box.Extents);

        for (int i = 0; i < 6; ++i)
        {
            XMVECTOR p = planes[i];
            XMVECTOR normal = XMVectorSetW(p, 0.0f);
            float d = XMVectorGetW(p);

            XMVECTOR absn = XMVectorAbs(normal);
            float r = XMVectorGetX(XMVector3Dot(extents, absn));

            float dist = XMVectorGetX(XMVector3Dot(normal, center)) + d;

            // 완전히 밖이면 false
            if (dist + r < 0.0f)
                return false;
        }
        return true;
    }
};


/* BVHNode */
struct BVHNode
{
    BoundingBox bounds;
    int left;
    int right;
    int start;
    int count;

    BVHNode()
        : left(-1), right(-1), start(-1), count(0)
    {
        bounds.Center = { 0,0,0 };
        bounds.Extents = { 0,0,0 };
    }

    bool IsLeaf() const { return (left == -1 && right == -1); }
};


// get AABB 합집합 BoundingBox
static BoundingBox UnionBoxes(const BoundingBox& a, const BoundingBox& b)
{
    XMFLOAT3 amin = { a.Center.x - a.Extents.x, a.Center.y - a.Extents.y, a.Center.z - a.Extents.z };
    XMFLOAT3 amax = { a.Center.x + a.Extents.x, a.Center.y + a.Extents.y, a.Center.z + a.Extents.z };
    XMFLOAT3 bmin = { b.Center.x - b.Extents.x, b.Center.y - b.Extents.y, b.Center.z - b.Extents.z };
    XMFLOAT3 bmax = { b.Center.x + b.Extents.x, b.Center.y + b.Extents.y, b.Center.z + b.Extents.z };

    XMFLOAT3 minPt = { std::min(amin.x, bmin.x), std::min(amin.y, bmin.y), std::min(amin.z, bmin.z) };
    XMFLOAT3 maxPt = { std::max(amax.x, bmax.x), std::max(amax.y, bmax.y), std::max(amax.z, bmax.z) };

    BoundingBox out;
    out.Center = { (minPt.x + maxPt.x) * 0.5f, (minPt.y + maxPt.y) * 0.5f, (minPt.z + maxPt.z) * 0.5f };
    out.Extents = { (maxPt.x - minPt.x) * 0.5f, (maxPt.y - minPt.y) * 0.5f, (maxPt.z - minPt.z) * 0.5f };
    return out;
}


// Build BVHNode (하향식 - 오브젝트 수 기반)
int BuildBVHNode(vector<BVHNode>& nodes, vector<int>& objIndices, const vector<BoundingBox>& boxes, 
    int start, int end, int leafMax = 4)
{
    int nodeIndex = (int)nodes.size();
    nodes.emplace_back();
    BVHNode& node = nodes.back();

    // 현재 노드의 AABB 계산
    BoundingBox bounds = boxes[objIndices[start]];
    for (int i = start + 1; i < end; ++i)
        bounds = UnionBoxes(bounds, boxes[objIndices[i]]);
    node.bounds = bounds;

    int count = end - start;

    // Leaf 조건
    if (count <= leafMax)
    {
        node.start = start;
        node.count = count;
        node.left = node.right = -1;
        return nodeIndex;
    }

    // 중심점 기준 분할축 선택
    XMFLOAT3 cmin = { FLT_MAX, FLT_MAX, FLT_MAX };
    XMFLOAT3 cmax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int i = start; i < end; ++i)
    {
        XMFLOAT3 c = boxes[objIndices[i]].Center;
        cmin.x = std::min(cmin.x, c.x);
        cmin.y = std::min(cmin.y, c.y);
        cmin.z = std::min(cmin.z, c.z);

        cmax.x = std::max(cmax.x, c.x);
        cmax.y = std::max(cmax.y, c.y);
        cmax.z = std::max(cmax.z, c.z);
    }

    XMFLOAT3 e = { cmax.x - cmin.x, cmax.y - cmin.y, cmax.z - cmin.z };

    int axis = 0;
    if (e.y > e.x && e.y >= e.z) axis = 1;
    else if (e.z > e.x && e.z > e.y) axis = 2;

    // 긴 축 기준 정렬
    std::sort(objIndices.begin() + start, objIndices.begin() + end,
        [&](int a, int b)
        {
            const auto& A = boxes[a];
            const auto& B = boxes[b];
            if (axis == 0) return A.Center.x < B.Center.x;
            if (axis == 1) return A.Center.y < B.Center.y;
            return A.Center.z < B.Center.z;
        });

    int mid = (start + end) / 2;

    // 완전 동일한 경우 -> 리프 처리
    bool same = true;
    for (int i = start + 1; i < end; ++i)
    {
        auto& p = boxes[objIndices[i - 1]].Center;
        auto& q = boxes[objIndices[i]].Center;
        if (p.x != q.x || p.y != q.y || p.z != q.z) { same = false; break; }
    }

    if (same)
    {
        node.start = start;
        node.count = count;
        node.left = node.right = -1;
        return nodeIndex;
    }

    // 자식 노드 재귀 생성
    node.left = BuildBVHNode(nodes, objIndices, boxes, start, mid, leafMax);
    node.right = BuildBVHNode(nodes, objIndices, boxes, mid, end, leafMax);

    return nodeIndex;
}


//BVH Frustum Culling
int BVHFrustumCulling(const vector<BVHNode>& nodes, const vector<int>& objIndices,
    const vector<BoundingBox>& boxes, const Frustum& frustum)
{
    if (nodes.empty()) return 0;

    int visible = 0;
    std::stack<int> st;
    st.push(0);

    while (!st.empty())
    {
        int ni = st.top();
        st.pop();

        const BVHNode& node = nodes[ni];

        if (!frustum.Intersects(node.bounds))
            continue;

        // Leaf
        if (node.IsLeaf())
        {
            for (int i = 0; i < node.count; ++i)
            {
                int obj = objIndices[node.start + i];
                if (frustum.Intersects(boxes[obj]))
                    visible++;
            }
        }
        else
        {
            if (node.left != -1) st.push(node.left);
            if (node.right != -1) st.push(node.right);
        }
    }

    return visible;
}


// Brute Force Culling
int BruteForceCulling(const vector<BoundingBox>& boxes, const Frustum& fr)
{
    int vis = 0;
    for (auto& b : boxes)
        if (fr.Intersects(b)) vis++;
    return vis;
}


XMMATRIX MakePerspectiveFovLH(float fovY, float aspect, float zn, float zf)
{
    return XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
}

XMMATRIX MakeLookAtLH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up)
{
    return XMMatrixLookAtLH(eye, at, up);
}

//------------------------------------------------------------------
int main()
{
    const int NUM_OBJECTS = 30000;
    const int LEAF_MAX = 8;
    const int FRAMES = 120;
    const float WORLD_SIZE = 2000.0f;
    const float MAX_EXT = 8.0f;

    // 랜덤 박스 생성
    mt19937 rng(12345);
    uniform_real_distribution<float> pos(-WORLD_SIZE, WORLD_SIZE);
    uniform_real_distribution<float> size(1.0f, MAX_EXT);

    vector<BoundingBox> boxes;
    boxes.reserve(NUM_OBJECTS);

    for (int i = 0; i < NUM_OBJECTS; ++i)
    {
        XMFLOAT3 c = { pos(rng), pos(rng) * 0.5f, pos(rng) };
        float sx = size(rng), sy = size(rng), sz = size(rng);

        BoundingBox b;
        b.Center = c;
        b.Extents = { sx * 0.5f, sy * 0.5f, sz * 0.5f };
        boxes.push_back(b);
    }

    // BVH 빌드
    vector<int> objIndices(NUM_OBJECTS);
    for (int i = 0; i < NUM_OBJECTS; ++i) objIndices[i] = i;

    vector<BVHNode> nodes;
    nodes.reserve(NUM_OBJECTS * 2);
    BuildBVHNode(nodes, objIndices, boxes, 0, NUM_OBJECTS, LEAF_MAX);

    cout << "BVH 빌드 완료\n";
    cout << "오브젝트 개수: " << NUM_OBJECTS << ", BVH 노드: " << nodes.size() << ", LeafMax = " << LEAF_MAX << "\n";

    // 컬링 테스트
    double bruteTime = 0, bvhTime = 0;
    int bruteCount = 0, bvhCount = 0;

    float aspect = 16.f / 9.f;
    float fov = XM_PIDIV4;
    float zn = 0.1f, zf = 4000.f;

    // 카메라 위치 1회만 설정
    XMVECTOR eye = XMVectorSet(600, 200, 600, 0);
    XMVECTOR at = XMVectorSet(0, 0, 0, 0);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);

    XMMATRIX view = MakeLookAtLH(eye, at, up);
    XMMATRIX proj = MakePerspectiveFovLH(fov, aspect, zn, zf);
    XMMATRIX vp = XMMatrixMultiply(view, proj);

    Frustum fr;
    fr.BuildFromMatrix(vp);

    // 브루트포스 1회
    {
        auto t0 = Clock::now();
        bruteCount = BruteForceCulling(boxes, fr);
        auto t1 = Clock::now();
        bruteTime = ms(t1 - t0).count();
    }

    // BVH 1회
    {
        auto t0 = Clock::now();
        bvhCount = BVHFrustumCulling(nodes, objIndices, boxes, fr);
        auto t1 = Clock::now();
        bvhTime = ms(t1 - t0).count();
    }

    cout << "----------------------------------------\n";
    cout << "프러스텀에서 보인 오브젝트 수\n";
    cout << "  Brute : " << bruteCount << "\n";
    cout << "  BVH   : " << bvhCount << "\n\n";

    cout << "컬링 시간\n";
    cout << "  BruteForce : " << bruteTime << " ms\n";
    cout << "  BVH        : " << bvhTime << " ms\n";
    cout << "----------------------------------------\n";
}
