#include <iostream>
#include <vector>
#include "stringHash.h"

using namespace std;

//  BloomFilter实现
class BloomFilter
{
public:
    BloomFilter(int bz = 1471)
        :bitSize_(bz)
    {
        bitMap_.resize(bitSize_/INT_SIZE+1);
    }

    //  k个hash映射，这里k=3
    void insert(const string &key)
    {
        size_t pos1 = SDBMHash(key.c_str()) % bitSize_;         //  一个hash函数 -> string映射到一个数 -> 映射到一个bit
        size_t pos2 = DJB2Hash(key.c_str()) % bitSize_;
        size_t pos3 = BKDRHash(key.c_str()) % bitSize_;
//        cout<<pos1<<" "<<pos2<<" "<<pos3<<endl;
        setBit(pos1);
        setBit(pos2);
        setBit(pos3);
    }

    //  存在则返回true
    bool query(const string &key)
    {
//        int pos1 = SDBMHash(key.c_str());         //  一个hash函数将string映射到一个bit
//        int pos2 = DJB2Hash(key.c_str());
//        int pos3 = BKDRHash(key.c_str());

        return queryBit(SDBMHash(key.c_str()) % bitSize_) &&
                queryBit(DJB2Hash(key.c_str()) % bitSize_) &&
                queryBit(BKDRHash(key.c_str()) % bitSize_);
    }

private:
    //  将BloomFilter中的第pos位置1
    void setBit(size_t pos)
    {
        int idx = pos/INT_SIZE;
        int offset = pos%INT_SIZE;
        bitMap_[idx] |= (1<<offset);
    }

    //  BloomFilter的第pos位 == 1 则返回true
    bool queryBit(size_t pos)
    {
        int idx = pos/INT_SIZE;
        int offset = pos%INT_SIZE;
        return (bitMap_[idx] & (1<<offset)) != 0 ;
    }

private:
    //  BloomFilter大小，也即bitMap需要多少位。
    int bitSize_;
    //  bitMap_存储所有的位 采用int作为位的容器
    vector<int> bitMap_;
    static const int INT_SIZE = sizeof(int);
};



class BlackList
{
public:
    void add(const string &key)
    {
        bloomFilter_.insert(key);
    }
    bool query(const string &key)
    {
        return bloomFilter_.query(key);
    }
private:
    BloomFilter bloomFilter_;
};


int main() {

    BlackList blackList;
    blackList.add("https://oi-wiki.org/");
    blackList.add("https://github.com/Cstardust/Data-Structure/blob/main/BitMap/bitmap.cpp");
    blackList.add("https://www.mihoyo.com/");
    blackList.add("https://www.tencent.com/zh-cn/index.html");

    cout<<blackList.query("https://oi-wiki.org/")<<endl;
    cout<<blackList.query("https://github.com/Cstardust/Data-Structure/blob/main/BitMap/bitmap.cpp")<<endl;
    cout<<blackList.query("https://www.mihoyo.com/")<<endl;
    cout<<blackList.query("https://www.tencent.com/zh-cn/index.html")<<endl;
    cout<<blackList.query("https://www.tencent.com/zh-cn/indexhtml")<<endl;

//    BloomFilter bloomFilter;
//    cout<<"insert"<<endl;
//    bloomFilter.insert("https://oi-wiki.org/");
//    bloomFilter.insert("https://github.com/Cstardust/Data-Structure/blob/main/BitMap/bitmap.cpp");
//    bloomFilter.insert("https://www.mihoyo.com/");
//    bloomFilter.insert("https://www.tencent.com/zh-cn/index.html");
//
//    cout<<"query"<<endl;
//
//    cout<<bloomFilter.query("https://oi-wiki.org/")<<endl;
//    cout<<bloomFilter.query("https://github.com/Cstardust/Data-Structure/blob/main/BitMap/bitmap.cpp")<<endl;
//    cout<<bloomFilter.query("https://www.mihoyo.com/")<<endl;
//    cout<<bloomFilter.query("https://www.tencent.com/zh-cn/index.html")<<endl;
//    cout<<bloomFilter.query("https://www.tencent.com/zh-cn/indexhtml")<<endl;

    return 0;
}
