#include"searcher.hpp"

int main()
{
    ns_searcher::Searcher* searcher=new ns_searcher::Searcher();
    searcher->InitSearcher();
    while(true)
    {
        cout<<"input search query"<<endl;
        string query;
        getline(cin,query);//getline不会包含回车
        string json_string;
        searcher->Search(query,json_string);
        //此时有了搜索的结果
        cout<<"search result->"<<json_string;
    }

    return 0;
}