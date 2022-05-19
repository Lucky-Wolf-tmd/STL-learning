#include<vector>
#include<functional>
using namespace std;
template<class T,class Sequence=vector<T>,
class Compare=less<typename Sequence::value_type>>