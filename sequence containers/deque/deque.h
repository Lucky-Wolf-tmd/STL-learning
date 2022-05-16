#include "deque_iterator.h"
// why BufSize default value is 0,the only reason is avoid same compiler's bug in constant expressions
// default using alloc as allocator
template <class T, class Alloc, size_t BufSize = 0>
class deque
{
public: // Basic types
    typedef T value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef size_t size_type;

public: // Iterators
    typedef __deque_iterator<T, T &, T *, BufSize> iterator;

protected:
    //元素的指针的指针（pointer of pointer T）
    typedef pointer *map_pointer;

protected:           // Data members
    iterator start;  //表征第一个节点
    iterator finish; //表征最后一个节点

    map_pointer map;    //指向map， map是块连续空间
                        //其每个元素都是一个指针，指向一个缓冲区
    size_type map_size; // map内有多个指针

public: // Basic accessors
    iterator begin() { return start; }
    iterator end() { return finish; }

    reference operator[](size_type n)
    {
        return start[difference_type(n)]; // invoke __deque_iterator<>::operator[]
    }
    reference front() { return *start; } // invoke __deque_iterator<>::operator*
    reference back()
    {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    size_type size() const { return finish - start; }
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return finish == start; }

protected: // Internal typedefs
    //专属的空间配置器，每次配置一个元素大小
    typedef simple_alloc<value_type, Alloc> data_allocator;
    //专属的空间配置器，每次配置一个指针大小
    typedef simple_alloc<pointer, Alloc> map_allocator;

    deque(int n, const value_type &value)
        : start(), finish(), map(0), map_size(0)
    {
        fill_initialize(n, value);
    }

    template <class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type &value)
    {
        create_map_and_nodes(n); //把deque的结构都产生并安排好
        map_pointer cur;
        __STL_TRY
        {
            //为每个节点的缓冲区设定初值
            for (cur = start.node; cur < finish.node; ++cur)
                uninitialized_fill(*cur, *cur + buffer_size(), value);
            //最后一个节点的设定稍有不同（因为尾端可能有备用空间，不必设初值）
            uninitialized_fill(finish.first, finish.cur, value);
        }
        catch (...)
        {
            //...
        }
    }

    template <class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements)
    {
        //需要节点数=（元素个数/每个缓冲区可容纳的元素个数）+1
        //如果刚好整除，会多配置一个节点
        size_type num_nodes = num_elements / buffer_size() + 1;

        //一个map要管理几个节点。最少8个，最多是“所需节点数加2”
        //前后各预留一个，扩充时可用
        map_size = max(initial_map_size(), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        //以上配置出一个"具有map_size个节点“的map

        //以下令nstart和nfinish指向map所拥有的全部节点的最中央区段
        //保持在最中央，可以使头尾两端的扩充量一样大。每个节点可以个对应一个缓冲区
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;

        map_pointer cur;
        __STL_TRY
        {
            //为map内的每个现用节点配置缓冲区，所有缓冲区加起来就是deque的
            //可用空间（最后一个缓冲区可能留有余裕）
            for (cur = nstart; cur <= nfinish; ++cur)
                *cur = allocate_node();
        }
        catch (...)
        {
            //"commit or rollback"语义：若非全部成功，就一个不留
        }
        //为deque内的两个迭代器设定正确内容
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first; // first, cur都是public
        finish.cur = finish.first + num_element % buffer_size();
    }

public:
    void push_back(const value_type &t)
    {
        if (finish.cur != finish.last - 1)
        {
            //最后一个缓冲区含两个以上的元素备用空间
            construct(finish.cur, t); //直接在备用空间上构造函数
            ++finish.cur;             //调整最后缓冲区的使用状态
        }
        else //最后缓冲区只剩一个元素备用空间
            push_back_aux(t);
    }

    //由于尾端只剩一个元素备用空间，于是push_back()调用push_back_aux()，
    //先配置一整块新的缓冲区，只剩一个备用空间时才会被调用
    template <class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back_aux(const value_type &t)
    {
        value_type t_copy = t;
        reserve_map_at_back();                //若符合某种条件，则必须重换一个map
        *(finish.node + 1) = allocate_node(); //配置一个新节点
        __STL_TRY
        {
            construct(finish.cur, t_copy);    //针对标的元素设值
            finish.set_node(finish.node + 1); //改变finish，令其指向新节点
            finish.cur = finish.fist;         //设定finish的状态
        }
        __STL_UNWIND(deallocate_node(*(finish.node + 1)));
    }

public:
    void push_front(const value_type &t)
    {
        if (start.cur != start.first)
        {                                //第一缓冲区尚有备用空间
            construct(start.cur - 1, t); //直接在备用空间上构造元素
            --start.cur;                 //调整第一缓冲区的使用状态
        }
        else //第一缓冲区已无备用空间
            push_front_aux(t);
    }

    //只有当start.cur==start.first时才会被调用
    //当第一个缓冲区没有任何别用元素时才会被调用
    template <class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front_aux(const value_type &t)
    {
        value_type t_copy = t;
        reserve_map_at_front();              //若符合某种条件，则必须重换一个map
        *(start.node - 1) = allocate_node(); //配置一个新节点（缓冲区）
        __STL_TRY
        {
            start.set_node(start.node - 1); //改变start，令其指向新节点
            start.cur = start.last - 1;
            construct(start.cur, t_copy);
        }
        catch (...)
        {
            //"commit or rollback"
            start.set_node(start.node + 1);
            start.cur = start.first;
            deallocate_node(*(start.node - 1));
            throw;
        }
    }

    void reserve_map_to_back(size_type nodes_to_add = 1)
    {
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            //如果map尾端的节点备用空间不足
            //符合以上条件必须重换一个map（配置更大的，拷贝原来的，释放原来的）
            reallocate_map(nodes_to_add, false);
    }

    void reserve_map_to_front(size_type nodes_to_add = 1)
    {
        if (nodes_to_add + 1 > start.node - map)
            //如果map前段的节点备用空间不足
            //符合以上条件必须重换一个map（配置更大的，拷贝原来的，释放原来的）
            reallocate_map(nodes_to_add, true);
    }

    template <class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reallocte_map(size_type nodes_to_add,
                                                 bool add_at_front)
    {
        size_type old_num_nodes = finish.node-start.node+1;
        size_type new_num_nodes = old_num_nodes+nodes_to_add;

        map_pointer new_nstart;
        if(map_size>2*new_num_nodes){
            new_nstart=map+(map_size-new_num_nodes)/2
            +(add_at_front?nodes_to_add:0); 
            if(new_nstart<start.node)copy(start.node,finish.node+1,new_nstart);
            else copy_backward(start.node,finish.node+1,new_nstart+old_num_nodes);
        }
        else{
            size_type new_map_size=map_size+max(map_size,nodes_to_add)+2;
            //配置一块空间，准备给新map用
            map_pointer new_map=map_allocator::allocate(new_map_size);
            new_nstart=new_map+(new_map_size-new_num_nodes)/2
            +(add_at_front?nodes_to_add:0);
            //把原map内容拷贝过来
            copy(start.node,finish.node+1,new_nstart);
            //释放原map
            map_allocator::deallocate(map,map_size);
            //设定新map的起始地址与大小
            map=new_map;
            map_size=new_map_size;
        }    
        //重新设定迭代器start和finish
        start.set_node(new_nstart);
        finish.set_node(new_nstart+old_num_nodes-1);
    }

    void pop_back(){
        if(finish.cur!=finish.first){
            //最后一个缓冲区有一个（或更多）元素
            --finish.cur;//调整指针，相当于排除了最后元素
            destroy(finish.cur);//将最后元素析构
        }
        else
        //最后缓冲区没有任何元素
        pop_back_aux(); //这里将进行缓冲区释放工作
    }

    //只有当finish.cur==finish.first时才会被调用
    template<class T,class Alloc,size_t BufSize>
    void deque<T,Alloc,BufSize>::pop_back_aux(){
        deallocate_node(finish.first);//释放最后一个缓冲区
        finish.set_node(finish.node-1);//调整finish的状态，使指向上一个缓冲区的最后一个元素
        finish.cur=finish.last-1;
        destroy(finish.cur);//将该元素x析构
    }

    void pop_front(){
        if(start.cur!=start.last-1){
            //第一个缓冲区有两个（或更多）元素
            destroy(start.cur);//将最后元素析构
            --start.cur;//调整指针，相当于排除了最后元素
        }
        else
        //第一个缓冲区仅有一个元素
        pop_front_aux(); //这里将进行缓冲区释放工作
    }

    //只有当start.cur==start.last-1时才会被调用
    template<class T,class Alloc,size_t BufSize>
    void deque<T,Alloc,BufSize>::pop_front_aux(){
        destroy(start.cur);//将第一缓冲区第一个元素（也是唯一一个）析构
        deallocate_node(start.first);//释放最后一个缓冲区
        start.set_node(start.node+1);//调整start的状态，使指向上一个缓冲区的最后一个元素
        start.cur=start.first;
    }

    //注意，最终需要保留一个缓冲区，这是deque的策略，也是deque的初始状态
    template<class T,class Alloc,size_t BufSize>
    void deque<T,Alloc,BufSize>::clear(){
        //以下针对头尾意外的每一个缓冲区（它们一定是满的）
        for(map_pointer node=start.node+1;node<finish.node;++node){
        //将缓冲区内所有元素析构，注意，用的是destroy()第二个版本
        destroy(*node,*node+buffer_size());
        //释放缓冲区内存
        data_allocator::deallocate(*node,buffer_size());
        }

        if(start.node!=finish.node){//至少有头尾两个缓冲区
        destroy(start.cur,start.last);
        destroy(finish.first,finish.cur);
        //以下释放尾缓冲区，头缓冲区保留
        data_allocator::deallocate(finish.first,buffer_size());
        }
        else//只有一个缓冲区
        destroy(start.cur,finish.cur);//将此唯一缓冲区内的所有元素析构
        //注意，并不释放缓冲区空间，这唯一的缓冲区将保留
        finish=start;//调整状态
    }

    //清除pos所指的元素，pos为清除点
    iterator erase(iterator pos){
        iterator next=pos;
        ++next;
        difference_type index=pos-start;//清除点之前的元素个数
        if(index<(size()>>1)){          //如果清除点之前的元素比较少
        copy_backward(start,pos,next);  //就移动清除点之前的元素
        pop_front();                    //移动完毕，最前一个元素冗余，弃之
        }
        else{                           //清除点之后的元素较少
        copy(next,finish,pos);          //移动清除点后的元素
        pop_back();                     //移动完毕，最后一个元素冗余，弃之
        }
        return start+index;
    }

    //清除[first,last)区间所有元素
    template
};