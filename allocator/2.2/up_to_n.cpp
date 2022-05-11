
#include <iostream>
using namespace std;
int main(){
    int n,num,ans;
cout<<"Please enter the num(num = 2^int) you want to up to:"<<endl;
cin>>n;
cout<<"Enter your num to up to:"<<endl;
cin>>num;
ans = ((num+n-1)&~(n-1));
cout<<"The "<<num<<" up to "<<n<<" is ";
cout<<ans<<endl;
return 0;
}