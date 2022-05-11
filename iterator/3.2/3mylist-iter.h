// file: 3mylist-iter.h
#include"3mylist.h"

template<class Item>  // Item can be a single or double list node.
struct ListIter{      // the iterator here only serve for list, because of its unique operator++
    Item* ptr;        // keep a reference to Container

    ListIter(Item* p = 0) //defaut ctor
        : ptr(p){}

    // no ncs to implement copy ctor ,cause the default action provide by compiler do enough;
    // no ncs to implement operator= ,cause the default action provide by compiler do enough;

    Item& operator*() const{return *ptr;}
    Item* operator->() const{return ptr;}

    // two following operator++ follows standard method, See [Meyers96] TERM 6
    //(1) pre-increment operator
    ListIter& operator++()
    {ptr = ptr->next();return *this;}
    //(2) post-increment operator
    ListIter operator++(int)
    {ListIter tmp = *this; ++*this; return tmp;}

    bool operator==(const ListIter& i)const
    {return ptr == i.ptr;}
    bool operator!=(const ListIter& i)const
    {return ptr!= i.ptr;}
};