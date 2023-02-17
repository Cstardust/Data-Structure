#include <iostream>
#include<cassert>
#include <random>
#include <algorithm>
using namespace std;


//  缺点 : 没有实现并发

//  目前是按照 level从1开始，不过好像从0开始计数也挺好
template<typename T = int>
class SkipList {
public:
    // 构造和析构函数名不用加<T>，其它出现模板的地方都加上类型参数列表
    SkipList() :
            top_(new Head(1)) {}

    ~SkipList() {
        while (top_) {
            Node *cur = top_;       //  cur负责遍历删除本层list
            top_ = static_cast<Head *>(top_->next);      //  top_指向下一层list
            while (cur) {
                Node *ne = cur->next;
                delete cur;
                cur = ne;
            }
        }
    }

    bool find(T target) {
        Node *pre = top_;
        Node *cur = pre->next;
        while (true) {
            if (cur != nullptr) {
                if (cur->data < target) {
                    pre = cur;
                    cur = cur->next;
                    continue;
                } else if (cur->data == target) {
                    return true;
                }
            }
            //  来到这里两种可能
            //  cur == nullptr
            //  cur != nullptr && cur->data > target
            //  对此都可以以当前是否为最后一层pre->down == nullptr ? 做统一处理
            //  当前是最后一层 : pre->down == nullptr , 此时还没找到target , 则false
            if (pre->down == nullptr) {
                return false;
            }
                //  当前不是最后一层  : cur == nullptr && pre->down != nullptr , 继续向下层遍历
            else {
                pre = pre->down;
                cur = pre->next;
            }
        }

        //  never reach
        return false;
    }


    //  false : 已经存在
    //  true : 插入成功
    bool insert(T data) {
        if (find(data))
            return false;
        //  确定level : data添加到 level[1 - level]层
        int level = min({randomLevel(), top_->level + 1});       //  top_->level + 1 : 最多添加一层 .

        cout << "( level " << level << " data " << data << " )" << endl;
        //  如果要创建新层链表 : 创建新level list的head , 并更新top_
        if (level > top_->level) {
            Head *head = new Head(level);
            head->down = top_;
            top_ = head;
        }

        //  创建要插入[1-level]的new node(data).
        //  1. 先将他们竖向串成链表. 串成一串
        Node *new_nodes[level];     //  [0,level-1]
        for (int i = level - 1; i >= 0; --i) {
            new_nodes[i] = new Node(data);
            if (i == level - 1) {
                new_nodes[level - 1]->down = nullptr;
            } else {
                new_nodes[i]->down = new_nodes[i + 1];
            }
        }

        //  2.  进行横向的逐层插入
        //  找到要插入的最高层链表的头节点
        Head *head_of_top_list_to_insert = top_;
        int t = top_->level - level;
        while ((t--) > 0 && head_of_top_list_to_insert) {
            head_of_top_list_to_insert = static_cast<Head *>(head_of_top_list_to_insert->down);
        }
        //  此时head_of_top_list_to_insert 已经指向要插入的最高层链表的头节点
        //  开始从目标最高层链表 逐层插入
        Node *pre = head_of_top_list_to_insert;
        Node *cur = pre->next;
        for (int i = 0; i < level; ++i) {
//            cout<<"level "<<i+1<<" : "<<pre->data<<" "<<cur<<endl;
            //  遍历本层
            while (cur && cur->data < data) {
                pre = cur;
                cur = cur->next;
            }
            //  pre new_node cur(null)
            new_nodes[i]->next = pre->next;
            pre->next = new_nodes[i];
            //  遍历到下一层
            pre = pre->down;
            if (pre)
                cur = pre->next;
        }

        //  逐层插入结束
        assert(pre == nullptr);

        //  插入结束
    }

    void show() const {
        cout << "============show begin===========" << endl;
        Head *head = top_;
        while (head != nullptr)   //  逐层遍历
        {
            cout << "level " << head->level << " : ";
            //  遍历当前层
            Node *cur = head;
            while (cur != nullptr) {
                cout << cur->data << " ";
                cur = cur->next;
            }
            cout << endl;
            //  下一层
            head = static_cast<Head *>(head->down);
        }
        cout << "============show end===========" << endl;
    }

    //   true : 删除成功
    //   false : 删除失败(skiplist中没有该元素)
    void remove(T target)
    {
        Node *pre = top_;
        Node *cur = pre->next;
        while(true)
        {
            if(cur!= nullptr)
            {
                if(cur->data < target)
                {
                    pre = cur;
                    cur = cur->next;
                    continue;
                }
                else if(cur -> data == target)
                {
                    pre->next = cur->next;
                    delete cur;
                }
            }

            //  删除空list
            if(top_->next == nullptr)
            {
                top_ = static_cast<Head*>(top_->down);
                delete top_;
            }

            //  cur == nullptr or cur->data > target
                //  结束删除遍历
            if(pre->down == nullptr)
            {
                break;
            }
            else
            {
                pre = pre->down;
                cur = pre->next;
            }
        }

        assert(pre->down == nullptr);
    }

private:
    int randomLevel() const
    {
        int level = 1;
        while(rand() % 2)
            ++level;
        return level;
    }

    struct Node
    {
        Node(T v = T(),Node *ne = nullptr,Node *down = nullptr)
                :data(v),next(ne),down(down)
        {}
        T data;
        Node *next;
        Node *down;
    };
    //  head of each level list
    struct Head : public Node
    {
        Head(int h = -1,T v = -1,Node *ne = nullptr,Node *down = nullptr)
                :level(h),Node(v,ne,down)
        {}
        int level;
    };

    void levelHeadList(Head *t)
    {
        cout<<"level head list start"<<endl;
        while(t)
        {
            cout<<t->level<<" ";
            t = static_cast<Head*>(t->down);
        }
        cout<<endl<<"level head list over"<<endl;
    }
private:
    Head *top_;     //  head of the most top level list of the SkipList
    default_random_engine randomEngine;
};



int main() {
    std::cout << "Hello, World!" << std::endl;

    SkipList<int> skipList;

    srand(time(nullptr));

//    for(int i=0;i<45;++i)
//    {
//        skipList.insert(rand()%100);
//    }
//    skipList.show();
//    level 7 : -1 8
//    level 6 : -1 8 62
//    level 5 : -1 8 57 62 95
//    level 4 : -1 8 39 57 58 62 68 92 95 97
//    level 3 : -1 8 9 12 13 21 30 39 57 58 62 68 92 95 97
//    level 2 : -1 0 8 9 12 13 21 23 30 39 48 57 58 62 68 74 85 92 95 97
//    level 1 : -1 0 2 6 8 9 12 13 21 23 27 29 30 32 39 41 45 48 49 53 54 57 58 62 63 68 74 76 77 85 92 94 95 97

    for(int i=0;i<10;++i)
    {
        skipList.insert(rand()%20);
        skipList.show();
    }


cout<<"))))))))))))))))))))))))))))))))))))))))))))"<<endl;

    for(int i=1;i<=10;++i)
    {
        skipList.remove(i);
        skipList.show();
    }

    return 0;
}
