#pragma once
#include "index.hpp"
#include <algorithm>
#include <unordered_set>

namespace ns_searcher
{
    struct DeduplicateNode
    {
        uint64_t doc_id;
        int weight;
        vector<string> words;
        DeduplicateNode()
            : doc_id(0), weight(0)
        {
        }
    };

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
            spdlog::info("GetInstace() start");
            index = ns_index::index::GetInstance(); // 获得单例对象
            // cout << "获取单例成功......" << endl;
            spdlog::info("GetInstance() ok");
            
            // LOG(NORMAL, "获取单例成功......");
            spdlog::info("BuildIndex() start");
            index->BuildIndex(output);
            spdlog::info("BuildIndex() ok");

            // cout << "建立正排和倒排索引成功......." << endl;
            // LOG(NORMAL, "建立正排和倒排索引成功.......");
        }
        void Search(const string &query, string &json_string)
        {
            spdlog::info("Search() called with parameters => {}", query);
            // 1. query搜索关键字
            // 2. json_string 返回给用户浏览器的搜索结果
            //[1]搜索的词，也要进行分词
            // boost::to_lower(query);
            vector<string> queryret; // 分完词的所有结果
            spdlog::info("JieBaUtil::cut start");
            ns_util::JiebaUtil::cut(query, queryret);
            spdlog::info("JieBaUtil::cut ret");
            // 我们在建立大小写的时候是忽略大小写的，所以也要把关键字左大小写的转化
            //[2]根据分词结果，查找到排拉链
            // 去重，用map来进行赛选，再放到ector中
            map<uint64_t, DeduplicateNode> invertednodeall;
            for (string &word : queryret)
            {
                boost::to_lower(word);
                // 这个地方获得了关键字，就要获得对应的倒排拉链
                spdlog::info("GetInvertedlist() start");
                vector<ns_util::InvertedElem> *list = index->GetInvertedlist(word);
                spdlog::info("GetInvertedlist() ret");
                // 获取倒排拉链之后，就根据获得的拉链，获得正排索引
                if (list == nullptr)
                {
                    // 没有找到
                    spdlog::info("GetInvertedlist() {} has no list", word);
                    continue;
                }
                // 不完美的地方，不同的关键字可能查出来的文档id是一样的
                // 我们需要进行去重
                spdlog::info("GetInvertedlist() {} has list", word);
                for (int i = 0; i < list->size(); i++)
                {
                    DeduplicateNode node;
                    node.doc_id = (*list)[i].doc_id;
                    node.weight += (*list)[i].weight;
                    node.words.push_back((*list)[i].word);
                    invertednodeall[(*list)[i].doc_id] = node; // 去重
                }
                // invertedlistall.insert(invertedlistall.end(), list->begin(), list->end()); // 把list中的所有节点插入
            }

            vector<DeduplicateNode> invertedlistall; // 把所有的倒排节点汇总在一起
            for (auto pair : invertednodeall)
            {
                invertedlistall.push_back(move(pair.second));
            }
            // 【3】更具获得的拉链，按照weirthg进行排序
            sort(invertedlistall.begin(), invertedlistall.end(), [](const DeduplicateNode &n1, const DeduplicateNode &n2)
                 {
                     return n1.weight > n2.weight; // 保持前面大于后面,降序排序
                 });
            // 【4】构建json串
            Json::Value root;
            for (auto &item : invertedlistall)
            {
                // 根据当前中的倒排节点获得doc_id,再拿这个doc_id去获得他的正排索
                spdlog::info("GetForwardIndex() start");
                ns_util::DocInfo *doc = index->GetForwardIndex(item.doc_id);
                spdlog::info("GetForwardIndex() ret");

                if (doc == nullptr)
                {
                    // 没找到
                    spdlog::info("GetForwardIndex() {} cannot find its doc", item.doc_id);
                    continue;
                }
                // 找到了
                // doc里面就是我们需要的东西了
                spdlog::info("GetForwardIndex() doc_id={} find its doc", item.doc_id);

                // string tmpjsonstr = ns_util::JsonUtil::ResponseSerialize(doc,item);
                // json_string += tmpjsonstr;
                Json::Value elem;
                elem["title"] = doc->title;
                elem["content"] = ns_util::StringUtil::GetDesc(doc->content, item.words[0]);
                elem["url"] = doc->url;
                root.append(elem);
            }
            // Json::StyledWriter writer;
            Json::FastWriter writer;

            json_string = writer.write(root);
            // spdlog::info("Search() returned {}",json_string);
        }
    };
};