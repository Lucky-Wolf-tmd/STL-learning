//filename: 4deque-test.cpp
#include<deque>
#include<iostream>
#include<algorithm>
using namespace std;

int main()
{
    deque<int> ideq(20,9);//注意：alloc只适用于G++
    cout<<"size="<<ideq.size()<<endl;
    //现在，应该已经构造了一个deque，有20个int元素，初值皆为9
    //缓冲区大小为32bytes

    //为每一个元素设定新值
    for(int i=0;i<ideq.size();++i) ideq[i]=i;
    for(int i=0;i<ideq.size();++i) cout<<ideq[i]<<' '; cout<<endl;
   

    //在最尾端增加三个元素
    for(int i=0;i<3;++i)ideq.push_back(i);
    for(int i =0;i<ideq.size();++i)cout<<ideq[i]<<' '; cout<<endl;
    cout<<"size="<<ideq.size()<<endl;

    //在最尾端再增加一个元素，其值为3
    ideq.push_back(3);
    for(int i =0;i<ideq.size();++i)cout<<ideq[i]<<' '; cout<<endl;
    cout<<"size="<<ideq.size()<<endl;

    //在最前端再增加一个元素，其值为99
    ideq.push_back(99);
    for(int i =0;i<ideq.size();++i)cout<<ideq[i]<<' '; cout<<endl;
    cout<<"size="<<ideq.size()<<endl;

     //在最前端再增加2个元素，其值为98,97
    ideq.push_back(98);
    ideq.push_back(97);
    for(int i =0;i<ideq.size();++i)cout<<ideq[i]<<' '; cout<<endl;
    cout<<"size="<<ideq.size()<<endl;

    //搜寻数值为99的元素并打印出来
    auto itr=find(ideq.begin(),ideq.end(),99);
    cout<<*itr<<endl;
    cout<<*itr._M_cur<<endl;

    return 0;
}