

//  堆
//  关注注释3点：1、2、3
//  保证每个节点都满足：左右儿子都小于其父节点 的完全二叉树
//  完全二叉树定义
    // 完全二叉树是一种特殊的二叉树，满足以下要求：
    // 所有叶子节点都出现在 k 或者 k-1 层，而且从 1 到 k-1 层必须达到最大节点数；
    // 第 k 层可以不是满的，但是第 k 层的所有节点必须集中在最左边。 
    // 需要注意的是不要把完全二叉树和“满二叉树”搞混了，完全二叉树不要求所有树都有左右子树，但它要求：
    // 任何一个节点不能只有右子树没有左子树
    // 叶子节点出现在最后一层或者倒数第二层，不能再往上


//  如何手写堆
    //  实现的
    //  1.插入一个数 heap[++sz] = x; up(sz);
    //  2.求集合中最小值 heap[1];
    //  3.删除最小值    heap[1] = heap[sz]; --sz; down(1);
    //  没实现的
    //  4.删除任意元素  heap[k] = heap[sz]; --sz; down(k);
    //  5.heap[k]=x;    down(k);    up(k);



#include<iostream>
#include <iostream>
#include <cstring>
#include <algorithm>
#include<math.h>

using namespace std;


const int maxn = 100010;

int h[maxn];
int sz = 0;

//  2. down函数写法
//  down完之后，保证以u为根节点的子树是小根堆
//  将大数down下去。大根堆也可以将小数down下去
void down(int u)
{
    //  将u节点 和其最小的儿子交换位置
    int t = u;
    int left = u*2;
    int right = u*2+1;
    
    if(sz >= left && h[u] > h[left]) u = left;
    if(sz >= right && h[u] > h[right]) u = right;
    
    if(u==t) return ;       //  u == t 意味着以u为根的树已经是一个小根堆了。没必要再向下down
    
    swap(h[u],h[t]);        //  交换
    down(u);                //  down 大数交换到了子结点上 有可能破坏该儿子原先的小根堆性质,所以应接着往下沉 所以递归
}

//  将小数up上去，大根堆也可以将大数up上去
void up(int u)
{
    int father = u / 2;
    if(father > 0 && h[father] > h[u])
    {
        swap(h[u],h[father]);
        up(father);
    }
}

int main()
{
    int n,m;
    cin>>n>>m;
    sz = n;
    for(int i=1;i<=n;++i)
    {
        cin>>h[i];        
    }
    
    
    // 1. 构建堆：从n/2开始down，
    //  因为n是最大值，n/2是n的父节点，
    //  所以n/2是最大的有儿子节点的父节点
    //  所以从n/2向前遍历，可以遍历到1-n/2位置的所有数字（也即，从倒数第二层到低一层的所有节点），且，down也可以将节点down到最底层
    //  所以，从n/2开始down，可以比较树中的所有节点。
        //  最根本的原因还是，从倒数第二层开始down（down遍倒数第二层的每个节点），可以保证这个还未形成堆的二叉树的最底层的每个子树，都是一个小根堆
        //  如果不从倒数第二层开始down，则无法保证正确性
    //  <n/2开始down，影响正确性
    //  >n/2开始down，没必要
    for(int i=n/2;i>=1;--i)
    {
        down(i);
    }
    
    while (m -- )
    {
        //  output
        cout<<h[1]<<" ";
        //  3. 删除根节点（最小的节点）
        h[1] = h[sz];   
        --sz;
        down(1);
    }
}