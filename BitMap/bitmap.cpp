#include <iostream>
#include<vector>
#include<memory>


using namespace std;

/*
 * bitmap可以解决
 *  有一亿个整数，最大值不超过一亿，
 *      问都有那些元素重复？
 *      谁是第一个重复的？
 *      但没办法解决谁是第一个不重复的（需要扫描所有数，并记录下每个数字的出现的个数，扫描完之后取出第一个没重复的）
 *  因为bitmap无法记录出现的次数，只能判断是否重复出现。
 *
 */


const int CHAR_SIZE = 8;     //  用来存储位bit的容器：char

void print_bit(char x)
{
    for(int i=7;i>=0;--i)
    {
        cout<<((x>>i)&1);
    }
    cout<<endl;
}

int main() {

    //  MAX_NUMBER = 125 -> number of bits is 125
    vector<int> v{12,13,12,78,95,64,125,17,78};
    //  s
    const int sz = 125/CHAR_SIZE + 1;
    //  定义位图数组
    char *bitmap = new char[sz]();      //  ()值初始化
    shared_ptr<char> p(bitmap);


    for(int i=0;i<v.size();++i)
    {
        //  计算出第v[i]个bit的索引：idx + offset
        //  第几个坑位
        unsigned int idx = v[i] / CHAR_SIZE;
        //  坑位里的第几位
        unsigned int offset = v[i] % CHAR_SIZE;
//        print_bit(bitmap[idx]);
        //  该数字是否出现过
        if((bitmap[idx] & (1 << offset)) == 0)      //  如果bitmap[idx]的offset位=0
        {
            bitmap[idx] |= (1<<offset);             //  将bitmap[idx]置1
        }
        else
        {
            cout<<v[i]<<endl;                       //  bitmap[idx]的offset位已经等于1 重复出现
        }
//        print_bit(bitmap[idx]);
    }
    return 0;
}
