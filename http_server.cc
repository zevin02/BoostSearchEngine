#include"searcher.hpp"
#include"cpp-httplib/httplib.h"
const string root_path="www_root";
int main()
{
    ns_searcher::Searcher searcher;
    searcher.InitSearcher();
    // searcher.Search();
    httplib::Server svr;
    svr.set_base_dir(root_path.c_str());//设置浏览器访问的根目录
    svr.Get("/s",[&](const httplib::Request& req,httplib::Response& res){
        if(!req.has_param("word"))
        {
            res.set_content("要搜索必须要有搜索关键字!","text/plain:charset=utf-8");
            return;
        }
        string word=req.get_param_value("word");//获得关键字的参数
        cout<<"用户在搜索使用的关键字->"<<word<<endl;
        string json_string;
        searcher.Search(word,json_string);
        res.set_content(json_string,"application/json");//设置响应
    });
    svr.listen("0.0.0.0",8080);
    return 0;
}