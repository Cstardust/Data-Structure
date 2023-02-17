//
// Created by dell on 2022-10-30.
//
#include<iostream>
#include<vector>
#include<algorithm>
#include<queue>
using namespace std;

//  AVL 树 ： 绝对的平衡树
    //  增 删 查 的复杂度都为 O(logn)
    //  当节点失衡时做出的平衡操作（左旋、右旋、左平衡、右平衡）均为 O(1)
#include <ntdef.h>
#include<cassert>
template<typename T>
class BalanceSearchTree
{
public:
    struct Node
    {
        Node(T val = 0,Node *l = nullptr,Node *r = nullptr,int h = 0)
            :data(val),
            left(l),
            right(r),
            height(0)
        {}
        T data;
        Node *left;
        Node *right;
        int height;
    };
public:
    BalanceSearchTree(Node *root = nullptr):root_(root){}
    ~BalanceSearchTree()
    {
        if(root_ == nullptr) return ;
        //  bfs delete
        queue<Node*> q;
        q.push(root_);
        while(!q.empty())
        {
            Node *t = q.front();
            q.pop();
            if(t->left) q.push(t->left);
            if(t->right) q.push(t->right);
            delete t;
        }
    }

    int getHeight(Node *node)
    {
        if(node == nullptr) return 0;
        return max(getHeight(node->left),getHeight(node->right))+1;
    }

    Node *insert(const T& data)
    {
        root_ = dfsInsert(root_,data);
        return root_;
    }

    void bfs()
    {
        //  分层打印BST树 。bfs树时维护一个层数
        using levelNode = pair<int,Node*>;
        queue<levelNode> q;
        q.push(make_pair(0,root_));
        int curLevel = 0;
        while(!q.empty())
        {
            levelNode t = q.front();
            q.pop();

            Node *node = t.second;
            int level = t.first;
            if(curLevel < level){ cout<<endl;curLevel = level;}

            cout<<"{ "<<level<<" " <<node->data<<" } ";
            Node *left_child = node->left;
            Node *right_child = node->right;
            if(left_child) q.push(make_pair(level+1,left_child));
            if(right_child) q.push(make_pair(level+1,right_child));
        }
        return ;
    }


    Node* remove(const T&data)
    {
        root_ = dfsRemove(root_ , data);
        return root_;
        return dfsRemove(root_ , data);
    }
private:

    Node *dfsRemove(Node *r,const T&data)
    {
        if(r == nullptr)
        {
            //  没找到节点，整个树没有变化。
            cout<<"node "<<data<<" does not existed"<<endl;
            return nullptr;
        }
        if(data > r->data)
        {
            r->right = dfsRemove(r->right,data);
            //  右子树删除节点，可能造成左子树太高
            if(getHeight(r->left) - getHeight(r->right) > 1)
            {
                //  左孩子的左子树太高
                if(getHeight(r->left->left) >= getHeight(r->left->right))
                {
                    r = rightRotate(r);
                }
                //  左孩子的右子树太高
                else
                {
                    r = leftBalance(r);
                }
            }
        }
        else if(data <  r->data)
        {
            r->left = dfsRemove(r->left,data);
            //  左子树删除节点，可能造成右子树太高
            if(getHeight(r->right) - getHeight(r->left) > 1)
            {
                //  有孩子的右子树太高
                if(getHeight(r->right->right) >= getHeight(r->right->left))
                {
                    r = leftRotate(r);
                }
                //  右孩子的左子树太高
                else
                {
                    r = rightBalance(r);
                }
            }
        }
        else if(data == r->data)
        {
            //  根据BST树可知，如果左右孩子都不为空，需要删除前驱或者后继。那
            //  那么再加上一个平衡条件，是应该删除前驱还是后继呢？
                //  答案：哪个孩子比较高，就删除哪个孩子的节点。
                //  左孩子较高，就删除前驱。右孩子较高，就删除后继。(高度相等可以归于删除前驱或者后继)
                //  这样可以保证删除节点后不会发生失衡。
            if(r->left!= nullptr && r->right!= nullptr)
            {
                //  删除前去
                if(getHeight(r->left) >= getHeight(r->right))
                {
                    Node *pre = r->left;
                    while(pre->right!= nullptr)
                    {
                        pre = pre->right;
                    }
                    r->data = pre->data;
                    r->left = dfsRemove(r->left,pre->data);        //  当前层继续向下递归，从r的左子树中删除前驱节点
                }
                //  删除后继
                else
                {
                    Node *post = r->right;
                    while(post->left!= nullptr)
                    {
                        post = post->left;
                    }
                    r->data = post->data;
                    r->right = dfsRemove(r->right,post->data);      //  当前层继续向下递归，从r的右子树中删除后继节点
                }
            }
            //  删除节点，最多有一个孩子
            else
            {
                if(r->left)
                {
                    Node *l = r->left;
                    delete r;
                    return l;
                }
                else if(r->right)
                {
                    Node *right = r->right;
                    delete r;
                    return right;
                }
                else
                {
                    delete r;
                    return nullptr;
                }
            }
        }
        r->height = getHeight(r);
        //  回溯时 将当前已经维护好的子树根节点返回给上级
        return r;
    }

    Node *rightRotate(Node *node)
    {
        //  rotate
        Node *child = node->left;
        node->left = child->right;
        child->right = node;
        //  update height       (左/右子树发生改变，因此高度需要更新）
        node->height = getHeight(node);
        child->height = getHeight(child);
        return child;
    }

    Node *leftRotate(Node *node)
    {
        //  rotate
        Node *child = node->right;
        node->right = child->left;
        child->left = node;
        //  update height
        node->height = getHeight(node);
        child->height = getHeight(child);
        return child;
    }

    Node *leftBalance(Node *node)
    {
        node->left = leftRotate(node->left);
        rightRotate(node);
    }

    Node *rightBalance(Node *node)
    {
        node->right = rightRotate(node->right);
        leftRotate(node);
    }


    //  向以r为根的BST子树插入data节点。并保证该BST树的性质。返回根节点r.(r始终是该BST子树的根节点)
    Node *dfsInsert(Node *r,const T& data)
    {
        if(r == nullptr)
        {
            return new Node(data);
        }
        else if(data > r->data)
        {
            r->right = dfsInsert(r->right,data);
            //  处理节点失衡。注意更新本层的树的根节点。然后再返回给上一层。
            //  右孩子过高
            if(getHeight(r->right) - getHeight(r->left) > 1)
            {
                //  右孩子的右子树 比 右孩子的左子树 高
                if(getHeight(r->right->right) >= getHeight(r->right->left))
                {
                    r = leftRotate(r);

                }
                //  有孩子的左子树 比 右孩子的右子树 高
                else
                {
                    r = rightBalance(r);
                }
            }
        }
        else if(data < r->data)
        {
            r->left = dfsInsert(r->left,data);
            //  左孩子过高
            if(getHeight(r->left) - getHeight(r->right) > 1)
            {
                //  左孩子的左子树 比 左孩子的右子树 高
                if(getHeight(r->left->left) >= getHeight(r->left->right))
                {
                    r = rightRotate(r);
                }
                //  左孩子的右子树 比 左孩子的左子树 高
                else
                {
                    r = leftBalance(r);
                }
            }
        }
        else
        {
            //  相同节点，不用再插入了。
            return r;
        }
        //  在回溯时 检测更新节点高度 。这里全部进行更新了。可优化。
        r->height = getHeight(r);
        return r;
    }
private:
    Node *root_;
};


int main()
{
    vector<int> vec{1,2,3,4,5,6,7,8,9,10};
    BalanceSearchTree<int> avl;
    for(auto x : vec)
    {
        avl.insert(x);
    }
    avl.bfs();

    cout<<endl<<"=================================="<<endl;


    avl.remove(9);
    avl.bfs();

    cout<<endl<<"=================================="<<endl;
    avl.remove(10);
    avl.bfs();


    cout<<endl<<"=================================="<<endl;
    avl.remove(6);
    avl.bfs();


    cout<<endl<<"=================================="<<endl;
    avl.remove(1);

    avl.bfs();
    cout<<endl<<"=================================="<<endl;

    avl.remove(2);

    avl.bfs();
    cout<<endl<<"=================================="<<endl;

    avl.remove(3);
    avl.bfs();
}
