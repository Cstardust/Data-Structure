#include<iostream>
#include<cstring>

using namespace std;

//  实现了 默认构造、左值拷贝构造、左值赋值、右值移动构造、右值移动赋值、+、<<的class MyString

class CMyString
{
public:
//  三大件：拷贝构造 拷贝赋值 析构函数
    CMyString(const char *ptr=nullptr)
    {
        cout<<"CMyString(const char*)"<<endl;
        if(!ptr)
        {
            this->m_ptr = new char[1];
            *m_ptr = '\0';
        }
        else
        {
            this->m_ptr = new char[strlen(ptr) + 1];
            strcpy(this->m_ptr,ptr);
        }
    }

    //  带左值引用参数的拷贝构造函数
    CMyString(const CMyString &str) //  拷贝构造
    {
        cout<<"CMyString(const CMyString &)"<<endl;

        this->m_ptr = new char[strlen(str.m_ptr) + 1];
        strcpy(this->m_ptr,str.m_ptr);
    }

    //  带右直引用参数的拷贝构造函数
    CMyString(CMyString && str)     //  str即将被销毁，故可以匹配到右直引用的参数列表
    {
        cout<<"CMyString(const CMyString &&)"<<endl;
        this->m_ptr = str.m_ptr;
        str.m_ptr = nullptr;        //  别忘了！！！不然一会str析构，则会把str.m_ptr给释放
    }

    // 带左值引用参数的赋值重载函数
    CMyString& operator=(const CMyString &str)
    {
        cout<<"CMyString& operator=(const CMyString &str)"<<endl;

        if(this==&str) return *this;    //  判是否是自己 拷贝构造则不需要，因为构造麻，调用构造函数的对象本身还没构造完呢，怎么可能和传进来的一样。

        delete[] this->m_ptr;
        this->m_ptr = new char[strlen(str.m_ptr) + 1];
        strcpy(this->m_ptr,str.m_ptr);
        return *this;
    }

    //  带右直引用参数的赋值重载函数
    CMyString& operator=(CMyString &&str)
    {
        cout<<"CMyString& operator=(CMyString&&) "<<endl;
        this->m_ptr = str.m_ptr;
        str.m_ptr = nullptr;        ////  别忘了！！！不然一会str析构，则会把str.m_ptr给释放
        //  这也就是C++ Primer所说的，使右直引用的对象处于可以安全析构的状态。
        return *this;
    }

    ~CMyString()
    {
        cout<<"~CMyString()"<<endl;
        delete []this->m_ptr;       //  别忘了[]!!!
        this->m_ptr = nullptr;
    }

    const char * c_str() const{return m_ptr;}

private:
    friend ostream& operator<<(ostream& os, const CMyString&);
    friend CMyString operator+(const CMyString&, const CMyString&);
    char *m_ptr;
};

//  出于某种原因，在这个返回对象的函数里，我们不能返回临时对象，只能返回已经定义好的对象
//  并且出于某种原因，我们也不能在调用函数处用初始化的方式接收函数返回值
CMyString GetString(CMyString &str)
{
    const char *p = str.c_str();
    CMyString tmpStr(p);        //  显示构造
    return tmpStr;
//  tmpStr对象 -> 为了获取该对象值，需在main栈帧上**右值拷贝构造**来构造临时对象 ---(然后就析构)--> main栈帧上的临时对象再**右值赋值**给str2
}

//  CMyString operator+(const CMyString &,const CMyString&)
CMyString operator+(const CMyString& str1, const CMyString& str2)
{
    CMyString tmpStr;
    tmpStr.m_ptr = new char[strlen(str1.m_ptr) + strlen(str2.m_ptr) + 1]; //  +1 '\0'
    strcpy(tmpStr.m_ptr, str1.m_ptr);
    strcat(tmpStr.m_ptr, str2.m_ptr);   //  strcat会自动在末尾加'\0'
    return tmpStr;
    //  返回的tmpStr是个即将被销毁的局部对象 是一个右值。!!!
    //  编译器将把他匹配给 右值&&引用的参数列表
    //  因此在构造main栈帧的临时对象时会调用右值拷贝构造。即 CMyString(CMyString&&)
}

ostream& operator<<(ostream& os, const CMyString& str)
{
    os << str.c_str();
    return os;
}
int main()
{
    {
        CMyString str1("aaaaaaaaaaaaa");
        CMyString str2;
        str2 = GetString(str1);
        cout<<str2.c_str()<<endl;
    }
    cout<<"=============="<<endl;
    {
        CMyString s1 = "hello ";
        CMyString s2 = "World";
        CMyString s3 = s1 + s2;     //  CMyString
        cout << s3 << endl;
    }
    return 0;
}


// CMyString(const char*)
// CMyString(const char*)
// CMyString(const char*)
// CMyString& operator=(CMyString&&) 
// ~CMyString()
// aaaaaaaaaaaaa
// ~CMyString()
// ~CMyString()
// ==============
// CMyString(const char*)
// CMyString(const char*)
// CMyString(const char*)
// hello World
// ~CMyString()
// ~CMyString()
// ~CMyString()