/*
BST树插入，删除，查询操作，递归和非递归实现
BST树前序，中序，后序，层序遍历的递归和非递归实现
BST求树的高度，节点元素个数
BST树区间元素查找
BST的LCA问题：求寻找最近公共祖先节点
BST树的镜像对称问题
求BST树中序遍历倒数第K个节点 LVR -> RVL
*/


#include <iostream>
#include<cassert>
#include<vector>
#include<queue>
#include<stack>
#include<algorithm>
#include<functional>

using namespace std;

template<typename T,typename Compare = less<T>>
class BST
{
public:
    struct Node {
        T data;
        Node *left;
        Node *right;
        Node(T v,Node *l= nullptr,Node*r = nullptr) : data(v),left(l),right(r){}
    };
public:
    BST(Compare comp = Compare()) :cmp_(comp),root_(nullptr){}

    ~BST()
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

    //  n_: 非递归
    //  插入
    void n_insert(const T& data)
    {
        if(root_ == nullptr)
        {
            root_ = new Node(data);
            return ;
        }

        Node *cur = root_;
        Node *pre = nullptr;

        while(cur != nullptr)
        {
            if(data == cur->data)
            {
                return ;                    //  树中相同元素只插入一个
            }
            if(!cmp_(data,cur->data))       //  >= 为 true
            {
                pre = cur;
                cur = cur->right;
            }
            else if(cmp_(data,cur->data))   //  < true
            {
                pre = cur;
                cur = cur->left;
            }
        }

        assert(cur== nullptr);
        if(cmp_(data , pre->data))
            pre->left = new Node(data);
        else
            pre->right = new Node(data);
        return ;
    }
    //  删除
    void n_remove(const T &data)
    {
        if(root_ == nullptr) return ;
        //  寻找要删除节点
        //  node 指向待删除的节点
        //  pre_node 指向node的父节点
        Node *node = root_;
        Node *pre_node = nullptr;
        while(node!= nullptr)
        {
            if(data == node->data)
            {
                break;
            }
            else if(!cmp_(data,node->data))
            {
                pre_node = node;
                node = node->right;
            }
            else
            {
                pre_node = node;
                node = node->left;
            }
        }
        if(node == nullptr) return ;

        //  situation 3 : 左右孩子都有。这里选择寻找**前驱节点**来覆盖要删除的node
        if(node->left != nullptr && node->right!= nullptr)
        {
            //  指向node的前驱节点。即node左子树中最大(右下)的节点
            Node *predecessor_node = node->left;
            //  前驱节点的父节点
            Node *pre_predecessor = node;
            while(predecessor_node->right!= nullptr)
            {
                pre_predecessor = predecessor_node;
                    predecessor_node = predecessor_node->right;
            }

            //  要删除的节点node 和 前驱节点 交换 data
            //  那么 要删除的节点 就变成了 前驱节点、要删除的节点的父节点也变为前驱节点的父节点
            swapData(node,predecessor_node);
//            node->data = predecessor_node->data;
            node = predecessor_node;
            pre_node = pre_predecessor;
        }

        //  转化为situation 1 : 没有孩子：直接删除该node ; situation 2 : 有一个孩子：将孩子写入node位置
        //  都可由下处理
        Node *child = node->left ? node->left : node->right;         //  至多有一个child，选择一个。
        if(pre_node == nullptr)
        {
            root_ = child;
        }
        else
        {
            pre_node->left == node ? pre_node->left = child : pre_node->right = child;
        }

        delete node;
    }
    //  查询
    bool n_query(const T& data)
    {
        if(!root_) return false;
        Node *cur = root_;
        while(cur!= nullptr)
        {
            if(data == cur->data)
            {
                break;
            }
            else if(!cmp_(data,cur->data))
            {
                cur = cur->right;
            }
            else
            {
                cur = cur->left;
            }
        }
        return cur!= nullptr;
    }
    //  前序遍历 VLR  不管前序、中序、后序遍历 还是递归比较好些。因为必须要用到栈。而递归就已经帮我们实现好了。
    void n_preOrder()
    {
        cout<<"===========n_preOrder start============"<<endl;
        if(root_ == nullptr) return ;
        stack<Node*> s;
        s.push(root_);
        while(!s.empty())
        {
            Node *t = s.top();
            s.pop();
            assert(t!= nullptr);
            //  deal with v data
            cout<<t->data<<" ";
            if(t->right) s.push(t->right);
            if(t->left) s.push(t->left);
        }
        cout<<endl<<"===========n_preOrder end=============="<<endl;
    }
    //  中序遍历 L V R
    void n_inOrder()
    {
        cout<<"===========n_inOrder start============"<<endl;
        if(root_ == nullptr) return ;
        stack<Node*> s;
        Node *cur = root_;
        //  左中右
        while(!s.empty() || cur != nullptr)     //  !stack.empty 是为了保证遍历完整个树 cur!=nullptr 是为了将左节点全部压入栈
        {
            if(cur)                     //  (子)树根节点  压栈 并不访问节点
            {
                s.push(cur);
                cur = cur->left;
            }
            else                        //  访问/遍历节点
            {
                Node *t = s.top();  //  在访问t的时候，t的左孩子一定已经访问完了。
                s.pop();
                cur = t->right;         //  压栈 t的左孩子和根(t自己)都已经访问完了 按照左中右 接下来该右孩子(右子树的根)入栈
            }
        }
        cout<<endl<<"===========n_inOrder end=============="<<endl;
    }
    //  后序遍历 L R V 。实现：由于V不好存储，将顺序逆序变换为 VRL ，然后再逆序，即得到后序遍历
    void n_postOrder(){
        cout<<"==========n_postOrder start============"<<endl;
        if(root_ == nullptr) return ;
        stack<Node*> s;
        stack<Node*> bt;
        s.push(root_);
        while(!s.empty())
        {
            Node *t = s.top();
            s.pop();
//            deal with t node
            bt.push(t);
            if(t->left) s.push(t->left);
            if(t->right) s.push(t->right);
        }
        while(!bt.empty())
        {
            Node *t = bt.top();
            cout<<t->data<<" ";
            bt.pop();
        }
        cout<<endl<<"==========n_postOrder end============"<<endl;
    }

    //  dfs版本
    void insert(const T& data){
        root_ = dfsInsert(root_,data);
        ///  轻松解决root_ = null的情景。
        /// 首先 从意义上来讲 ，dfsInsert 保证使得root_成为BST树的根节点，故可以自然处理root；
        /// 其次，对于root = nullptr 会直接将node给root作为根节点
    }
    bool query(const T& data){
        return dfsQuery(root_,data);
    }
    void remove(const T& data){
        root_ = dfsRemove(root_,data);
    }
    //  层序遍历
    void levelOrder()
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
    void preOrder()
    {
        cout<<"==========dfs preOrder start==========="<<endl;
        dfsPreOrder(root_);
        cout<<endl<<"==========dfs preOrder end==========="<<endl;
    }
    //  中序遍历
    void inOrder()
    {
        cout<<"==========dfs inOrder start==========="<<endl;
        dfsInOrder(root_);
        cout<<endl<<"==========dfs inOrder end==========="<<endl;
    }
    //  后序遍历
    void postOrder()
    {
        cout<<"==========dfs postOrder start==========="<<endl;
        dfsPostOrder(root_);
        cout<<endl<<"==========dfs postOrder end==========="<<endl;
    }

    //  fill vec with [v1,v2]
    void findValues(vector<T> & vec,const T &v1,const T &v2)
    {
        dfsFindValues(root_,vec,v1,v2);
    }

    bool isChildTree(const Node * child) const
    {
        if(!child) return true;
        if(!root_) return false;
        Node *cur = root_;
        while(cur)
        {
            if(cur->data == child->data) break;
            else if(cmp_(child->data,cur->data))        //  <
            {
                cur = cur->left;
            }
            else                                        //  >
            {
                cur = cur->right;
            }
        }
        if(cur == nullptr) return false;
        return dfsIsChildTree(cur,child);
    }


    //  x,y的最近公共祖先：从根节点开始向下遍历，第一个 x <= node <= y 就是 x和y自底向上的最近的公共祖先
    Node* getLCA(const T& x,const T& y)
    {
        //  如果函数里需要用到递归的话 大概率dfs要再写成另一个辅助函数。因为dfs和对外接口的函数的参数不同
        //  真是好久不写算法了。。老年人复健了属于是
        if(!root_) return nullptr;
        return dfsLCA(root_,x,y);
    }

    //  LVR LAST K -> 倒序 RVL FIRST K
     Node* getInOrderLastK(int k)
    {
        i_last_k = 0;
        if(!root_) return nullptr;
        return dfsGetInOrderLastK(root_,k);
    }

    void showAllData();
private:
    int i_last_k;
     Node *dfsGetInOrderLastK(Node *cur,int k)
    {
        if(!cur) return nullptr;
        Node *t = dfsGetInOrderLastK(cur->right,k);
        if(t!= nullptr) return t;

        ++i_last_k;
        if(i_last_k == k) return cur;

        return dfsGetInOrderLastK(cur->left,k);
    }
    //  返回以cur为根的树的x和y的最近公共祖先
    Node* dfsLCA(Node *cur, const T& x,const T& y)
    {
        if(!cur) return nullptr;
        //  没用cmp这里 偷懒了。
        if(cur->data > x && cur->data > y)
        {
            return dfsLCA(cur->left,x,y);
        }
        else if(cur->data < x && cur-> data < y)
        {
            return dfsLCA(cur->right,x,y);
        }
        else    //  x <= node <= y
        {
            return cur;
        }
    }

    bool dfsIsChildTree(const Node * r , const Node * child) const {
        //  check
        if(!r && !child) return true;
        if(!r) return false;
        if(!child) return true;
        if(r->data!=child->data) return false;
        //  dfs root_ and child
        return dfsIsChildTree(r->left,child->left)
                && dfsIsChildTree(r->right,child->right);
    }

    void dfsFindValues(Node *cur,vector<T> & vec,const T& v1,const T& v2)
    {
        if(cur == nullptr) return ;
        //  剪枝 : 只有 cur->data >= v1 cur的左子树才会有符合 >=v1 的
        if(!cmp_(cur->data,v1))
        {
            dfsFindValues(cur->left,vec,v1,v2);           //  左 中 右
        }

        //  剪枝 已经把[v1,v2]的数都取完了  //  cur->data > v2
        if(!(v2==cur->data) && !cmp_(cur->data,v2)) return ;

        //  !(cur->data < v1) 即  v2 >= cur->data >= v1
        if(!cmp_(cur->data,v1))
        {
            vec.push_back(cur->data);
        }

        dfsFindValues(cur->right,vec,v1,v2);
    }
    //  递归前序遍历
        //  以 根 左 右 的顺序遍历以cur为根节点的子树
    void dfsPreOrder(Node *cur)
    {
        if(cur == nullptr)
            return ;
        cout<<cur->data<<" ";
        dfsPreOrder(cur->left);
        dfsPreOrder(cur->right);
    }

    void dfsInOrder(Node* cur)
    {
        if(cur == nullptr)
            return ;
        dfsInOrder(cur->left);
        cout<<cur->data<<" ";
        dfsInOrder(cur->right);
    }

    void dfsPostOrder(Node *cur)
    {
        if(cur == nullptr)
            return ;
        dfsPostOrder(cur->left);
        dfsPostOrder(cur->right);
        cout<<cur->data<<" ";
    }

    //  将node(T)正确插入以cur为根的子树 并返回cur(即返回该子树的树根 -- 是为了和其对应的父节点建立/保持联系）(这样就通过返回值而非传参来传递父节点了）（传递父节点是为了让新建立的节点挂在树上）
    //  **保证使得以cur为根的子树 成为一棵BST树。并子树返回根节点cur**
    Node* dfsInsert(Node *cur,const T &val)
    {
        if(cur == nullptr)
        {
            return new Node(val);
        }
        else if(!cmp_(val , cur->data))                         //  >
        {
            cur->right = dfsInsert(cur->right , val);       //  cur->right/left = 仅仅是为了让新节点挂在树上 对已经建立号关系的节点没有影响。
        }
        else
        {
            cur->left = dfsInsert(cur->left,val);
        }
        return cur;
    }

    bool dfsQuery(Node *cur,const T&data)
    {
        if(!cur)
            return false;
        if(data == cur->data)
        {
            return true;
        }
        if(!cmp_(data,cur->data))
        {
            return dfsQuery(cur->right,data);
        }
        else
        {
            return dfsQuery(cur->left,data);
        }
    }

    //  从以cur为根的BST树中删除 node(data)
    //  并保证删除完之后，以cur为根的子树仍然是一个BST树
    //  返回该子树的根节点 cur
    Node* dfsRemove(Node *cur,const T& data)
    {
        if(cur == nullptr)
            return nullptr;
        if(data == cur->data)       //  cur is the node to delete
        {
            //  situation 3
            if(cur->left!= nullptr && cur->right!= nullptr)
            {
                //  寻找前驱节点
                Node *predecessor = cur->left;
                while(predecessor->right!= nullptr)
                {
                    predecessor = predecessor->right;
                }
                //  覆盖data
                cur->data = predecessor->data;
                //  要删除的节点变成了原先的前驱节点. dfs删除前驱节点
                //  转化成situation 1 , 2
                cur->left = dfsRemove(cur->left,predecessor->data);
                return cur;
            }
            //  situation 1
            else if(cur->left == nullptr && cur->right == nullptr)
            {
                delete cur;
                return nullptr;
            }
            //  situation 2
            else
            {
                if(cur->left!= nullptr)
                {
                    Node *t = cur->left;
                    delete cur;
                    return t;
                }
                else
                {
                    Node *t = cur->right;
                    delete cur;
                    return t;
                }
            }
        }
        else if(!cmp_(data,cur->data))
        {
            cur->right = dfsRemove(cur->right,data);
        }
        else
        {
            cur->left = dfsRemove(cur->left,data);
        }
        return cur;
    }
    void swapData(Node *x,Node *y)
    {
        swap((x->data),(y->data));
    }
private:
    Node *root_;
    Compare cmp_;
};

template<typename T,typename Compare>
void BST<T,Compare>::showAllData() {
    cout<<"bst tree"<<endl;
}

int main()
{
//    testChild();
//    testLCA();
//    testLastK();
    testCtorDtor();
    return 0;
}




void testBase()
{
    BST<int> bst;
    vector<int> data{58,24,67,0,34,62,69,5,41,64,78};
    for(int i=0;i<data.size();++i)
    {
        bst.insert(data[i]);
    }



    bst.levelOrder();
    cout<<endl<<"==========================="<<endl;

    bst.preOrder();

    bst.inOrder();

    bst.postOrder();

    bst.n_preOrder();
    bst.n_inOrder();
    bst.n_postOrder();

    cout<<endl<<"==========================="<<endl;

    bst.remove(58);
    bst.levelOrder();

    cout<<endl<<"==========================="<<endl;

    bst.remove(78);
    bst.levelOrder();
    cout<<endl<<"==========================="<<endl;

    bst.remove(67);
    bst.levelOrder();

    cout<<endl<<"==========================="<<endl;

    cout<<bst.n_query(41)<<" "<<bst.n_query(65)<<" "<<bst.n_query(64)<<endl;
    cout<<bst.query(41)<<" "<<bst.query(65)<<" "<<bst.query(64)<<endl;


    cout<<endl<<"==========================="<<endl;

    bst.remove(1);
    bst.remove(24);
    bst.levelOrder();

    cout<<endl<<"==========================="<<endl;

    bst.remove(34);
    bst.remove(5);

    bst.levelOrder();

    cout<<endl<<"==========================="<<endl;

    vector<int> vec;
    bst.findValues(vec,41,41);
    for_each(vec.begin(),vec.end(),[](int x)->void {cout<<x<<" ";});
    cout<<endl;
    bst.showAllData();
}


void testChild()
{
    BST<int> bst;
    vector<int> data{58,24,67,0,34,62,69,5,41,64,78};
    for(int i=0;i<data.size();++i)
    {
        bst.insert(data[i]);
    }
    bst.levelOrder();

    using Node = BST<int>::Node;
    Node *r = new Node(24);
    r->left = new Node(0);
    r->left->right = new Node(5);
    r->right = new Node(34);
    r->right->right = new Node(41);
    cout<<endl;
    cout<<bst.isChildTree(r)<<endl;
}


void testLCA()
{
    BST<int> bst;
    vector<int> data{58,24,67,0,34,62,69,5,41,64,78,1};
    for(int i=0;i<data.size();++i)
    {
        bst.insert(data[i]);
    }
    bst.levelOrder();

    cout<<bst.getLCA(1,41)->data<<endl;
    cout<<bst.getLCA(1,5)->data<<endl;
    cout<<bst.getLCA(1,0)->data<<endl;
    cout<<bst.getLCA(1,78)->data<<endl;
    cout<<bst.getLCA(41,64)->data<<endl;
}


void testLastK()
{
    BST<int> bst;
    vector<int> data{58,24,67,0,34,62,69,5,41,64,78,1};
    for(int i=0;i<data.size();++i)
    {
        bst.insert(data[i]);
    }
//    bst.levelOrder();
//    cout<<endl;
    bst.inOrder();
    cout<<endl;
    cout<<bst.getInOrderLastK(1)->data<<endl;
    cout<<bst.getInOrderLastK(2)->data<<endl;
    cout<<bst.getInOrderLastK(3)->data<<endl;
}


void testCtorDtor()
{
    using Elm = pair<int,string>;
    BST<Elm,function<bool(Elm,Elm)>> bst([](Elm x,Elm y)->bool{
        return x.second < y.second;
    });
    bst.insert(make_pair(1,"999"));
    bst.insert(make_pair(1,"123"));
    bst.insert(make_pair(1,"456"));
    bst.insert(make_pair(1,"789"));
}
