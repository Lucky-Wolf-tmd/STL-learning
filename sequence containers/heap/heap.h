/*
使用list作为priority queue的底层机制，元素插入操作可享受常数时间。
但是要找到list中的极值,却需要对整个list进行线性扫描.
我们也可以改变做法,让元素插入前先排序,使得list元素总是有序的
但这么一来,虽然获取极值和元素删除操作效率达到最高,但插入操作却需要线性时间
*/

/*
比较麻烦的做法是binary search tree作为priority queue底层机制
元素插入和极值获取 只需要O(logN)对数时间
但过于小题大作
一来bst 对输入要求有足够的随机性（否则出现倾斜）
二来bst 实现起来并不容易（相对）
priority queue 的复杂度最好介于queue和bst之间
而binary heap就是适当候选人
*/

/*
binary heap就是一种complete binary tree
也就是说整颗binary tree除了最底层叶节点外，是填满的，而最底层叶节点从左到右不能有空隙

CBT整棵树内没有任何节点漏洞，这带来一个极大的好处：我们可以利用array来存储所有节点。

小技巧：
将array的#0元素保留（或者设置为无限大值/无线小值），当CBT中的某个节点位于array的i处时，
其left child必然位于array2i处，right child必然位于array的2i+1处
父节点必然位于i/2处
通过如此规则，array可以轻易实现出CBT，这种通过array映射tree的方式，我们称之为implicit representation，隐式表现

由于array无法动态改变大小，而heap必然需要这个功能，所以用vector来替代array

max-heap 最大值放在根节点，并总是位于底层array或vector的起头处
min-heap 最小值放在根节点，并总是位于底层array或vector的起头处

STL提供的是max-heap，以下以max-heap指代heap
*/

// percolate up 上滤、上溯
//将新节点与其父节点比较，如果key值比父节点大，就父子对换位置，如此一直上溯，直到不需要对换，或者到达根节点为止

template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first,
                      RandomAccessIterator last)
{
    //注意此函数被调用时，新元素已置于底部容器最尾端
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <class RandomAccessIterator, class Distance, class T>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *)
{
    __push_heap(first, Distance((last - fist) - 1), Distance(0), T(*(last - 1)));
    //以上根据implicit representation heap的结构特性：新值必须置于底部容器最尾端
    //此即第一个洞号：（last- first）-1）
}

//以下这组push_back()不允许指定"大小比较标准"
template <class RandomAccessIterator, class Distance, class T>
void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value)
{
    Distance parent = (holeIndex - 1) / 2; //找出父节点
    while (holeIndex > topIndex && *(first + parent) < value)
    {
        //当尚未到达顶端,且父节点小于新值(不符合heap次序特性)
        //由于以上使用operator<,可知STL heap是一种大根堆
        *(first + holeInex) = *(first + parent); //令洞值为父值
        holeIndex = parent;                      // percolate up;调整洞号,向上提升至父节点
        parent = (holeIndex - 1) / 2;            //新洞的父节点
    }                                            //持续至顶端,或者满足heap次序特性为止
    *(first + holeIndex) = value;                //令洞值为新值,完成插入操作
}

template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    __pop_heap_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T *)
{
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)),
               distance_type(first));
               //以上根据implicit representation heap 的次序特性,pop操作的结果应为底部容器的第一个元素
               //因此,首先设定欲调整值为尾值,然后将首值调至尾节点
               //(所以以上将迭代器result设为last-1)
               //然后重整[first,last-1),
               //使之重新成为一个合格的heap
}

//以下这组__pop_heap()不允许指定"大小比较标准"
template<class RandomAccessIterator,class T,class Distance>
inline void __pop_heap(RandomAccessIterator first,
                       RandomAccessIterator last,
                       RandomAccessIterator result,
                       T value, Distance*){
    *result=*first; //设定尾值为首值,于是尾值即为欲求结果
                    //可由客户端稍后再用底层容器的pop_back()取出尾值
    __adjust_heap(first,Distance(0),Distance(last-first),value);
    //以上欲重新调整heap，洞号为0（即树根处），与调整值为value（原尾值）
}

//以下这个__adjust_heap() 不允许指定大小比较标准
template<class RandomoAccessIterator, class Distance, class T>
void __adjust_heap(RandomoAccessIterator first,Distance holeIndex,
                Distance len,T value){
    Distance topIndex=holeIndex;
    Distance secondChild = 2*holeIndex+2;//洞节点的right child
    while(secondChild<len){
        //比较洞节点的左右孩子的值,然后以secondChild代表较大子节点
        if(*(first+secondChild)<*(first+(secondChild-1))) secondChild--;
        //Percolate down:令较大子值为洞值，再令洞号下移至较大子节点处
        *(first+holeIndex)=*(first+secondChild);
        holeIndex=secondChild;
        //找出新洞节点的right child
        secondChild = 2*(secondChild+1);
    }
    if(secondChild==len){//没有右子节点，只有左子节点
    //percolate down:令left child值为洞值，再令洞号下移至左子节点处
    *(first+holeIndex)=*(first+(secondChild-1));
    holeIndex=secondChild-1;
    }
    //此时可能尚未满足次序特性，执行一次percolate up操作
    //读者回应：不可如此
    //侯捷测试：的确不行
    __push_heap(first,holeIndex,topIndex,value);
}

//以下这个sort_heap算法不允许指定"大小比较标准"
template<class RandomAccessIterator>
void sort_heap(RandomAccessIterator first,RandomAccessIterator last){
    /*以下，每执行一次pop_heap(),极值(在STL中为极大值)即被放在尾端
    扣除尾端再执行一次pop_heap()，次极值又被放在新尾端，一直下去，最后得到排序结果
    */
   while(last-first>1)
   pop_heap(first,last--);//每执行一次pop_heap(),操作范围缩一格
}

//将[first,last)排列为一个heap
template<class RandomAccessIterator,class T,class Distance>
void make_heap(RandomAccessIterator first,RandomAccessIterator last,T*,Distance*){
    __make_heap(first,last,value_type(first),distance_type(first));
}

//以下这组make_heap()不允许指定大小比较标准
template<class RandomAccessIterator,class T, class Distance>
void __make_heap(RandomAccessIterator first,RandomAccessIterator last,T*,Distance*){
    if(last-first<2)return;//如果长度为0或1，不必排序
    Distance len=last-first;
    //找出第一个需要重排的子树头部，以parent标出，由于任何叶子节点都不需要执行percolate down,所以以下所有计算，
    //以parent命名不佳，宜为holeIndex
    Distance holeIndex=(len-2)/2;

    while(true){
        //重排以holeIndex为首的子树，len是为了让__adjust_heap()判断操作范围
        __adjust_heap(first,holeIndex,len,T(*(first+holeIndex)))
        if(holeIndex==0)return ;//走完根节点，就结束
        holeIndex--;            //(已重排的子树的)头部向前一个节点
    }
}