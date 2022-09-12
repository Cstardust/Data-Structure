#include<iostream>
#include<string>
#include"md5.h"
#include<list>
#include<set>
#include<algorithm>
#include<vector>
#include<map>

using std::cout;
using std::endl;
using std::string;
using std::list;
using std::set;     //  红黑树 有序 适用于排序
using std::vector;
using std::map;
/*
 * 一致性哈希：
 *  虚拟节点、物理节点、哈希环
 *  哈希环上只有虚拟节点即可
 *
 *  一致性哈希一共有两次哈希
 *      1. 将虚拟节点(IP) hash 到 哈希环 上
 *      2. 将client IP hash  到 哈希环 上
 *      本代码中选用MD5哈希函数
 */
# if 1
class PhysicalHost;
class VirtualHost;

//  物理主机 真实节点 不存在于hash环上，因此不需要存储md5_值
//  存储ip值 用于根据IP寻找物理host
//  存储虚拟节点列表    用于根据物理host得到虚拟节点并遍历
    //  Physical Host 负责创建 Virtual Host
    //  Virtual Host指向其映射到的Physical Host
class PhysicalHost
{
public:
    PhysicalHost(const string &server_ip,int virtual_host_number=10)
        :server_ip_(server_ip)
    {
        for(int i=0;i<virtual_host_number;++i)
        {
            virtualHosts_.emplace_back(
                    server_ip_ + "#" + std::to_string(i),       //  virtualHost MD5
                    this);
        }
    }

    string getIp() const
    {
        return server_ip_;
    }

    const list<VirtualHost>& getVirtualHost() const
    {
        return virtualHosts_;
    }
private:
    string server_ip_;                             //  store ip for find
    list<VirtualHost> virtualHosts_;
};


//  虚拟节点占据哈希环上的一个个位置，映射到各自的主机节点
//  虚拟节点占据的位置就是md5
class VirtualHost
{
public:
    VirtualHost(const string &virtual_ip,const PhysicalHost* ptr)
            :ptr_(ptr),
            md5_(::getMD5(virtual_ip.c_str()))
    {}

    //  set 红黑树排序
    bool operator<(const VirtualHost& host_b) const
    {
        return md5_ < host_b.md5_;
    }
    //  find算法 ==
    bool operator==(const VirtualHost& host_b) const
    {
        return md5_ == host_b.md5_;
    }

    uint32_t getMD5() const
    {
        return md5_;
    }

    const PhysicalHost* getPhysical() const
    {
        return ptr_;
    }
private:
    const PhysicalHost *ptr_;       //  virtual -> physical
    uint32_t md5_;                  //  store md5_ for traverse
};

//  一致性哈希
    //  哈希环是一个uint_32的空间
    //  哈希环上面是一堆虚拟节点
    //  根据传入的client_IP将client分发到每个
class ConsistentHash
{
public:
    ConsistentHash() = default;
    //  添加真实节点（物理server）Physical Host
        //  通过将physical host对应的virtual host加入hash环
    void addHost(PhysicalHost& physical_Host)
    {
        const auto & virtual_hosts = physical_Host.getVirtualHost();
        for(const auto& host : virtual_hosts)
        {
            hash_circles_.insert(host);
        }
    }
    //  删除Physical Host
        //  通过将physical Host对应的virtual Host从hash环中删除
    void delHost(PhysicalHost& physical_host)
    {
        const auto & virtual_hosts = physical_host.getVirtualHost();
        for(const auto& host : virtual_hosts)
        {
            set<VirtualHost>::iterator iter = hash_circles_.find(host);
            if(iter == hash_circles_.end()) continue;
            hash_circles_.erase(iter);
        }
    }

    //  **负载均衡函数！！**
    //  将传入的client 负载均衡到 哈希环上的节点
    //  将client的IP映射到哈希环上的虚拟节点Virtual Host上，然后根据虚拟节点找到对应的真实节点Physical Host 并返回Physical Host的IP
    //  返回client_ip 应当负载到 的physical server
    //  这个函数 在找到对应的physical server 之后 并没有进行相应的存储操作或者连接。因为我们本身也没实现server
        //  所以找到physical server之后，仅仅以返回physical的IP作为业务操作
    string mapClientToHost(const string& client_ip) const
    {
        uint32_t idx = ::getMD5(client_ip.c_str());
        //  在hashCircle上 顺时针旋转 直到遇到一个Virtual Host
            //  感觉可以用用二分查找优化。不过set不支持[]
        for(const auto &host:hash_circles_)
        {
            if(host.getMD5() > idx)
            {
                //  virtual -> getPhysical 获取真实节点 -> getIP 真实节点返回IP
                return host.getPhysical()->getIp();
            }
        }
        throw "Consistent Hash Internal error";
    }
private:
    set<VirtualHost> hash_circles_;     //  hash circle 哈希环。上面是virtual host节点。根据md5值进行排序
};


void loadBalancing(ConsistentHash & hashCircle);

int main()
{
    //  0. 一致性哈希
    ConsistentHash hashCircle;
    //  1. 根据物理节点 创建 Virtual Host 虚拟节点，将虚拟节点映射到哈希环上
        //  physical Host <---> virtual Host ---hash---> hash circle
    PhysicalHost physicalHost_1("10.117.124.10",150);
    PhysicalHost physicalHost_2("10.117.124.20",150);
    PhysicalHost physicalHost_3("10.117.124.30",150);

    //  2. 将physical节点以及其virtual节点加入哈希环
    hashCircle.addHost(physicalHost_1);
    hashCircle.addHost(physicalHost_2);
    hashCircle.addHost(physicalHost_3);

    //  3. 开始负载均衡。
        //  将client 映射到 哈希环上的server节点上
        //  client --hash--> virtual Host ---> physical Host
    loadBalancing(hashCircle);

    cout<<"******************************"<<endl;
    cout<<"physicalHost_1 crashed"<<endl;
    cout<<"******************************"<<endl;
    //  删除一个host1之后
        //  其数据负载均衡到host2 和 host3
    hashCircle.delHost(physicalHost_1);

    loadBalancing(hashCircle);
}

void loadBalancing(ConsistentHash & hashCircle)
{
    //  client_ip
    vector<string> vec{
            "192.168.1.123",
            "192.168.1.12",
            "192.168.1.13",
            "192.168.1.23",
            "192.168.1.54",
            "192.168.1.89",
            "192.168.1.21",
            "192.168.1.27",
            "192.168.1.49",
            "192.168.1.145",
            "192.168.2.34",
            "192.168.6.78",
            "192.168.2.90",
            "192.168.4.5"
    };

    //  physical    clients
    map<string,vector<string>> logMap;  //  client to physical server 的映射日志
    for(int i=0;i<vec.size();++i)
    {
        auto & client_ip = vec[i];
        string server_ip = hashCircle.mapClientToHost(client_ip);
        logMap[server_ip].emplace_back(client_ip);
    }

    for(const auto & item : logMap)
    {
        cout<<"server : "<<item.first<<endl;
        cout<<"client : "<<item.second.size()<<endl;
        for(auto & cliet:item.second)
        {
            cout<<cliet<<endl;
        }
        cout<<"=========================================="<<endl;
    }
}

#endif


//int main()
//{
//    cout<<MD5("1110")<<endl;
//    cout<<MD5("1111")<<endl;
//    cout<<MD5("192.168.1.72#1")<<endl;
//    cout<<MD5("192.168.1.72#2")<<endl;
//
//    cout<<getMD5("1110")<<endl;
//    cout<<getMD5("1111")<<endl;
//    cout<<getMD5("192.168.1.72#1")<<endl;
//    cout<<getMD5("192.168.1.72#2")<<endl;
//}