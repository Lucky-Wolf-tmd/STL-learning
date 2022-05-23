#pragma once
#include "RBTree_iterator.h"

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
class rb_tree
{
protected:
    typedef void *void_pointer;
    typedef __rb_tree_node_base *base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<__rb_tree_node, Alloc> rb_tree_node_allocator;
    typedef __rb_tree_color_type color_type;

public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef __rb_tree_node *link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type &x)
    {
        link_type tmp = get_node(); //配置空间
        __STL_TRY
        {
            construct(&tmp->value_field, x); //构造内容
        }
        __STL_UNWIND(put_node(tmp));
        return tmp;
    }

    link_type clone_node(link_type x)
    { //复制一个节点的值和色
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    void destroy_node(link_type p)
    {
        destroy(&p->value_field);
        put_node(p);
    }

protected:
    // RB-Tree 只以三笔数据表现
    size_type node_count; //追踪记录树的大小（节点数量）
    link_type header;     //实现上的一个小技巧（头指针，链类结构的实现tip）
    Compare key_compare;  //节点间键值的大小比较标准，应该会是一个function object仿函数

    //以下三个函数方便用于获得header 的成员
    link_type &root() const { return (link_type &)header->parent; }
    link_type &leftmost() const { return (link_type &)header->left; }
    link_type &rightmost() const { return (link_type &)header->right; }

    //以下6个函数用来方便获得节点x的成员
    static link_type &left(link_type x) { return (link_type &)(x->left); }
    static link_type &right(link_type x) { return (link_type &)(x->right); }
    static link_type &parent(link_type x) { return (link_type &)(x->parent); }
    static reference value(link_type x) { return x->value_field; }
    static const Key &key(link_type x) { return KeyOfValue()(value(x)); }
    static color_type &color(link_type x) { return (color_type &)(x->color); }

    //以下6个函数用来方便获取节点x的成员
    static link_type &left(base_ptr x) { return (link_type &)(x->left); }
    static link_type &right(base_ptr x) { return (link_type &)(x->right); }
    static link_type &parent(base_ptr x) { return (link_type &)(x->parent); }
    static reference value(base_ptr x) { return ((link_type)x)->value_field; }
    static const Key &key(base_ptr x) { return KeyOfValue()(value(link_type(x))); }
    static color_type &color(base_ptr x) { return (color_type &)(link_type(x)->color); }

    //求取极大值和极小值，node class 有实现此功能，交给他们完成即可
    static link_type minimum(link_type x) { return (link_type)__rb_tree_node_base::minimum(x); }
    static link_type maximum(link_type x) { return (link_type)__rb_tree_node_base::maximum(x); }

public:
    typedef __rb_tree_base_iterator<value_type, reference, pointer> iterator;

private:
    iterator __insert(base_ptr x, base_ptr y, const value_type &v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void init()
    {
        header = get_node();           //产生一个节点空间，令header指向它
        color(header) = __rb_tree_red; //令header为红色，用来区分header和root，在iterator.operator--之中
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
    }

    public:
                                        //allocation deallocation
    rb_tree(const Compare& comp=Compare()):node_count(0),key_compare(comp){init();}
    ~rb_tree(){clear();put_node(header);}
    rb_tree<Key,Value,KeyOfValue,Compare,Alloc>&
    operator=(const rb_tree<Key,Value,KeyOfValue,Compare,Alloc>& x);

    public:
                                        //accessors
    Compare key_comp()const{return key_compare;}
    iterator begin(){return leftmost();} //起点为最左最小节点 
    iterator end(){return header;}       //终点为header所指处
    bool empty() const {return node_count == 0;}
    size_type size() const{return node_count;}
    size_type max_size() const{return size_type(-1);}

    public:
                                        //insert erase
    pair<iterator,bool> insert_unique(const value_type& x);
    iterator insert_equal(const value_type& x);
    //...
};