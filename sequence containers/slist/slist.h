#include "slist_iterator.h"
template <class T, class Alloc>
class slist
{
public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef __list_iterator<T, T &, T *> iterator;
    typedef __list_iterator<T, const T &, const T *> const_iterator;

private:
    typedef __slist_node<T> list_node;
    typedef __slist_node_base list_node_base;
    typedef __slist_iterator_base iterator_base;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;

    static list_node *create_node(const value_type &x)
    {
        list_node *node = list_node_allocator::allocate(); //配置空间
        __STL_TRY
        {
            construct(&node->data, x); //构造元素
            node->next = 0;            //
        }
        __STL_UNWIND(list_node_allocator::deallocate(node));
        return node;
    }

    static void destroy_node(list_node *node)
    {
        destroy(&node->data);                  //析构元素
        list_node_allocator::deallocate(node); //释放空间
    }

private:
    list_node_base head; //不是指针，是实例
public:
    slist() { head.next = 0; }
    ~slist() { clear(); }

public:
iterator begin(){return iterator((list_node*)head.next);}
iteraotr end(){return iteraotr(0);}
size_type size()const{return __slist_size(head.next);}
bool empty()const{return head.next==0;}
// 两个slist互换：只要将head交换互指即可
void swap(slist& L){
    list_node_base* tmp=head.next;
    head.next=L.head.next;
    L.head.next=tmp;
}

public:
//获取头部元素
reference front(){return ((list_node*)head.next>)->data;}
//从头部插入元素 （新元素成为slist第一个元素）
void push_front(const value_type& x){
    __slist_make_link(&head,create_node(x));
}

//注意：没有push_back()
// 从头部取走元素，删除，修改head
void pop_front(){
    list_node* node=(list_node*)head.next;
    head.next=node->next;
    destroy_node(node);
}
};