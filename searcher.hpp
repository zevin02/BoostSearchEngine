#pragma once
#include "index.hpp"
#include <algorithm>
namespace ns_searcher
{
    class Searcher
    {
    private:
        ns_index::index *index; // 单例

    public:
        Searcher()
        {
        }
        ~Searcher()
        {
        }
        void InitSearcher()
        {
            // 获取或者构建index对象
            // 根据index对象建立索引
            index = ns_index::index::GetInstance(); // 获得单例对象
            cout << "获取单例成功......" << endl;
            index->BuildIndex(output);
            cout << "建立正排和倒排索引成功......." << endl;
        }
        void Search(const string &query, string &json_string)
        {
            // 1. query搜索关键字
            // 2. json_string 返回给用户浏览器的搜索结果
            //[1]搜索的词，也要进行分词
            // boost::to_lower(query);
            vector<string> queryret; // 分完词的所有结果
            ns_util::JiebaUtil::cut(query, queryret);
            // 我们在建立大小写的时候是忽略大小写的，所以也要把关键字左大小写的转化
            //[2]根据分词结果，查找到排拉链
            vector<ns_util::InvertedElem> invertedlistall; // 把所有的倒排节点汇总在一起
            for (string &word : queryret)
            {
                boost::to_lower(word);
                // 这个地方获得了关键字，就要获得对应的倒排拉链
                vector<ns_util::InvertedElem> *list = index->GetInvertedlist(word);
                // 获取倒排拉链之后，就根据获得的拉链，获得正排索引
                if (list == nullptr)
                {
                    // 没有找到
                    continue;
                }
                // 不完美的地方，不同的关键字可能查出来的文档id是一样的
                invertedlistall.insert(invertedlistall.end(), list->begin(), list->end()); // 把list中的所有节点插入
            }

            // 【3】更具获得的拉链，按照weirthg进行排序
            sort(invertedlistall.begin(), invertedlistall.end(), [](const ns_util::InvertedElem &n1, const ns_util::InvertedElem &n2)
                 {
                     return n1.weight > n2.weight; // 保持前面大于后面,降序排序
                 });
            // 【4】构建json串
            for (ns_util::InvertedElem &item : invertedlistall)
            {
                // 根据当前中的倒排节点获得doc_id,再拿这个doc_id去获得他的正排索
                ns_util::DocInfo *doc = index->GetForwardIndex(item.doc_id);
                if (doc == nullptr)
                {
                    // 没找到
                    continue;
                }
                // 找到了
                // doc里面就是我们需要的东西了
                
                string tmpjsonstr = ns_util::JsonUtil::ResponseSerialize(doc,item);
                json_string += tmpjsonstr;
            }
        }

    private:

    };
};