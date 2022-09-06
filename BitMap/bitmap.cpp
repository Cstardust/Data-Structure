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


const int MAP_SIZE = 8;     //  如果是int的话，MAP_SIZE就是 sizeof int

void print_bit(char x)
{
    for(int i=7;i>=0;--i)
    {
        cout<<((x>>i)&1);
    }
    cout<<endl;
}

int main() {

    vector<int> v{12,13,12,78,95,64,125,17,78};
    //  s
    const int sz = 125/MAP_SIZE + 1;
    //  定义位图数组
    char *bitmap = new char[sz]();      //  ()值初始化
    shared_ptr<char> p(bitmap);


    for(int i=0;i<v.size();++i)
    {
        //  map_size一定时，一对idx和offset可以唯一对应一个数据x (x=map_size*idx+offset)
        //  第几个坑位
        unsigned int idx = v[i] / MAP_SIZE;
        //  坑位里的第几位
        unsigned int offset = v[i] % MAP_SIZE;
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
