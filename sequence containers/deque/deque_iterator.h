#include<stddef.h>
template<class T,class Ref,class Ptr,size_t BufSiz>
struct __deque_iterator{
    typedef __deque_iterator<T,T&,T*,BufSize> iterator;
    typedef __deque_iterator<T,const T&,const T*,BufSiz> const_iterator;
    static size_t buffer_size(){return __deque_iterator(BufSize,sizeof(T));}

    typedef random_access_iterator_tag iterator_category; //(1)
    typedef T value_type;                                 //(2)
    typedef Ptr pointer;                                  //(3)
    typedef Ref reference;                                //(4)
    typedef size_t size_type;                     
    typedef ptrdiff_t difference_type;                    //(5)
    typedef T** map_pointer; 

    typedef __deque_iterator self;

    // keep connect to container
    T* cur;  // the current element in the buffer pointed by this iterator 
    T* first;// the first in the buffer pointed by this iterator
    T* last; // the last in the buffer pointed by this iterator
    map_pointer node; // point to control centre

    // if n isn't 0, return n, means buffer size define by user.
    // if n is 0, means buffer size is default value, so that
    //   if sz(element size, sizeof(value_type))less than 512, return 512/sz
    //   if sz not less than 512, return 1

    inline size_t __deque_buf_size(size_t n,size_t sz){
        return n!=0?n:(sz<512?size_t(512/sz):size_t(1));
    } 

    void set_node(map_pointer new_node){
        node = new_node;
        first = *new_node;
        last = first+difference_type(buffer_size());
    }

    reference operator*() const {return *cur;}
    pointer operator->() const {return &(operator*());}
    difference_type operator-(const self& x)const {
        return difference_type(buffer_size())*(node-x.node-1)+(cur-first)+(x.last-x.cur);
    }

    // refer to More Effective C++, item6: Distinguish between prefix and postfix forms of increment and decrement operators
    self& operator++(){
        ++cur;
        if(cur==last){
            set_node(node+1);
            cur =first;
        }
        return *this;
    }
    self operator++(int){ //standard postfix
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--(){
        if(cur==first){
            set_node(node-1);
            cur=last;
        }
        --cur;
        return *this;
    }
    self operator--(int){
        self tmp=*this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n){
        difference_type offset=n+(cur+first);
        if(offset>=0&&offset<difference_type(buffer_size()))
        // target position in the same buffer
        cur+=n;
        else{
            //target position is not in the same buffer
            difference_type node_offset =
             offset>0?offset/difference_type(buffer_size())
             :-difference_type((-offset-1)/buffer_size())-1;
            // set to right node(also buffer)
            set_node(node+node_offset);
            // set to right element
            cur = first+(offset-node_offset*difference_type(buffer_size()));
        }
        return *this;
    }

    // refer to More Effective C++, item22: Consider using op= instead of
    // stand-alone op
    self operator+(difference_type n)const{
        self tmp=*this;
        return tmp+=n;// invoke operator+=
    }

    self& operator-=(difference_type n){return *this+= -n;}
    // aboves using operator+= to finish operator-=

    // refer to More Effective C++, item22: Consider using op= instead of
    // stand-alone op
    self operator-(difference_type n)const{
        self tmp=*this;
        return tmp-=n; //invoke operator-=
    }

    //followings implement random I/O, iterator can jump n difference
    reference operator[](difference_type n)const {return *(*this+n);}
    //aboves invoke operator* operator+

    bool operator==(const self& x)const{return cur==x.cur;}
    bool operator!=(const self& x)const{return !(*this==x);}
    bool operator<(const self& x)const {
        return (node==x.node)?(cur<x.cur):(node<x.node);
    }
};