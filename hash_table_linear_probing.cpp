//
// Created by dell on 2022-09-03.
//


# if 0
/*
 * Hash Table with Linear Probing
 * 解决冲突方法为 线性探测法 的 hash表
 *      类似于unordered_set : 元素无序存放，且不允许重复插入.
 *      插入、搜索、删除时间复杂度为O(1)
 *      loadFactor = 0.75
 *      bucketNum = STATE_USING + STATE_DEL（因为已经删除元素的桶不可以再放元素，因此应当算入bucketNum）
 *      只要当扩容的时候，才能将STATE_DEL从bucketNum中去除。
 *      缺点：已经删除的元素的桶不能再放入元素。待改进。
 *          改进措施：第一轮寻址不可将元素放入STATE_DEL的hash桶，寻址一圈之后，可以放入状态为STATE_DEL的hash桶。是否会对插入策略造成影响还未证明。我认为不会 没写是因为太麻烦
 * Interface
 *      1. insert
 *      2. find
 *      3. erase
 * Private Interface:
 *      1. expand (when loadFactor > 0.75)
 */


#include<iostream>

using namespace std;

//  状态
enum State{
    STATE_UNUSED,   //  从没被用过
    STATE_USING,    //  正在被使用
    STATE_DEL       //  已经被删除
};

//  hash桶
struct Bucket{
    Bucket(int x=0,State s=STATE_UNUSED)
        :key(x),st(s){}
    int key;
    State st;
};

// unordered_set
class HashTable{
public:
    HashTable(int sz = 0,double lf = 0.75)
        :table_(nullptr),loadFactor_(lf),bucketNum_(0),tableSize_(0),primeIdx_(0)
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
        //  new hash表
        table_ = new Bucket[tableSize_];
    }

    bool insert(int key)
    {
        cout<<"key "<<key<<" "<<bucketNum_<<" "<<tableSize_<<endl;
        //  cmp loadFactor whether expand
        cout<<"loadfactor = "<< (1.0*bucketNum_/tableSize_)<<endl;
        if(1.0*bucketNum_/tableSize_ >= loadFactor_){
            if(!expand())
                return false;
        }

        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        //  有元素&&元素不是x  / 空着的但是之前删除的
        while((table_[idx].st==STATE_USING&&table_[idx].key!=key)||(table_[idx].st==STATE_DEL))
        {
            ++idx;
            idx = idx%tableSize_;
            //  限制：最多只能走一圈
            if(idx == t){
                cout<<"never reach! insert internal error!"<<endl;
                break;
            }
        }

        //  可能state = STATE_UNUSED
        //  也可能state = STATE_USING && val = x;
        if(table_[idx].st==STATE_USING&&table_[idx].key==key)           //  找到
        {
            cout<<"the key "<<key<<" has already been inserted "<<endl;
            return true;
        }
        else if(table_[idx].st==STATE_UNUSED)                           //  之前没有
        {
            ++bucketNum_;
            table_[idx].st = STATE_USING;
            table_[idx].key = key;
            return true;
        }

        cout<<"never reach! insert internal error!"<<endl;
        return false;
    }

    bool find(int key)
    {
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        //  有元素&&元素不是x  / 空着的但是之前删除的
        while((table_[idx].st==STATE_USING&&table_[idx].key!=key)||(table_[idx].st==STATE_DEL))
        {
            ++idx;
            idx = idx%tableSize_;
            //  限制：最多只能走一圈
            if(idx == t)
                break;
        }
        //  可能state = STATE_UNUSED
        //  也可能state = STATE_USING && val = x;  直这就是找到了
        if(table_[idx].st==STATE_USING&&table_[idx].key==key)
        {
            return true;
        }
        return false;
    }

    bool erase(int key)
    {
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        while((table_[idx].st==STATE_USING&&table_[idx].key!=key)||(table_[idx].st==STATE_DEL))
        {
            ++idx;
            idx = idx%tableSize_;
            //  限制：最多只能走一圈
            if(idx == t){
                cout<<"debug"<<endl;
                break;
            }
        }
        //  找到
        if(table_[idx].st==STATE_USING&&table_[idx].key==key){
            table_[idx].st = STATE_DEL;
            table_[idx].key = 0;        //  没用这句话
            return true;
        }
        cout<<key <<" not found in hash table"<<endl;
        return false;
    }

    void show()
    {
        for(int i=0;i<tableSize_;++i)
        {
            cout<<table_[i].key<<" ";
        }
        cout<<endl;
    }

    ~HashTable()
    {
        delete[] table_;
        table_ = nullptr;
    }

private:

    bool expand()
    {
        cout<<"expand"<<endl;
        if (primeIdx_ == PRIME_SIZE)
        {
            throw "HashTable is too large, can not expand anymore!";
        }

        int newSz = primes_[primeIdx_+1];
        int oldSz = primes_[primeIdx_];
        primeIdx_+=1;
        tableSize_ = newSz;

        Bucket *new_table = new Bucket[newSz];
        Bucket *old_table = table_;
        table_ = new_table;

        bucketNum_ = 0;
        //  将old_table中的元素重新插入new_table
        for(int i=0;i<oldSz;++i) //  insert(old_table[i].key); 不能重复调用 因为insert里面会修改bucketNum
        {
            //  STATUS_DEL元素 不插入
            if(old_table[i].st!=STATE_USING) continue;

            //  new_table的地方一定够
            int key = old_table[i].key;
            int idx = (key%tableSize_ + tableSize_)%tableSize_;
            int t = idx;
            //  有元素&&元素不是x  / 空着的但是之前删除的
            while((table_[idx].st==STATE_USING&&table_[idx].key!=key)||(table_[idx].st==STATE_DEL))
            {
                ++idx;
                idx = idx%tableSize_;
                //  限制：最多只能走一圈  其实根本不会出现 因为loadfactor导致了一定有空位
                if(idx == t){
                    cout<<"never reach! expand internal error!"<<endl;
                    break;
                }
            }

            if(table_[idx].st==STATE_UNUSED)                           //  之前没有
            {
                table_[idx].st = STATE_USING;
                table_[idx].key = key;
                ++bucketNum_;
            }
            else
            {
                cout<<"never reach! expand internal error!"<<endl;
                return false;
            }
        }

        delete[] old_table;
    }

    Bucket* table_;         //  hash表
    int tableSize_;         //  hash表大小
    int bucketNum_;         //  已经使用的bucket
    double loadFactor_;     //  装载因子
    static int primes_[10]; //  素数表
    static const int PRIME_SIZE = 10;
    int primeIdx_;
};

int HashTable::primes_[10]={3,7,23,47,97,251,443, 911,1471, 42773};

int main()
{
    HashTable hashTable;
    hashTable.insert(1);

    hashTable.insert(2);
    hashTable.insert(5);


    hashTable.insert(6);
    hashTable.show();

    hashTable.erase(6);

    hashTable.insert(17);
    hashTable.show();
    hashTable.erase(1);
    hashTable.erase(2);
    hashTable.show();

    hashTable.insert(7);
    hashTable.show();

    hashTable.insert(9);
    hashTable.show();

    hashTable.insert(1);
    hashTable.show();
}

#else if

/*
 * Hash Table with Linear Probing
 * 解决冲突方法为 线性探测法 的 hash表
 *      类似于unordered_multiset : 元素无序存放，且允许重复插入.
 *      插入、搜索、删除时间复杂度为O(1)
 *      loadFactor = 0.75
 *      bucketNum = STATE_USING
 * Interface
 *      1. insert
 *      2. find
 *      3. erase
 * Private Interface:
 *      1. expand (when loadFactor > 0.75)
 */

#include <iostream>
using namespace std;

// 桶的状态
enum State
{
    STATE_UNUSE, // 从未使用过的桶
    STATE_USING, // 正在使用的桶
    STATE_DEL,   // 元素被删除了的桶
};

// 桶的类型
struct Bucket
{
    Bucket(int k = 0, State s = STATE_UNUSE)
            : key(k)
            , st(s)
    {}

    int key;      // 存储的数据
    State st;  // 桶的当前状态
};

// 线性探测哈希表类型
class HashTable
{
public:
    HashTable(int sz = primes_[0], double loadFactor = 0.75)
            : useBucketNum_(0)
            , loadFactor_(loadFactor)
            , primeIdx_(0)
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
        //  new hash表
        table_ = new Bucket[tableSize_];
    }

    ~HashTable()
    {
        delete[]table_;
        table_ = nullptr;
    }

public:
    // 插入元素
    bool insert(int key)
    {
        // 考虑扩容
        double factor = useBucketNum_*1.0 / tableSize_;
        cout << "loadFactor:" << factor << endl;
        if (factor > loadFactor_)
        {
            // 哈希表开始扩容
            expand();
        }

        int idx = key % tableSize_;

        int i = idx;
        //  因为是multiset，所以不管之前存不存在，都可以放
        do
        {
            if (table_[i].st != STATE_USING)
            {
                table_[i].st = STATE_USING;
                table_[i].key = key;
                ++useBucketNum_;
                return true; // O(1)
            }
            i = (i + 1) % tableSize_;
        } while (i != idx); // O(n)

        return false;
    }

    // 删除元素
        //  全部key都要删除
    bool erase(int key)
    {
        int idx = key % tableSize_;

        int i = idx;
        do
        {
            if (table_[i].st == STATE_USING && table_[i].key == key)
            {
                table_[i].st = STATE_DEL;
                useBucketNum_--;
            }
            i = (i + 1) % tableSize_;
        } while (table_[i].st != STATE_UNUSE && i != idx);

        return true;
    }

    // 查询  count(key)
    bool find(int key)
    {
        int idx = key % tableSize_;

        int i = idx;
        do
        {
            if (table_[i].st == STATE_USING && table_[i].key == key)
            {
                return true;
            }
            i = (i + 1) % tableSize_;
        } while (table_[i].st != STATE_UNUSE && i != idx);

        return false;
    }

    void show()
    {
        for(int i=0;i<tableSize_;++i)
        {
            cout<<table_[i].key<<" ";
        }
        cout<<endl;
    }

private:
    // 扩容操作
    void expand()
    {
        ++primeIdx_;
        if (primeIdx_ == PRIME_SIZE)
        {
            throw "HashTable is too large, can not expand anymore!";
        }

        Bucket* newTable = new Bucket[primes_[primeIdx_]];
        for (int i = 0; i < tableSize_; i++)
        {
            if (table_[i].st == STATE_USING) // 旧表有效的数据，重新哈希放到扩容后的新表
            {
                int idx = table_[i].key % primes_[primeIdx_];

                int k = idx;
                do
                {
                    if (newTable[k].st != STATE_USING)
                    {
                        newTable[k].st = STATE_USING;
                        newTable[k].key = table_[i].key;
                        break;
                    }
                    k = (k + 1) % primes_[primeIdx_];
                } while (k != idx);
            }
        }

        delete[]table_;
        table_ = newTable;
        tableSize_ = primes_[primeIdx_];
    }

private:
    Bucket* table_;    // 指向动态开辟的哈希表
    int tableSize_;    // 哈希表当前的长度
    int useBucketNum_; // 已经使用的桶的个数
    double loadFactor_;// 哈希表的装载因子

    static const int PRIME_SIZE = 10; // 素数表的大小
    static int primes_[PRIME_SIZE];   // 素数表
    int primeIdx_; // 当前使用的素数下标
};

int HashTable::primes_[PRIME_SIZE] = { 3, 7, 23, 47, 97, 251, 443, 911, 1471, 42773 };

int main()
{
    HashTable hash_table;
    hash_table.insert(21);
    hash_table.insert(32);
    hash_table.insert(14);
    hash_table.insert(15);

    hash_table.insert(22);

    cout << hash_table.find(21) << endl;
    hash_table.erase(21);
    cout << hash_table.find(21) << endl;

    hash_table.show();
}


#endif