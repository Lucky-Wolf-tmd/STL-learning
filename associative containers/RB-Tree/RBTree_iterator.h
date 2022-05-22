#include "RBTree_node.h"
struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_categoty;
    typedef ptrdiff_t difference_type;

    base_ptr node; //它用来与容器之间产生一个连接关系（make a reference）

    //以下可以直接实现于operator++内，因为再无他处调用
    void increment()
    {
        if (node->right != 0)
        {                           //如果有Right Child
            node = node->right;     //往右走
            while (node->left != 0) //然后一直往左子树走到底
                node = node->left;  //即为递增
        }
        else
        {                              //如果没有Right Child
            base_ptr y = node->parent; //找到父节点
            while (node == y->right)   //如果当前节点就是一个右子节点
            {
                node = y; //就一直上溯，直到不是右子节点为止
                y = y->parent;
            }
            if (node->right != y) //若此时Right Child节点不是此时的父节点
                node = y;         //此时父节点即为解答
                                  //否则此时的node即为解答
        }
        /*
        注意,以上判断“若此时的右子节点不等于此时的父节点”，是为了应付一种
        特殊情况，我们欲寻找根节点的下一节点，而恰巧根几点无右子节点，
        当然，以上特殊情况必须配合RB-tree根节点与特殊的header之间的特殊关系
        */
    }

    //以下可以直接实现于operator--内，因为再无他处调用
    void decrement()
    {
        if (node->color == __rb_tree_red && //如果是红节点，且
            node->parent->parent == node)   //父节点的父节点等于自己
            node = node->right;             //右节点即为解答
        //以上情况发生于node为header时（也是node为end()时）
        //注意header的右子节点即mostright，指向整棵树的max节点
        else if (node->left != 0) //如果有Left Child
        {
            base_ptr y = node->left; // y指向左子节点
            while (y->right != 0)    //当y有右子节点时
                y = y->right;        //一直往右子节点走到底
            node = y;                //最后即为答案
        }
        else
        {                              //既非根节点，亦无左子节点
            base_ptr y = node->parent; //找出父节点
            while (node == y->left)
            {                  //当当前节点为左子节点时
                node = y;      //一直交替往上走，直到现行节点
                y = y->parent; //不是左子节点
            }
            node = y;
        }
    }
};

// RB-Tree 的正规迭代器
template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef __rb_tree_iterator<Value, Value &, Value *> iterator;
    typedef __rb_tree_iterator<Value, const Value &, const Value *> const_iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr> self;
    typedef __rb_tree_node<Value> *link_type;

    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator &it) { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    opinter operator->const
    {
        return &(operator*());
    }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

    self &operator++()
    {
        increment();
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        increment();
        return tmp;
    }

    self &operator--()
    {
        decrement();
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        decrement();
        return tmp;
    }
};