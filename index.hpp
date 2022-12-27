#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "util.hpp"
#include <mutex>
#include"log.hpp"
using namespace std;
namespace ns_index
{

    class index
    {
        // 正排索引,根据id找文档
    private:
        vector<ns_util::DocInfo> forward_index; // 正排索引
        // 倒排索引,一个关键字和一组id的索引
        unordered_map<string, vector<ns_util::InvertedElem>> Inverted_index; // 根据关键字查找到对应的数据
        index()
        {
        }
        index(const index &in) = delete;
        index &operator=(const index &in) = delete;
        static index *instance; // 构建单例
        static mutex mtx;

    public:
        ~index()
        {
        }
        // 根据id找到正排缩影
        static index *GetInstance()
        {
            if (instance == nullptr) // 进行双重判断
            {
                mtx.lock();
                // 解决线程安全的问题
                if (instance == nullptr)
                {
                    instance = new index();
                }
                mtx.unlock();
            }
            return instance;
        }
        ns_util::DocInfo *GetForwardIndex(const uint64_t doc_id)
        {
            if (doc_id > forward_index.size())
            {
                cerr << "doc_id out range,error!" << endl;
                return nullptr;
            }
            return &forward_index[doc_id];
            // return nullptr;
        }
        // 根据关键字找到倒排拉链
        vector<ns_util::InvertedElem> *GetInvertedlist(const string &word)
        {
            auto iter = Inverted_index.find(word);
            if (iter == Inverted_index.end())
            {
                cerr << word << " have no InvertList" << endl;
                return nullptr;
            }
            return &(iter->second);
        }
        // 构建索引
        bool BuildIndex(string html_path)
        {
            // 把parse处理完的数据交过来
            // 根据去标签之后的文档构建正派和倒排索引
            spdlog::info("BuildIndex() called with parameters =>{}",html_path);
            ifstream ifs(html_path, ios::in | ios::binary);
            if (!ifs.is_open())
            {
                spdlog::info("open {} file fail",html_path);
                // cerr << "open " << html_path << " fail" << endl;
                return false;
            }
            spdlog::info("open {} file success",html_path);

            string line;
            int n = 0;
            while (getline(ifs, line))
            {
                ns_util::DocInfo *ret = BuildForwardIndex(line, n); // 建立正排索引
                if (ret == nullptr)
                {
                    spdlog::info("BuildForwardIndex {} error",line);
                    // cerr << "build " << line << " error" << endl;
                    continue;
                }
                // spdlog::info("BuildForwardIndex {} success");
                
                BuildInvertedIndex(ret); // 建立倒排索引

                n++;
            }
            return true;
        }

    private:
        ns_util::DocInfo *BuildForwardIndex(const string &line, int n) // 构建正排索引
        {
            // 1. 解析line，进行字符串切分
            // 3. 插入到vector中
            vector<string> ret;
            string sep = "\3";
            ns_util::StringUtil::CutString(line, &ret, sep);
            if (ret.size() != 3)
            {
                cerr << "cut error" << endl;
                return nullptr;
            }
            // 2. 字符串填充到docinfo中
            ns_util::DocInfo doc;
            doc.title = ret[0];
            doc.content = ret[1];
            doc.url = ret[2];
            doc.doc_id = n;
            forward_index.push_back(move(doc));
            return &forward_index.back();
        }
        bool BuildInvertedIndex(ns_util::DocInfo *&doc) // 构建到排拉链
        {
            // DocInfo(title,content,url,id)
            // 倒排拉链的映射，根据关键字找最对应的序号，查找拉链
            // 1。根据拿到的数据title/content进行拆分出多个关键字
            // 2.处理相关性，次频,可以知道在文档和标题中，词出现的次数
            // 3.构建相关性
            struct word_cnt
            {
                int title_cnt;
                int content_cnt;
                word_cnt()
                    : title_cnt(0), content_cnt(0)
                {
                }
            };
            unordered_map<string, word_cnt> word_map; // 用来暂存词频的
            vector<string> titleword;                 //
            ns_util::JiebaUtil::cut(doc->title, titleword);
            for (string &word : titleword)
            {
                boost::to_lower(word); // 把分词统一转化成小写
                word_map[word].title_cnt++;
            }
            // 同样还要进行内容的词频统计
            vector<string> contentword; //
            ns_util::JiebaUtil::cut(doc->content, contentword);
            for (string &word : contentword)
            {
                boost::to_lower(word); // 把分词统一转化成小写
                word_map[word].content_cnt++;
            }
//
#define X 10
#define Y 1
            // 忽略大小写
            for (auto word_pair : word_map)
            {
                ns_util::InvertedElem item; // 倒排索引
                item.doc_id = doc->doc_id;
                item.word = word_pair.first;
                item.weight = X * word_pair.second.title_cnt + Y * word_pair.second.content_cnt; // 相关性
                Inverted_index[word_pair.first].push_back(move(item));                           // 把他放进
            }
            // 统计标题中词出现的次数
            return true;
        }
    };
    index *index::instance = nullptr; // 把该index在类外进行初始化
    mutex index::mtx;

};