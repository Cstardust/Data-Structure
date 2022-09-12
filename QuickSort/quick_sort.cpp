//  快排
//  时间复杂度 n logn
//  每层:O(n) 共logn层

#include<iostream>

using namespace std;

const int maxn = 100005;
int h[maxn];

//  当前层结束后
//   j和i一定会错开。l,j,i(j+1),r(且最多错开1位，因为i遇到>=x的就停止，j遇到<=x就停止，而错开后i会走j走过的路，j会走i走过的路)
//  且一定有[l,i] <= x,[j,r] >= x
//  故一个大区间可以分为两个小区间 [l,j]<=x [j+1,r]>=x
//  故可分为
void quick_sort(int l,int r)
{
    if(l>=r) return ;
    
    int x = h[(l+r)/2];
    int i = l-1;
    int j = r+1;
    while(i<j)
    {
        do i++; while(i<=r && h[i]<x);  //  i遇到>=x的就停止
        do j--; while(j>=0 && h[j]>x);
        if(i<j)
        {
            swap(h[i],h[j]);
        }
    }
    
    quick_sort(l,j);
    quick_sort(j+1,r);
}

int main()
{
    int n;
    cin>>n;
    for(int i=0;i<n;++i)
    {
        cin>>h[i];
    }
    
    quick_sort(0,n-1);
    
    for(int i=0;i<n;++i)
    {
        cout<<h[i]<<" ";
    }
    
}


/////////////////////


#include<iostream>
#include<cstdio>

using namespace std;

const int maxn=10000010;
int q[maxn];


//于归并排序的划分点不同在于：
//quick的划分点是数组中间位置存的数的值
//merge的划分点是数组的中间位置
//quick是先排好每次大区间 在向下递归排下一次的小区间
//merge是先递归至最底层 先拍好最小的 再向上返回
/*******************
quicksort步骤
1.传入数组 区间左边界 右边界
【2.end.基情框判断：if（l>=r）return；（最小区间）】
2.取标准数（又叫划分中点）为l与r的中间值（不是边界就行）
 取左右指针 i j i指向区间左边界的左侧 j指向区间右边界的右侧
3.while循环i与j还未相遇 进行一轮排序 i向右移动 找大于等于x的数，i停下；j向左移动 找小于等于x的数，j停下；
如果i与j还未相遇 则交换i与j指向的值
此时左区间全部小于x 右区间全部大于x
4.递归调用函数对左右区间接着排序
quick——sort（q,l,j);left
quick_sort(1,j+1,r);right;
没了

************************/



//每次传入数组 以及 ；左边界 右边界（边界是最边上有数的一点 （包含该数））
void quick_sort(int q[],int l,int r)
{
    if(l>=r) return;

    int x=q[(l+r)/2];//划分中点（标准数）//取端点会时间太长
    int i=l-1;//i与j指向边界两侧
    int j=r+1;

    //while一轮排序（交换）//使i与j相遇时（错开) 左边的数都小于等于x 右边的数都大于等于x
    while(i<j){
        //在左区间里找不符合的数 （ia要求左区间里的数都比x小）（所以遇到小的数及符合标准 继续向下移动 看下一位是否比标准数大 若比其大 则停下 记录该位置 稍后进行交换ia
        //这两步的作用是找到并记住不符合规范的数的位置
        do i++; while(q[i]<x);//没有等号！！遇到与标准数相同的数也会立即停下！！
        do j--; while(q[j]>x);//注意j是-- 不是++ 从右向左移动 sb了
        if(i<j){
            swap(q[i],q[j]);
        }
    }

    quick_sort(q,l,j);//！！注意左区间1到j
    quick_sort(q,j+1,r);//右区间j+1到r

}



int main()
{
    int n;
    cin>>n;
    for(int i=0;i<n;i++){
        scanf("%d",&q[i]);
    }

    quick_sort(q,0,n-1);

    for(int i=0;i<n;i++){
        printf("%d ",q[i]);
    }
    return 0;
}
