// not finished
void main()
{
    List<int> mylist;
    for(int i =0;i<5;++i){
        mylist.insert_front(i);
        mylist.insert_end(i+2);
    }
    mylist.distplay();

    ListIter<ListItem<int>> begin(mylist.front());
    ListIter<ListItem<int>> end; //default 0,nullptr
    ListIter<ListItem<int>> iter;//default 0,nullptr

    iter = find(begin,end,3);
    if(iter == end)cout<<"not found"<<endl;
    else cout<<"found"<<iter->value()<<endl;
    // found 3

    iter = find(begin,end,7);
    if(iter == end)cout<<"not found"<<endl;
    else cout<<"found"<<iter->value()<<endl;
    // not found
}