//  没有实现析构，不过也好做，和其他树一样，就是个bfs.
//  主要关注插入和删除。
//
// Created by dell on 2022-11-05.
//
#include<iostream>
#include<queue>
#include<unordered_map>
#include<cassert>

using namespace std;
template<typename T>
class RBTree
{
public:
    enum Color{BLACK , RED};
    struct Node
    {
        Node(T v = T(),Color c = BLACK ,Node *p = nullptr,Node *l= nullptr,Node *r= nullptr)
            :data(v),
            color(c),
             parent(p),
            left(l),
            right(r)
        {}
        T data;
        Color color;
        Node *left;
        Node *right;
        Node *parent;
    };
    unordered_map<int,string> colors{{0,"BLACK"},{1,"RED"}};
public:
    RBTree(Node *r = nullptr)
        : root_(r)
    {}

    void bfs()
    {
        cout<<endl<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
        //  分层打印BST树 。bfs树时维护一个层数
        using levelNode = pair<int,Node*>;
        queue<levelNode> q;
        if(!root_) return ;
        q.push(make_pair(0,root_));
        int curLevel = 0;
        while(!q.empty())
        {
            levelNode t = q.front();
            q.pop();

            Node *node = t.second;
            int level = t.first;
            if(curLevel < level){ cout<<endl;curLevel = level;}

            cout<<"{ "<<level<<" " <<node->data<<" "<<colors[node->color]<<" "<<node->left<<" "<<node->right<<" } ";
            Node *left_child = node->left;
            Node *right_child = node->right;
            if(left_child) q.push(make_pair(level+1,left_child));
            if(right_child) q.push(make_pair(level+1,right_child));
        }
        cout<<endl<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
        return ;
    }

    void insert(const T &data)
    {
        if(root_ == nullptr)
        {
            root_ = new Node(data,BLACK);       //  根节点必须是black
            return ;
        }

        Node *cur = root_;
        Node *pre = nullptr;
        while(cur)
        {
            if(data>cur->data)
            {
                pre = cur;
                cur = cur->right;
            }
            else if(data < cur->data)
            {
                pre = cur;
                cur = cur->left;
            }
            else        //  找到相同节点
            {
                return ;
            }
        }
        Node *node = new Node(data,RED,pre);        //  为使得影响最小，插入节点为RED
        if(node->data > pre->data)
        {
            pre->right = node;
        }
        else
        {
            pre->left = node;
        }

        //  如果新插入节点的父节点是 RED ，那么需要分类讨论进行调整
        //  如果新插入节点的父节点是 BLACK ，那么不需要调整
        if(pre->color == RED)
        {
            fixAfterInsert(node);
        }
    }

    void remove(const T& data)
    {
        if(!root_) return;

        Node *cur = root_;
        while(cur)
        {
            if(data > cur->data)
            {
                cur = cur->right;
            }
            else if(data < cur->data)
            {
                cur = cur->left;
            }
            else
            {
                break;
            }
        }
        if(!cur) return ;

        //  situation 3 : 2个孩子。那么找前驱节点对目标节点进行覆盖
        if(cur->left && cur->right)
        {
            //  寻找前驱节点（左子树中最右的节点）
            Node *pioneer = cur->left;
            while(pioneer->right)
            {
                pioneer = pioneer->right;
            }
            //  前驱节点覆盖目标节点
            cur->data = pioneer->data;
            //  前驱节点变成目标节点
            cur = pioneer;
        }

        //  当前目标节点的孩子节点
        Node *child = cur->left ? cur->left : cur->right;
        //  situation 2
            //  child不为空，将child接在cur的parent的下
        if(child != nullptr)
        {
            Node *parent = cur->parent;
            child->parent = parent;
            if(parent == nullptr)
            {
                root_ = child;
            }
            else
            {
                if(cur == parent->left)
                {
                    parent->left = child;
                }
                else if(cur == parent->right)
                {
                    parent->right = child;
                }
                else
                {
                    cout<<"internal error!"<<endl;
                }
            }

            //  调整
            Color c = color(cur);
            delete cur;
            //  删除红色，无需调整
            if(c == RED)
            {
                return ;
            }
            //  删除黑色，需要调整
            else if(c == BLACK)
            {
                fixAfterRemove(child);
            }
        }
        //  situation 1 : 删除节点的孩子均为空. child == nullptr
        else
        {
            //  删除的是根节点
            //  cur的父节点为nullptr child为nullptr。故cur为root
            if(cur->parent == nullptr)
            {
                delete cur;
                root_ = nullptr;
                return ;
            }
            //  删除的是叶子节点
            else
            {
                //  叶子是black，那么不能随便删. 因为会影响性质5
                if(cur->color == BLACK)
                {
                    //  从兄弟侧借来一个兄弟节点，或者减少兄弟侧的一个节点
                    fixAfterRemove(cur);
                }
                //  删除cur而不必担心黑色节点数量性质
                if(cur->parent->left == cur)
                {
                    cur->parent->left = nullptr;
                }
                else
                {
                    cur->parent->right = nullptr;
                }
                delete cur;
            }
        }
    }

private:
    //  作用：从cur的兄弟一侧借来一个节点，使得cur这侧和cur兄弟一侧的黑色节点个数相同；且不改变cur节点之下的节点结构。也即，不会改变cur节点的left right
    void fixAfterRemove(Node *cur)      //  传入补上来的孩子节点
    {
//        cout<<"======================fixAfterRemove start======================"<<endl;
        if(cur->color == RED)
        {
            cur->color = BLACK;             //  发现RED节点 直接变为BLACK即可
            return ;
        }
        while(cur!=root_ && color(cur) == BLACK)
        {
            Node *parent = cur->parent;
            if(cur == parent->left)
            {
                Node *brother = parent->right;
                //  situation 4
                if(color(brother) == RED)
                {
                    parent->color = RED;
                    brother->color = BLACK;
                    leftRotate(parent);
                    brother = parent->right;        //  对situation 4调整完，目标节点没变，更新brother节点。落入其他三种情况。
                }

                assert(brother->color == BLACK);
                //  situation 3
                if(color(brother->left) == BLACK && color(brother->right) == BLACK)
                {
                    brother->color = RED;
                    cur = cur->parent;
                    continue;                       //  继续下一轮调整。向上回溯。落入四种情况。
                }
                else
                {
                    //  situation 2
                    if(color(brother->left) == RED && color(brother->right) == BLACK)
                    {
                        brother->left->color = BLACK;
                        brother->color = RED;
                        rightRotate(brother);
                        brother = parent->right;        //  目标节点没变。更新brother节点。落入situation 1
                    }
                    //  situation 1
                    assert(color(parent->left) == BLACK && color(parent->right) == BLACK && color(parent->right->right) == RED);
                    brother->color = parent->color;
                    parent->color = BLACK;
                    brother->right->color = BLACK;
                    leftRotate(parent);
                    //  调整结束
                    break ;
                }
            }
            //  翻过来 同理
            else
            {
                Node *brother = parent->left;
                //  situation 4
                if(color(brother) == RED)
                {
                    parent->color = RED;
                    brother->color = BLACK;
                    rightRotate(parent);
                    brother = parent->left;        //  对situation 4调整完，目标节点没变，更新brother节点。落入其他三种情况。
                }

                assert(brother->color == BLACK);
                //  situation 3
                if(color(brother->right) == BLACK && color(brother->left) == BLACK)
                {
                    brother->color = RED;
                    cur = cur->parent;
                    continue;                       //  向上回溯 。继续下一轮调整。落入四种情况。
                }
                else
                {
                    //  situation 2
                    if(color(brother->right)== RED && color(brother->left) == BLACK)
                    {
                        brother->right->color = BLACK;
                        brother->color = RED;
                        leftRotate(brother);
                        brother = parent->left;        //  目标节点没变。更新brother节点。落入situation 1
                    }

                    //  situation 1
                    assert(color(parent->right) == BLACK && color(parent->left) == BLACK && color(parent->left->left) == RED);
                    brother->color = parent->color;
                    parent->color = BLACK;
                    brother->left->color = BLACK;
                    rightRotate(parent);
                    break;                  //  调整结束
                }
                //  可以看出 红黑树删除最多旋转三次。situation 4 -> 2 -> 1
            }
        }
        //  祖先节点为红色，变黑即可。(针对情况3的一种处理）
        if(color(cur)==RED)
            cur->color = BLACK;
//        cout<<"======================fixAfterRemove end======================"<<endl;
    }

    //  分为三种情况
    void fixAfterInsert(Node *cur)
    {
//        cout<<"============fixInsert start============"<<endl;
        //  已知cur是红色
        //  那么 如果parent是红色 就一直调整下去
        while(color(cur)==RED && color(cur->parent) == RED)     //  color(cur->parent) 处理了空节点的情况
        {
            Node *parent = cur->parent;
            Node *grand = parent->parent;
//            cout<<cur->data<<" "<<cur->parent->data<<" "<<(cur==parent->left)<<" "<<(cur==parent->right)<<" "<<colors[cur->color]<<endl;
            //  parent 是 grand的左孩子
            if(parent == grand->left)
            {
//                cout<<"branch1"<<endl;
                Node *uncle = grand->right;
                //  情况1
                if(uncle && uncle->color == RED)
                {
//                    cout<<"tag4"<<endl;
                    parent->color = BLACK;
                    grand->color = RED;
                    uncle->color = BLACK;
                    //  继续调整
                    cur = cur->parent->parent;
                    continue;
                }
                //  情况2,3
                if(uncle== nullptr || uncle->color == BLACK)
                {
//                    cout<<"tag3.0"<<endl;
                    //  可以看到红黑树一次调整最多旋转两次 3->2
                    //  情况3 -> 2
                    if(cur != parent->left && parent == grand->left)
                    {
//                        cout<<"tag3"<<endl;
                        leftRotate(parent);
                        //  旋转完之后交换一下
                        swap(cur,parent);
                    }
//                    cout<<"tag3.1s"<<endl;
                    //  情况2
                    parent->color = BLACK;
                    grand->color = RED;
                    rightRotate(grand);     //  维护路径上的block node 数量
                    // finish
                    break;
                }
            }
            //  同理 parent 是 grand的右孩子
            else
            {
//                cout<<"branch2"<<endl;
                Node *uncle = grand->left;
                //  情况1
                if(uncle && uncle->color == RED)
                {
//                    cout<<"tag1"<<endl;
                    parent->color = BLACK;
                    grand->color = RED;
                    uncle->color = BLACK;
                    //  继续调整
                    cur = cur->parent->parent;
                    continue;
                }
                //  情况2,3
                if(uncle== nullptr || uncle->color == BLACK)
                {
//                    cout<<"tag2"<<endl;
                    //  可以看到红黑树一次调整最多旋转两次 3->2
                    //  情况3 -> 2
                    if(cur != parent->right && parent == grand->right)
                    {
                        rightRotate(parent);
                        //  旋转完之后交换一下
                        swap(cur,parent);
                    }
//                    cout<<cur->data<<" "<<cur->parent->data<<" "<<(cur==parent->left)<<" "<<(cur==parent->right)<<" "<<colors[cur->color]<<endl;
                    //  情况2
                    parent->color = BLACK;
                    grand->color = RED;
                    leftRotate(grand);     //  维护路径上的block node 数量
                    // finish
                    break;
                }
            }

        }
        root_->color = BLACK;
//        cout<<"============fixInsert end============"<<endl;
    }
    //  node的右孩子绕着node左旋转
    void leftRotate(Node *node)
    {
        Node *child = node->right;
        Node *parent = node->parent;

        node->right = child->left;
        if(child->left)
            child->left->parent = node;

        child->left = node;
        node->parent = child;

        child->parent = parent;
        if(parent == nullptr)
        {
            root_ = child;
        }
        else
        {
            if(node == parent->left) parent->left = child;
            else if(node == parent->right) parent->right = child;
            else {cerr<<"internal error!"<<endl; exit(-1);}
        }
    }

    //  node的左孩子绕着node进行右旋转。
    void rightRotate(Node *node)
    {
        Node *child = node->left;
        Node *parent = node->parent;

        node->left = child->right;
        if(child->right)
            child->right->parent = node;

        child->right = node;
        node->parent = child;

        child->parent = parent;
        if(parent == nullptr)
        {
            root_ = child;
        }
        else
        {
            if(parent->right == node)   parent->right = child;
            else if(parent->left == node) parent->left = child;
            else {cerr<<"internal error"<<endl; exit(-1);}
        }
    }

    //  对于null和node均可返回对应color
    Color color(Node *node)
    {
        return node ? node->color : BLACK;
    }

private:
    Node *root_;
};



int main()
{
    RBTree<int> rb;
    for(int i=1;i<=10;++i)
    {
        rb.insert(i);
    }
    rb.bfs();
    for(int i=0;i<=10;--i)
        rb.remove(i);
    rb.bfs();

    for(int i=100;i>=0;--i) {
        rb.insert(i);
        rb.remove(i+20);
    }
    rb.bfs();
}
