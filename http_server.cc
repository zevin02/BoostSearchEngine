#include"searcher.hpp"
#include"cpp-httplib/httplib.h"
#include<mysql/mysql.h>
#include<memory>
const string root_path="www_root";
int main()
{
    // ns_searcher::Searcher searcher;
    shared_ptr<ns_searcher::Searcher> searcher;

    spdlog::info("Boost Search Engine start");
    searcher->InitSearcher();
    spdlog::info("Boost Search Engine build ok");
    // searcher.Search();
    httplib::Server svr;
    svr.set_base_dir(root_path.c_str());//设置浏览器访问的根目录
    svr.Get("/s",[&](const httplib::Request& req,httplib::Response& res){
        if(!req.has_param("word"))
        {
            spdlog::info("用户没输入关键字");
            res.set_content("要搜索必须要有搜索关键字!","text/plain:charset=utf-8");
            return;
        }
        string word=req.get_param_value("word");//获得关键字的参数
        // cout<<"用户在搜索使用的关键字->"<<word<<endl;
        // LOG(NORMAL,"用户在搜索使用的关键字->"+word);
        spdlog::info("用户输入的关键字 {}",word);
        string json_string;
        searcher->Search(word,json_string);
        // cout<<json_string<<endl;
        spdlog::debug(json_string);
        res.set_content(json_string,"application/json");//设置响应
    });
    // LOG(NORMAL,"服务器启动成功....");
    spdlog::info("服务器启动成功....");
    svr.listen("0.0.0.0",8080);
    
    return 0;
}
