//
// Created by dell on 2022-09-04.
//


/*
 * Chained Hash Table
 *  解决寻址冲突：通过在hash桶上挂链表的方式
 *
 * 如下实现HashTable
 *      类似于 unordered_set , 无序、不允许插入重复元素
 * Interface
 *      find        趋近于O(1)
 *      insert      趋近于O(1)
 *      erase       趋近于O(1)
 *   private:
 *      expand      O(n)
 *  hash函数还是除留余数法
 *
 *  Tips：
 *      1. resize vs reserve
 *      2. vector swap allocator相同时，快速交换（通过交换成员变量）
 *  PS : 没有实现分段锁，不过也好实现，就是每个hash桶一个锁，开个数组就行。
 */



/*
 * 为什么链式？
    cpp和java的无序关联容器用的是链式哈希表，而不是线性探测法。为什么？
        因为线性探测哈希表的缺陷：
          发生hash冲突时，寻址是趋近于O(n)的复杂度，存储变慢了。
                能否优化？没办法，线性探测方法里就有一个数组，没办法优化。
        多线程环境中
            多线程不应当同时对一个数组进行增删查。
            线性探测所用到的基于数组实现的哈希表，只能给全局的哈希表用互斥锁来保证哈希表的原子操作，保证线程安全！并发效果不好
            分段的锁！：多线程环境解决方法。既保证了线程安全，又有一定并发量，提高了效率！。
            但是cpp库里的无序关联容器实现并没有采用分段的锁，也即并没有实现线程安全，但不妨碍当我们想实现一个可以用在多线程环境下的容器时，通过分段锁欸到方式实现。
    如果其他桶都没链表，但是有一个桶的链表特别长？那是hash函数的问题。
    对于线性探测法的两个缺点的优化
        优化一：当链表长度>x，把桶里面的这个链表转化成红黑树(搜索复杂度O(logn))
        优化二：连是哈系表的每个桶都可以创建自己的互斥锁，不同桶中的链表操作，可以并发执行。
 */



#include<iostream>
#include<vector>
#include<list>
#include<algorithm>

using namespace std;


//  实现unordered_set 无序、且不可重复插入元素
class HashTable{
public:
    HashTable(int sz=0,double lf=0.75)
        :bucketNum_(0),
        tableSize_(sz),
        loadFactor_(lf),
        primeIdx_(0)
    {
        //  确定大小 tableSize_
        for(primeIdx_ =0;primeIdx_ < PRIME_SIZE;++primeIdx_)
        {
            int &i = primeIdx_;
            if(sz<primes_[i]){
                sz = primes_[i];
                break;
            }
        }
        tableSize_  = min(sz,primes_[PRIME_SIZE-1]);       //  最大也只能是素数表里的最大

        //  生成table
        table_.resize(tableSize_);                         //  将table_的长度调整到tableSize_，并且每个元素都是一个默认初始化的
    }

    //  O(1) -- O(n) （瓶颈在find上）
        //  可能O(n)的原因是搜索的链表过长，
        //  如果链表过长
            //  单个链表过长（散列结果集中）：hash函数设计的不好，这些key都映射到了一个hash桶上
            //  如果散列结果分散（分散的话就可以影响loadFactor，进而进行扩容防止过长）：那么链表长度一般不会过长，因为有loadFactor。使得在适当的时候进行扩容，不会使得key一直插入这些桶挂载的链表，使得链表过长。
    bool insert(int key)
    {
        double factor = 1.0*bucketNum_/tableSize_;
        cout<<key<<" "<<bucketNum_<<" "<<tableSize_<<" "<<factor<<endl;
        if(factor >= loadFactor_){
            expand();
        }

        int idx = (key%tableSize_ + tableSize_)%tableSize_;     //  链表挂在哪个hash桶上

        list<int>& it = table_[idx];
        if(it.empty())  ++bucketNum_;                           //  只有在用了一个新的hash桶链表时，才会有一个++Num;

        list<int>::iterator iter = ::find(it.begin(),it.end(),key); //  找key    ::find 使用全局find 而非类内的find
        if(iter!=it.end()){
            cout<<"the key "<<key<<" is already in"<<endl;
            return false;
        }
        it.emplace_front(key);
        return true;
    }

    //  O(1) -- O(n)
    bool find(int key)
    {
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        list<int>& it = table_[idx];
        list<int>::iterator iter = ::find(it.begin(),it.end(),key);
        return iter!=it.end();
    }

    //  O(1) -- O(n)
    bool erase(int key)
    {
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        list<int>& it = table_[idx];
        list<int>::iterator iter = ::find(it.begin(),it.end(),key);
        if(iter==it.end())
        {
            cout<<"the key "<<key<<" not found!"<<endl;
            return false;
        }
        it.erase(iter);     // O(1)
//        it.remove(key);       O(n)
        if(it.empty())              //  hash桶空了
            --bucketNum_;

        return true;
    }

    void show()
    {
        for(int i=0;i<tableSize_;++i)
        {
            list<int> &it = table_[i];
            cout<<"hash list "<<i<<endl;
            if(it.empty()) continue;
            for_each(it.begin(),it.end(),[](int x){cout<<x<<" ";});
            cout<<endl;
        }
    }

private:
    //  O(n)
    void expand()
    {
        cout<<"===============expand==============="<<endl;

        //  只交换成员变量，因此很快。不是拷贝所有元素进行交换！
        vector<list<int>> oldTable;
        table_.swap(oldTable);          //  allocator相同，交换成员变量

        if(primeIdx_ == PRIME_SIZE-1)
        {
            throw "the hash table is too big! failed to expand!";
        }

        int oldSz = tableSize_;
        tableSize_ = primes_[++primeIdx_];
        table_.resize(tableSize_);

        bucketNum_ = 0;         //  重置bucketNum_ 因为这和线性探测法不同。线性探测法的hash桶的个数不会变，因为没有别的数据结构，只有一个数组。而链式hash，由于处理冲突时会插入链表，因此bucketNum的个数可能改变
        for(const list<int>& it:oldTable)
        {
            for(int x:it)
            {
                if(!insert(x)){
                    cout <<"never reach ! expand internal error!"<<endl;
                }
            }
        }

    }

private:
    vector<list<int>> table_;
    int bucketNum_;                     //  使用的hash桶的个数，不是使用的节点的个数
    int tableSize_;
    double loadFactor_;

    static const int PRIME_SIZE = 10;
    static int primes_[PRIME_SIZE];
    int primeIdx_;
};

int HashTable::primes_[PRIME_SIZE] = { 3, 7, 23, 47, 97, 251, 443, 911, 1471, 42773 };


int main()
{
    HashTable hashTable;

    //  check insert
    hashTable.insert(1);
    hashTable.insert(2);
    hashTable.insert(4);
    hashTable.show();

    //  check expand
    hashTable.insert(5);
    hashTable.insert(0);
    hashTable.insert(7);
    hashTable.show();

    //  check find
    cout<<hashTable.find(1)<<endl;
    hashTable.erase(1);
    cout<<hashTable.find(1)<<endl;

    cout<<hashTable.erase(9)<<endl;
    hashTable.insert(9);
    cout<<hashTable.find(9)<<endl;
    cout<<"========================================"<<endl;
    hashTable.show();
}


/*
 * vector resize
 *  调整容器的大小，使其包含n个元素。
 *   如果n小于当前容器的大小，则将内容减少到其前n个元素，并删除超出范围的元素（并销毁它们）。
 *   如果n大于当前容器的大小，则通过在末尾插入所需数量的元素来扩展内容，以达到n的大小。如果指定了val，则将新元素初始化为val的副本，否则将对它们进行值初始化。
 *   如果n也大于当前容器容量，将自动重新分配已分配的存储空间。
 *   请注意，此函数通过插入或擦除容器中的元素来更改容器的实际内容。
 *   reserve
 *  只调整容器大小(申请空间)，并不填充元素
 *       Request a change in capacity
 *       Requests that the vector capacity be at least enough to contain n elements.
 *       If n is greater than the current vector capacity, the function causes the container to reallocate its storage increasing its capacity to n (or greater).
 *       In all other cases, the function call does not cause a reallocation and the vector capacity is not affected.
 v       This function has no effect on the vector size and cannot alter its elements.
 */

/*
1 0 3 0
2 1 3 0.333333
4 2 3 0.666667
hash list 0
hash list 1
4 1
hash list 2
2
5 2 3 0.666667
0 2 3 0.666667
7 3 3 1
===============expand===============
0 0 7 0
4 1 7 0.142857
1 2 7 0.285714
5 3 7 0.428571
2 4 7 0.571429
hash list 0
7 0
hash list 1
1
hash list 2
2
hash list 3
hash list 4
4
hash list 5
5
hash list 6
1
0
the key 9 not found!
0
9 4 7 0.571429
1
========================================
hash list 0
7 0
hash list 1
hash list 2
9 2
hash list 3
hash list 4
4
hash list 5
5
hash list 6
 */