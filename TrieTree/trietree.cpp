#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;


class TrieTree
{
public:
    TrieTree()
    {
    }

    //  插入
    //  log(平均每个节点孩子个数) * O(m)
    void insert(const string &str)
    {
        TrieNode *cur = &root;
        int m = str.size();
        for(int i=0;i<m;++i)
        {
            char c = str[i];
            if(!cur->find(c))
            {
                TrieNode *t = new TrieNode(c);
                cur->childs_[c] = t;
                cur = t;
            }
            else
            {
                cur = cur->childs_[c];
            }
            if(i == m-1)
            {
                ++cur->cnt_;
            }
        }
    }

    //  查询TrieTree中str个数
    //  O(m)
    int query(const string &str)
    {
        TrieNode *cur = &root;
        int m = str.size();
        for(int i=0;i<m;++i)
        {
            auto c=  str[i];
            if(!cur->find(c))
                return 0;
            cur = cur->childs_[c];
        }
        return cur->cnt_;
    }

    //  TrieTree树排序功能
    //  前序打出字典树中存储的所有单词
    void preOrder()
    {
        cout<<"======= preOrder ======"<<endl;
        vector<string> res;
        string s;
        preOrder(res,s,&root);

        for(auto &item : res)
        {
            cout<<item<<endl;
        }
    }

    //  功能2：前缀搜索
    vector<string> prefixQuery(const string &prefix)
    {
        TrieNode *cur = &root;
        int m = prefix.size();
        //  先搜索前缀
        //  O(m)
        for(int i=0;i<m;++i)
        {
            //  走完所有的子节点 还没走完prefix
            if(cur == nullptr)
                return {};
            //  前缀不存在
            if(!cur->find(prefix[i]))
                return {};
            cur = cur->childs_[prefix[i]];
        }
//        cout<<"ASDSADASD  "<<cur->data_<<endl;

        vector<string> res;
        //  如果前缀也是个单词，那么加入res
        if(cur->cnt_)
            res.push_back(prefix);

        //  搜索以prefix为前缀的单词
        preOrder(res,prefix,cur);
        return res;
    }

    //  todo
    void remove(const string &str)
    {
        //  要删除的节点分为三种情况
        //  该单词和另一个单词在一条线上
            //  1.  要删除的节点后面有单词。即有子节点
                //  那么不删除该节点，只是将该节点技术置0
            //  2.  要删除的节点后面无单词。
                //  那么从自上而下 最后一个计数为1的节点开始向下删除直到删除该节点
        //  该单词和另一个单词共用一个父节点
            //  3.  不删除父节点，从该父节点向下开始删除本单词剩余部分.
    }

private:
    struct TrieNode
    {
        TrieNode(char c = '\0',int cnt = 0):data_(c),cnt_(cnt),childs_({}){}
        char data_;
        int cnt_;
        map<char,TrieNode*> childs_;

        bool find(char c){return childs_.find(c)!= childs_.end();}
    };

    //  将以r为根节点的子树的所有节点（不包括r） 若和之前的父路径组成一个字符串 则加入vec
    //  复杂度 O(平均深度) * O(平均孩子个数）
        //  看本层r节点的孩子节点的char 是否是一个字符串。是，则加入vec
        //  然后递归每个孩子
        //  为什么是看孩子节点的char不是本层地自己的？因为一开始调用传进来的时候传的是根节点空.即父节点
    void preOrder(vector<string>& vec,const string &s,TrieNode *r)
    {
        //  遍历r的孩子节点，看是否有字符串
        for(auto & item : r->childs_)
        {
            TrieNode* child = item.second;
            if(child->cnt_!=0)
            {
                vec.push_back(s + child->data_);
            }
        }

        for(auto &item : r->childs_)
        {
            auto c = item.first;
            TrieNode* child = item.second;
            preOrder(vec,s+c,child);
        }
    }
private:
    TrieNode root;
};


int main() {

    TrieTree tt;
    tt.insert("hello");
    tt.insert("hello");
    tt.insert("helloo");
    tt.insert("hel");
    tt.insert("hel");
    tt.insert("hel");
    tt.insert("china");
    tt.insert("ch");
    tt.insert("ch");
    tt.insert("heword");
    tt.insert("hellw");
    tt.insert("he");
    tt.insert("he");

    cout<<tt.query("hello")<<endl;  //  2
    cout<<tt.query("china")<<endl;  //  1
    cout<<tt.query("ch")<<endl;     //  2
    cout<<tt.query("heword")<<endl;  // 1
    cout<<tt.query("hellw")<<endl;  //  1
    cout<<tt.query("helloo")<<endl;  //  1
    cout<<tt.query("hel")<<endl;  //  3
    cout<<tt.query("he")<<endl;  //  2


    tt.preOrder();

    cout<<"============================"<<endl;

    vector<string> vec = tt.prefixQuery("hello");
    for(auto &s : vec)
    {
        cout<<s<<endl;
    }
    return 0;
}
