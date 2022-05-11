// file: 3tag-test.cpp
// simulate test to the influence of inherit relationship

#include<iostream>
using namespace std;

struct B{};             //simulate to InputIterator
struct D1:public B{};   //simulate to ForwardIterator
struct D2:public D1{};   //simulate to BidirectionalIterator

template<class I>
void func(I&p,B){cout<<"B version"<<endl;}
template<class I>
void func(I&p,D2){cout<<"D2 version"<<endl;}

int main(){
    int *p;
    func(p,B()); // parameters fit. 
    func(p,D1());// paremeters don't fit, forwarding-invoke by inherit relationship
    func(p,D2());// parameters fit.
}

