// file:4heap-test.cpp
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int main()
{

    {
        // test heap(底层以vector完成)
        int ia[9] = {0, 1, 2, 3,4, 8, 9, 3, 5};
        vector<int> ivec(ia, ia + 9);

        make_heap(ivec.begin(), ivec.end());
        for (int i = 0; i < ivec.size(); ++i)
            cout << ivec[i] << ' ';
        cout << endl;

        ivec.push_back(7);
        push_heap(ivec.begin(), ivec.end());
        for (int i = 0; i < ivec.size(); ++i)
            cout << ivec[i] << ' ';
        cout << endl;

        push_heap(ivec.begin(), ivec.end());
        cout << ivec.back() << endl;
        ivec.pop_back();

        for (int i = 0; i < ivec.size(); ++i)
            cout << ivec[i] << ' ';
        cout << endl;

        sort_heap(ivec.begin(),ivec.end());
        for (int i = 0; i < ivec.size(); ++i)
            cout << ivec[i] << ' ';
        cout << endl;
    }
    {
        //test heap(底层以array完成)
        int ia[9] = {0, 1, 2, 3, 4,8, 9, 3, 5};
        make_heap(ia,ia+9);
        //array无法动态改变大小，因此不可以对满载的array进行push_heap()操作,
        //因为那得先在array尾端增加一个元素，如果对一个满载的array执行push_heap()
        //该函数会将最后一个元素视为新增元素，并将其余元素视为一个完整的heap结构，
        //因此执行后的的结果等于原先的heap

        sort_heap(ia,ia+9);
        for(int i=0;i<9;++i)
        cout<<ia[i]<<' ';
        cout<<endl;
        //经过排序后的heap不再是一个合法的heap
        //array排序了，但堆特性不符合了

        //重新做一个heap
        make_heap(ia,ia+9);
        pop_heap(ia,ia+9);
        cout<<ia[8]<<endl;
    }

    {
        //test heap(底层以array完成)
        int ia[6]={4,1,7,6,2,5};
        make_heap(ia,ia+6);
        for(int i=0;i<6;++i)
        cout<<ia[i]<<' ';
        cout<<endl;
    }
    return 0;
}