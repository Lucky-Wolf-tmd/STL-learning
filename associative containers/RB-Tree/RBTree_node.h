typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black=true;

struct __rb_tree_node_base{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;  //颜色节点，非黑即红
    base_ptr parent;   //RBTree的许多操作，必须直到父节点
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x){//x节点下的最小值
        while(x->left!=0)x=x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x){//x节点下的最大值
        while(x->right!=0)x=x->right;
        return x;
    }
};

template<class Value>
struct __rb_tree_node:public __rb_tree_node_base
{
    typedef __rb_tree_node<Value>* link_type;
    Value value_field;//节点值
}