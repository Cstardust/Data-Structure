/*
 * Top K 问题
 *  Heap(priority_queue)的应用
 *  可与查重(HashTable的应用(unordered_set or unordered_map))结合
 *      C++的堆
 *          priority_queue<int,vector<int>,less<int>> heap;
 *          不支持迭代器
 *          不支持随机访问[]
 *          那如何遍历？
 *              只能访问堆顶元素 heap.top();
 *              压入堆：    heap.push(x)
 *              弹出堆顶元素 heap.pop();
 *              边弹出边访问
 */

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <functional>

using namespace std;

//  查重 + Top K
//  HashTable + Heap
int main()
{
    const int N = 100000;
    vector<int> vec;
    srand(time(nullptr));
    for (int i = 0; i < 100000; ++i)
        vec.push_back(rand() % 1000);
    int k = 5;
    //  查询重复出现次数最多的k个元素
    //  记录重复次数
    unordered_map<int, int> m;
    for (int i = 0; i < N; ++i)
    {
        ++m[vec[i]];
    }
    //  小根堆（priority queue）里面存的是一个个pair<int,int>
    //  比较大小的方式是我们自定义的。通过lambda表达式。比较pair的second大小。
    using MyType = pair<int, int>;
    priority_queue<MyType, vector<MyType>, function<bool(MyType &, MyType &)>> minHeap(
        [](MyType &a, MyType &b) -> bool
        {
            return a.second < b.second;
        });

    for (const auto &item : m)
    {
        if (minHeap.size() < k)
        {
            minHeap.push(item);
        }
        else
        {
            if (minHeap.top().second < item.second)
            {
                minHeap.pop();
                minHeap.push(item);
            }
        }
    }

    while (!minHeap.empty())
    {
        const auto &item = minHeap.top();
        cout << item.first << " " << item.second << endl;
        minHeap.pop();
    }
    //    priority_queue<int,vector<int>,less<int>> p;
    //    explicit
    //    priority_queue(const _Compare& __x, _Sequence&& __s = _Sequence())
    //    : c(std::move(__s)), comp(__x)
    //    { std::make_heap(c.begin(), c.end(), comp); }

    //  ERROR
    //  想想map也不能用sort。map底层是个hashtable。hashtable虽说是个数组，数组支持随机访问，但key映射到的位置是不允许被改变的啊。不然不是白hash了。
    //    sort(m.begin(),m.end(),
    //         [](pair<int,int> &a,pair<int,int>& b)->bool{return a.second>b.second;}
    //         ); ERROR!
    return 0;
}

#ifdef TOP_K_1
int main()
{
    //    std::cout << "Hello, World!" << std::endl;

    vector<int> vec;
    srand(time(nullptr));
    for (int i = 0; i < 1000; ++i)
        vec.push_back(rand() % 1000);
    int k = 10;
#ifdef TopMax
    //  求 top k小的数
    //  小根堆维护k个最小的数字的集合
    priority_queue<int, vector<int>, greater<int>> maxHeap;
    for (int i = 0; i < 1000; ++i)
    {
        if (maxHeap.size() < k)
            maxHeap.push(vec[i]);
        else
        {
            if (maxHeap.top() > vec[i])
            {
                maxHeap.pop();
                maxHeap.push(vec[i]);
            }
        }
    }

    while (maxHeap.size())
    {
        cout << maxHeap.top() << endl;
        maxHeap.pop();
    }
#endif
#ifdef TopMin
    //  求 top k大的数
    //  小根堆维护k个最大的数字的集合
    priority_queue<int, vector<int>, less<int>> minHeap;
    for (int i = 0; i < 1000; ++i)
    {
        if (minHeap.size() < k)
        {
            minHeap.push(vec[i]);
        }
        else
        {
            if (vec[i] > minHeap.top())
            {
                minHeap.pop();
                minHeap.push(vec[i]);
            }
        }
    }

    while (minHeap.size())
    {
        cout << minHeap.top() << endl;
        minHeap.pop();
    }
#endif

    return 0;
}

#endif

////////////////////////////////////
/*
 * 寻找第TopK小的数字
 * 快速选择 时间复杂度O(n)
 * 快速排序 + 只在答案落在的区间内进行快排
 *  n + n/2 + n/4 + ...  n/2^n= n(1+1/2+...) = O(n)
 *
 * 最后 h[k-1]是第k小的数字
 * 且结束选择之后,从h[0]到h[k-1] 是前k小的数字（但是无序）
 *
 */

#include <iostream>

using namespace std;
const int maxn = 100010;
int q[maxn];
int n, m;

void quick_sort(int l, int r)
{
    if (l >= r)
        return;
    int i = l - 1;
    int j = r + 1;
    int x = q[(l + r) / 2];
    while (i < j)
    {
        do
            i++;
        while (q[i] < x);
        do
            j--;
        while (q[j] > x);
        if (i < j)
            swap(q[i], q[j]);
    }
    //只对答案落在的区间进行分治，进行排序
    if (m - 1 <= j)
        quick_sort(l, j);
    else
        quick_sort(j + 1, r);
    return;
}

int main()
{
    cin >> n >> m;
    for (int i = 0; i < n; i++)
        cin >> q[i];
    quick_sort(0, n - 1);
    cout << q[m - 1] << endl;
}
