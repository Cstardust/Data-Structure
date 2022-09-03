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
 *
 *      注意把 空位分成 STATE_UNUSED和STATE_DEL两种状态
 *          防止在find和count和erase时达到O(n)
 *          根据STATE_UNUSED可以提前判定之后的hash桶中有妹有key
 */

#include<iostream>
#include<math.h>

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



class HashTable{
public:
    HashTable(int sz=0,double lf=0.75)
            : table_(nullptr),
              tableSize_(sz),
              bucketNum_(0),
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
        //  new hash表
        table_ = new Bucket[tableSize_];
    }

    bool insert(int key)
    {
        //  cmp loadFactor to expand
        double load_factor = 1.0*bucketNum_ / tableSize_;
        cout<<"key = "<<key<<" bucketNum_ = "<<bucketNum_<<" tableSize_ = "<<tableSize_<<" loadFactor = "<<load_factor<<endl;

        if(load_factor>loadFactor_){
            expand();
        }

        //  find the first
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        //  可以放多个同一元素
        //  最多轮一圈 当然不可能走到这里 never reach t == idx 因为loadFactor < 0.75 所以必定会有STATE_UNUSED
        do {
            //  找到第一个不是正在被使用的
            if(table_[idx].st == STATE_UNUSED || table_[idx].st == STATE_DEL){
                table_[idx].st = STATE_USING;
                table_[idx].key = key;
                ++bucketNum_;
                return true;            //  O(1)
            }
            idx = (idx+1)%tableSize_;
        }while(t!=idx);                 //  O(n)

        cout<<"never reach! insert internal error!"<<endl;
        return false;
    }

    bool find(int key)
    {
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        do{
            if(table_[idx].st == STATE_USING && table_[idx].key == key)
            {
                return true;
            }
            idx = (idx+1)%tableSize_;
        }while(t!=idx && table_[idx].st!=STATE_UNUSED);

        return false;
    }

    int count(int key)
    {
        int cnt = 0;
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        do{
            if(table_[idx].st == STATE_USING && table_[idx].key == key)
                ++cnt;
            idx = (idx+1)%tableSize_;
        }while(t!=idx && table_[idx].st!=STATE_UNUSED);
        //   如果之前又插入了一个key元素，那么，这个key元素一定会在占据这个STATE_UNUSED桶，而此时这个桶仍然是STATE_UNUSED，证明从该位置往后没有key
        //   究其原因，还是因为key寻址时的路线，在这些操作 :insert , count , erase 时是一致的。
        //   这也是为什么要把STATE_UNUSED 和 STATE_DEL 分开
        //   如果有一个空位是STATE_UNUSED，那么不能认为该位置之后的桶中都没有key
        //   因为这个DEL可能是后删除的，是在该key之前插入，并在已经插入完一堆该key之后删除。
        return cnt;
    }

    bool erase(int key)
    {
        int idx = (key%tableSize_ + tableSize_)%tableSize_;
        int t = idx;
        do{
            if(table_[idx].st == STATE_USING && table_[idx].key == key)
            {
                table_[idx].st = STATE_DEL;
                table_[idx].key = -1;           //  useless
                --bucketNum_;
            }
            idx = (idx+1)%tableSize_;
        }while(t!=idx && table_[idx].st!=STATE_UNUSED);
        //   如果之前又插入了一个key元素，那么，这个key元素一定会在占据这个STATE_UNUSED桶，而此时这个桶仍然是STATE_UNUSED，证明从该位置往后没有key
        //   究其原因，还是因为key寻址时的路线，在这些操作 :insert , count , erase 时是一致的。
        //   这也是为什么要把STATE_UNUSED 和 STATE_DEL 分开
        //   如果有一个空位是STATE_UNUSED，那么不能认为该位置之后的桶中都没有key
        //   因为这个DEL可能是后删除的，是在该key之前插入，并在已经插入完一堆该key之后删除。
        return true;
    }

    void show()
    {
        for(int i=0;i<tableSize_;++i){
            cout<<table_[i].key<<" ";
        }
        cout<<endl;
    }

    ~HashTable()
    {
        delete []table_;
        table_ = nullptr;
    }
private:
    void expand()
    {
        cout<<"expand"<<endl;
        if(primeIdx_ == PRIME_SIZE-1)
        {
            throw "the hash table is too big! failed to expand!";
        }

        int oldSz = tableSize_;
        tableSize_ = primes_[++primeIdx_];
        Bucket* oldTable = table_;
        table_ = new Bucket[tableSize_];
        for(int i=0;i<oldSz;++i)
        {
            //  对有效的hash桶的元素进行重新插入
            if(oldTable[i].st != STATE_USING) continue;
            int key = oldTable[i].key;
            int idx = (key%tableSize_ + tableSize_)%tableSize_;
            int t = idx;

            //  重新插入新的哈希表 注意别重新计算bucketNum_
            do {
                if(table_[idx].st == STATE_UNUSED){
                    table_[idx].st = STATE_USING;
                    table_[idx].key = key;
                    break;
                }
                idx = (idx+1)%tableSize_;
            }while(idx!=t);

        }
        delete []oldTable;
    }

    Bucket* table_;
    int tableSize_;
    int bucketNum_;
    double loadFactor_ = 0.75;
    static const int PRIME_SIZE = 10;
    int primeIdx_;
    static int primes_[PRIME_SIZE];
};


int HashTable::primes_[PRIME_SIZE] = { 3, 7, 23, 47, 97, 251, 443, 911, 1471, 42773 };

int main()
{
    HashTable hashTable;
    hashTable.insert(1);
    hashTable.insert(2);
    hashTable.insert(2);
    hashTable.insert(4);
    hashTable.insert(7);

    hashTable.show();

    hashTable.erase(1);
    hashTable.erase(2);
    hashTable.erase(3);
    hashTable.show();
    hashTable.insert(88);
    hashTable.show();

//    cout<<hashTable.find(4)<<endl;
//    cout<<hashTable.find(-1)<<endl;
//    cout<<hashTable.find(6)<<endl;
//    cout<<hashTable.find(88)<<endl;
//

    hashTable.insert(3);
    hashTable.insert(3);
    hashTable.show();
    hashTable.insert(3);
    hashTable.insert(3);
    hashTable.show();
    cout<<hashTable.count(3)<<endl;

    hashTable.erase(7);
    cout<<hashTable.count(3)<<endl;
}

/*
 * key = 1 bucketNum_ = 0 tableSize_ = 3 loadFactor = 0
key = 2 bucketNum_ = 1 tableSize_ = 3 loadFactor = 0.333333
key = 2 bucketNum_ = 2 tableSize_ = 3 loadFactor = 0.666667
key = 4 bucketNum_ = 3 tableSize_ = 3 loadFactor = 1
expand
key = 7 bucketNum_ = 4 tableSize_ = 7 loadFactor = 0.571429
7 1 2 2 4 0 0
7 -1 -1 -1 4 0 0
key = 88 bucketNum_ = 2 tableSize_ = 7 loadFactor = 0.285714
7 -1 -1 -1 4 88 0
key = 3 bucketNum_ = 3 tableSize_ = 7 loadFactor = 0.428571
key = 3 bucketNum_ = 4 tableSize_ = 7 loadFactor = 0.571429
7 -1 -1 3 4 88 3
key = 3 bucketNum_ = 5 tableSize_ = 7 loadFactor = 0.714286
key = 3 bucketNum_ = 6 tableSize_ = 7 loadFactor = 0.857143
expand
0 0 0 3 3 4 3 7 3 0 0 0 0 0 0 0 0 0 0 88 0 0 0
4
4
 */

#endif