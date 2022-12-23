#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "util.hpp"


using namespace std;
namespace ns_index
{
    struct DocInfo
    {
        string title; // 文档的标题
        string content;
        string url;
        uint64_t doc_id; // 文档的id
    };
    struct InvertedElem
    {
        string word;
        uint64_t doc_id;
        int weight; // 权重
    };
    class index
    {
        // 正排索引,根据id找文档
    private:
        vector<DocInfo> forward_index; // 正排索引
        // 倒排索引,一个关键字和一组id的索引
        unordered_map<string, vector<InvertedElem>> Inverted_index; // 根据关键字查找到对应的数据
    public:
        index()
        {
        }
        ~index()
        {
        }
        // 根据id找到正排缩影
        DocInfo *GetForwardIndex(const uint64_t doc_id)
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
        vector<InvertedElem> *GetInvertedlist(const string &word)
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
            ifstream ifs(html_path, ios::in | ios::binary);
            if (!ifs.is_open())
            {
                cerr << "open " << html_path << " fail" << endl;
                return false;
            }
            string line;
            int n = 0;
            while (getline(ifs, line))
            {
                DocInfo *ret = BuildForwardIndex(line, n); // 建立正排索引
                if (ret == nullptr)
                {
                    cerr << "build " << line << " error" << endl;
                    continue;
                }
                n++;
                BuildInvertedIndex(ret); // 建立倒排索引
            }
            return true;
        }

    private:
        DocInfo *BuildForwardIndex(const string &line, int n)
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
            DocInfo doc;
            doc.title = ret[0];
            doc.content = ret[1];
            doc.url = ret[2];
            doc.doc_id = n;
            forward_index.push_back(move(doc));
            return &forward_index.back();
        }
        bool BuildInvertedIndex(DocInfo*& doc)
        {
            // DocInfo(title,content,url,id)
            // 倒排拉链的映射，根据关键字找最对应的序号，查找拉链
            // 1。根据拿到的数据title/content进行拆分出多个关键字
            // 2.处理相关性，次频,可以知道在文档和标题中，词出现的次数
            // 3.构建相关性
            struct word_cnt{
                int title_cnt;
                int content_cnt;
                word_cnt()
                :title_cnt(0),content_cnt(0)
                {}
            }
            unordered_map<string,word_cnt> word_map;//用来暂存词频的
            vector<string> titleword;//
            ns_util::JiebaUtil::cut(doc.title,titleword)
            for(string& word:titleworld)
            {
                boost::to_lower(word);//把分词统一转化成小写
                word_map[word].title++;
            }
            //同样还要进行内容的词频统计
            vector<string> contentword;//
            ns_util::JiebaUtil::cut(doc.content,contentword);
            for(string& word:content)
            {
                boost::to_lower(word);//把分词统一转化成小写
                word_map[word].content++;
            }
            //
            #define X 10
            #define Y 1
            //忽略大小写
            for(auto word_pair:word_map)
            {
                InventedElem item;//倒排索引
                item.doc_id=doc.doc_id;
                item.word=word_pair.first;
                item.weight=X*word_pair.second.title_cnt+Y*word_pair.second.content_cnt;//相关性
                Invented_index[word_pair.first].push_back(item);//把他放进
            }
            //统计标题中词出现的次数
            return true;
        }
    };
};