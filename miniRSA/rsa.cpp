#include <iostream>
#include "Logger.h"
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include<cassert>
#include<random>

using namespace std;
using ULL = unsigned long long;

//  均匀分布 利用/dev/random 硬件产生真随机数
int getUniformDistributionInt(int minNum,int maxNum)
{
    static random_device seed;                                 // 硬件生成随机数种子
    ranlux48 engine(seed());                                // 利用种子生成随机数引擎
    uniform_int_distribution<int> distrib(minNum, maxNum);  //  设置随机数范围，并为均匀分布
    return distrib(engine);                                 //  随机数
}


int64_t gcd(int a, int b)
{
    return b ? gcd(b, a % b) : a;
}

int extendGcd(int a, int b, int &x, int &y)
{
    if (!b)
    {
        //  ax + by = gcd(a,b) = 1;
        x = 1;
        y = 0;
        //  return 最大公约数
        return a;
    }
    //  else
    int g = extendGcd(b, a % b, y, x);
    //  易知当前层的x y 与 下一层的x1 y1的关系是
    //  : x = x1 ; y = y1 - (a/b)*x
    //  即x不用变 y需要-
    y = y - a / b * x;
    return g;
}

//  快速计算 a ^ k mod p
ULL quickM(ULL a, int k, int p)
{
    ULL res = 1;
    //  k = 2^0 + 2^1 + ....
    //  a^k = a^(2^0) * a^(2^1) * ....
    while (k)
    {
        //  可能会溢出 必须保证 res*a <= 64bit
        if (k & 1)
            res = res * a % p;
        k >>= 1;
        a = a * a % p;
    }
    return res;
}


static int checks[] = {2,3,5,7,11,13,17,19,23,29};
//  利用费马小定理和二次探测 检测素数。概率性算法。
bool millerRabin(ULL num)
{
    //  a. 特判0 1 2
    if (num == 2)
        return true;
    if (num < 2 || !(num % 2))
        return false;

    //  b. 拆分 num-1
        //  num-1 = (2^s) * t 
    ULL s = 0;
    ULL t = num-1;
    while(!(t%2))
    {
        t >>= 1;
        ++s;
    }
    //  c. 检验是否是素数 通过二次探测和费马小定理
    //  取checks数组中的元素为a, 逐个进行检验
    for(int i=0;i<10;++i)
    {
        int a = checks[i];
        if(a >= num) continue;
        //  1. 进行s次二次探测
        //  二次探测定理：如果 p 是一个素数 , 0<x< p 则 方程 x^2=1(modp)的解为x=1或者x=p-1
            //  利用其逆否定理
            //  -> 如果 0<x<p 且 方程 x^2=1(modp)的解 不是x=1和x=p-1
            //  -> 那么p一定不是一个素数
                //  如果p没通过该探测 则 p一定是个合数
                //  如果p通过了该探测 则 p**很可能**是素数 有可能是合数
        //  反复运用 s次二次探测定理
            //  先取一个gcd(num,..) = 1 且 < num的数a 。计算出 a^t
            //  记x = a^t mod num
            //  每次都求 test = x^2 mod num 。判断test == 1.
                //  若test == 1 且 (x!=1 或者 x!=num-1) 则num不是素数 
        ULL x = quickM(a,t,num);
        for(int i=0;i<s;++i)        //  (a^t)^(2^s) = a^(2^s * t) = a^(num-1)
        {
            //  x^2 mod num   . (a^t)^2 mod num (总共做s次循环)
            ULL res_of_quadratic_probing = quickM(x,2,num);
            if(res_of_quadratic_probing == 1)
            {
                if(x!=num-1 && x!=1)
                {
                    return false;
                }
            }
            x = res_of_quadratic_probing;
        }
        //  2. 最后使用一次 费马小定理 进行监测
        //  如果p是一个质数，而且整数a与p互质（即最小公因数gcd(a,p) = 1），则有a^(p-1)≡1(modp) 
            //  利用其逆否定理
            //  如果 a与p互质 a^(p-1)!=1(modp) 那么p一定不是个质数
        if(x!=1) 
            return false;
    }
    return true;
}


//  密钥public key 和 secret key均由RSA自己生成
//  用户输入
class RSA
{
public:
    using KEY = pair<ULL, ULL>;

    RSA() : padding_(false)
    {
        //  0. initial table
        initialTable();
        //  1. initialPQ
        initialPQ();
        //  2. initialN
        initialN();
        //  3. inital EulerN
        calEulerN();
        //  4. inital E
        initialE();
        //  5. initial D
        initialD();
        //  6. inital public Key
        initialPublicKey();
        //  7. inital secret Key
        initialSecretKey();
    }

    //  设置明文
    void setPlainText(const string& plainText)
    {
        plainText_ = plainText;
        //  分组
        splitInToPlainPackets();
    }

    void setPainTextPath(const string& path)
    {
        //  从文件中读取
        int fd = open(path.c_str(),O_RDONLY);
        int len = read(fd,buf,sizeof buf);
        assert(len!=-1);
        plainText_ = buf;
        //  分组
            //  因为plainText_ > N 所以需要分组 保证每个分组一定小于N
        splitInToPlainPackets();
    }

    //  加密
    void encrypt()
    {
        cout<<"==============START ENCRYPT============="<<endl;
        for (int i = 0; i < plainPackets_.size(); ++i)
        {
            ULL r = quickM(plainPackets_[i], publicKey_.first, publicKey_.second);
            cipherPackets_.push_back(r);
        }
        cout<<"==============END ENCRYPT============="<<endl;
        //  debug
        // cout << "cipherPackets ";
        // for_each(cipherPackets_.begin(), cipherPackets_.end(), [](int x)
        //          { cout << x << " "; });
        // cout << endl;
        //  没写分组之前的
        // cipherText_ = quickM(plainText_,publicKey_.first,publicKey_.second);
    }
    //  解密
    void decrypt()
    {
        cout<<"==============START DECRYPT============="<<endl;
        for (int i = 0; i < cipherPackets_.size(); ++i)
        {
            ULL r = quickM(cipherPackets_[i], secretKey_.first, secretKey_.second);
            decryptPackets_.push_back(r);
        }
        cout<<"==============END DECRYPT============="<<endl;
        //  debug
        // cout << "decryptPackets ";
        // for_each(decryptPackets_.begin(), decryptPackets_.end(), [](int x)
        //          { cout << x << " "; });
        // cout << endl;
        //  没写分组之前的
        // decryptText_ = quickM(cipherText_,secretKey_.first,secretKey_.second);

        //  讲decryptPackets中的int 转化字符 拼接成string 存入decryptText
        retrieveAsString();
    }
    //  return 明文
    string getPlainText()
    {
        return plainText_;
    }
    //  return 密文
    string getEncyptText()
    {
        return cipherText_;
    }
    //  return 解密后的明文
    string getDecryptText()
    {
        return decryptText_;
    }

    KEY getPublicKey()
    {
        return publicKey_;
    }
    KEY getSecretKey()
    {
        return secretKey_;
    }

    void showKeyPoints()
    {
        cout<<"p :"<<p_<<"\t"<<"q :"<<q_<<endl;
        cout<<"n :"<<n_<<endl;
        cout<<"e :"<<e_<<endl;
        cout<<"d :"<<d_<<endl;

        cout<<"=================plainText below after group=============="<<endl;
        for_each(plainPackets_.begin(), plainPackets_.end(), [](int x)
                 { cout << x <<" " ; });
       cout<<endl; 
        cout<<"=================cipherText below after group============="<<endl;
        for_each(cipherPackets_.begin(), cipherPackets_.end(), [](int x)
                 { cout << x <<" " ; });
        cout << endl;

        cout<<"=================decryptText below after group============="<<endl;
        for_each(decryptPackets_.begin(), decryptPackets_.end(), [](int x)
                 { cout << x <<" " ; });
        cout << endl;

        cout<<"=================decryptText below============"<<endl;
        cout<<decryptText_<<endl;
    }

private:
    void initialTable()
    {
        for (int i = 0; i <= 9; ++i)
        {
            table_['0' + i] = i;
            inverseTable_[i] = '0' + i;
        }
        for (int i = 10; i <= 35; ++i)
        {
            table_['a' + i - 10] = i;
            inverseTable_[i] = 'a' + i - 10;
        }
        for (int i = 36; i <= 61; ++i)
        {
            table_['A' + i - 36] = i;
            inverseTable_[i] = 'A' + i - 36;
        }
    }

    void initialPQ()
    {
        int primes[2];
        int idx = 0;
        //  srand(time(nullptr))
        // int start = 1000 + rand() % 9000;
        int start = getUniformDistributionInt(1000,10000);
        
        for (int i = start;; ++i)
        {
            if (idx == 2)
                break;
            if (millerRabin(i))
            {
                primes[idx++] = i;
            }
        }
        p_ = primes[0];
        q_ = primes[1];
    }

    void initialN()
    {
        n_ = p_ * q_;
    }

    void calEulerN()
    {
        eulrN_ = (p_ - 1) * (q_ - 1);
    }

    void initialE()
    {
        for (ULL i = n_ / 2; i < eulrN_; ++i)
        {
            if (gcd(i, eulrN_) == 1)
            {
                //  public key (e,n)
                e_ = i;
                return;
            }
        }
        cerr << "never reach!" << endl;
        exit(-1);
    }

    void initialD()
    {
        int x, y;
        int g = extendGcd(e_, eulrN_, x, y);
        //  用init64_t 不能用ull
        //  因为如果用ull x%t的x会先转化成ull
        int64_t t = eulrN_ / g;
        d_ = (x % t + t) % t;
    }

    void initialPublicKey()
    {
        publicKey_.first = e_;
        publicKey_.second = n_;
    }

    void initialSecretKey()
    {
        secretKey_.first = d_;
        secretKey_.second = n_;
    }

    //  从i开始 找到第一个是字母或者数字的下标（包括i）
    //  [i,size())
    void skipNonNumOrLetter(int &i)
    {
        while (i < plainText_.size() && (!(isdigit(plainText_[i]) || (isalpha(plainText_[i])))))
            ++i;
    }

    //  每个分组的值m 一定要小于n = 40870433
    //  不然RSA失效
    void splitInToPlainPackets()
    {
        int idx = 0;
        while (idx < plainText_.size())
        {
            //  找到一个字母或者数字
            skipNonNumOrLetter(idx);
            if (idx == plainText_.size())
                break;
            int packet = table_[plainText_[idx]] * 100;
            ++idx;

            //  找到下一个不是? . 的位置
            skipNonNumOrLetter(idx);
            //  没有了 填充80
            if (idx == plainText_.size())
            {
                padding_ = true;
                packet += 80;
            }
            //  还有 加上即可
            else
            {
                packet += table_[plainText_[idx]];
            }
            //  合并完 加入packets
            plainPackets_.push_back(packet);
            //  继续向下遍历
            ++idx;
        }
        //  debug
        // cout << "plainPacket ";
        // for_each(plainPackets_.begin(), plainPackets_.end(), [](int x)
        //          { cout << x << " "; });
        // cout << endl;
    }

    void retrieveAsString()
    {
        for (int i = 0; i < decryptPackets_.size(); ++i)
        {
            int x = decryptPackets_[i];
            if (padding_ && i == decryptPackets_.size() - 1)
            {
                x -= 80; //  无用
                x /= 100;
                decryptText_.push_back(inverseTable_[x]);
            }
            else
            {
                decryptText_.push_back(inverseTable_[x / 100]);
                decryptText_.push_back(inverseTable_[x % 100]);
            }
        }
    }

private:
    int p_, q_;                 //  随机选取的两个大素数
    ULL n_;                     //  n = p*q
    ULL eulrN_;                 //  fai n = (p-1)*(q-1)
    ULL e_;                     //  gcd(e,eurlN) = 1 ; public key (e,n)
    ULL d_;                     //  de mod eurlN = 1 ; secret key (d,n)
    KEY publicKey_, secretKey_; //  public key (e,n) ; secret key (d,n)
    string plainText_;          //  明文
    string cipherText_;         //  密文     //  分组之后 没用了 都存在cipherPackets里面了
    string decryptText_;        //  解密后

    vector<int> plainPackets_;   //  明文分组       加密方的明文 
    vector<int> cipherPackets_;  //  密文分组       加密方讲一个明文分组加密后存入一个密文分组 因此明文和密文分组对应. 并没有采用指导书上的 密文全在一起 然后传输给对方，对方再分组。直接用一个json把存有密文的vector发给对方这不香吗？
    vector<int> decryptPackets_; //  解密分组       解密方对传送过来的cipherPackets解密后的明文

    unordered_map<char, int> table_;
    unordered_map<int, char> inverseTable_;
    bool padding_; //  明文是否填充 现假定用80填充
    char buf[4096] = {0};


    //  分组方法：一个明文组 加密之后 放入 一个密文组 。讲密文组vector交给解密方，解密方进行解密
    //  为什么要分组 ？ 因为明文m的大小不可以大于n。要分组到每个分组的m都小于<n
    //  为什么m不可以大于n?
        //  加密公式： c = m ^ e mod n
        //  解密公式： m = c ^ d mod n
        //  如果m>n的话 那么 无论怎么加密解密 
        //  解密的时候需要modn 
        //  最后得到的一定是一个比n小的数m' . 而已知m>n ，因此 得到的m'一定不是m！！故m不可以>n
        //  此时得到的数 是一个和 m mod n之后同余的数。
        //  那么 我们可以用m'不断+n 最后会在某一刻得到m
};



int main()
{
    RSA rsa;
    rsa.setPainTextPath("./plainText.txt");
    rsa.encrypt();
    rsa.decrypt();
    rsa.showKeyPoints();
    return 0;
}
