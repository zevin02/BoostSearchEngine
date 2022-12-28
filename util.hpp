#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "cppjieba/include/cppjieba/Jieba.hpp"
#include "jsoncpp/json/json.h"
#include "spdlog/spdlog.h"

const std::string src_path = "/home/zevin/boost_1_80_0/";
// const std::string src_path = "data/input/";
const std::string output = "data/raw_html/raw.txt";
using namespace std;
// static int n=0;
namespace ns_util
{
    class FileUtil
    {
    public:
        static bool ReadFile(const std::string &file_path, std::string &filetext)
        {

            ifstream in(file_path, ios::in);
            // 打开一个文件
            if (!in.is_open()) // 打开成功
            {
                spdlog::info("open {} fail", file_path);
                return false;
            }
            spdlog::info("open {} success", file_path);

            string line;
            while (getline(in, line)) // 按行读取in中的文件
            {
                filetext += line;
            }

            in.close(); // 关闭文件
            return true;
        }
    };
    class StringUtil
    {
    public:
        static void CutString(const string &target, vector<string> *out, string sep) // 字符串切分
        {
            boost::split(*out, target, boost::is_any_of(sep), boost::token_compress_on); // 压缩中间的分隔符,把所有的压缩成一个\3
        }
        static string GetDesc(string content, string word) // 同时要判断第一个单词和最后一个单词是否是一个完整的单词
        {
            // 根据content内容，在里面找第一次出现的word，往前截取50字节(如果没有，就从开头来获得)，往后截取100字节的内容(没有就到end)
            // 找到首次出现的位置
            // size_t pos = content.find(word);//我们这里的word是小写，但是content中并不是忽略大小写进行查找的
            auto iter = search(content.begin(), content.end(), word.begin(), word.end(), [](int x, int y)
                               { return tolower(x) == tolower(y); }); // 忽略成大小写进行查找
            size_t prev_order = 100;
            size_t next_order = 150;
            if (iter == content.end())
            {
                // cerr << "not find word" << endl;
                spdlog::info("not find keyword {}", word);
                return "NONE1";
            }
            else
            {
                size_t pos = distance(content.begin(), iter);
                int start = 0;
                int end = content.size();
                if (pos > prev_order)
                {
                    start = pos - prev_order;
                }
                if (end > pos + next_order)
                {
                    end = pos + next_order;
                }
                if (end < start)
                {
                    spdlog::info("getdesc() end<start");
                    return "NONE2";
                }
                string substr = content.substr(start, end - start);
                // 从substr中找到第一个空格
                size_t forwardblank = substr.find(" "); // 从前面找，还要从后面找
                size_t invertedblank = substr.rfind(" ");
                if (forwardblank == string::npos || invertedblank == string::npos)
                {
                    return substr + "...";
                }
                else
                {
                    substr = substr.substr(forwardblank + 1, invertedblank - forwardblank - 1) + "...";
                    return substr;
                }
            }
        }
        static void escapecontent(string &filetext, int &i, char &c) // 转义content
        {
            if (filetext[i + 1] == 'l' && filetext[i + 2] == 't')
            {
                c = '<';
                i += 3;
            }
            else if (filetext[i + 1] == 'g' && filetext[i + 2] == 't') // 判断&gt,>转义的
            {

                c = '>';
                i += 3;
            }
            else if (filetext[i + 1] == 'a' && filetext[i + 2] == 'm' && filetext[i + 3] == 'p') // 清理&amp；因为html给转义
            {
                i += 4;
            }
        }
        static void replace_all(std::string &dst_str, std::string sub_str, std::string new_str) // replace all string
        {

            boost::algorithm::replace_all(dst_str, sub_str,new_str);
        }
        // static
        static void escapetitle(string &filetext) // 转义title
        {
            if (filetext.find("&gt;") != string::npos)
            {
                replace_all(filetext, "&gt;", ">");
            }
            if (filetext.find("&lt;") != string::npos)
            {
                replace_all(filetext, "&lt;", "<");
            }
            if (filetext.find("&amp;") != string::npos)
            {
                replace_all(filetext, "&amp;", "&");
            }
        }
    };

    class JiebaUtil
    {

    private:
        static cppjieba::Jieba jieba; // 构造函数,静态对象属于这个类
    public:
        static void cut(const string line, vector<string> &out) // 把一个字符床切分成了多个字符串，只有在static里面才能使用这个静态对象
        {
            jieba.CutForSearch(line, out);
        }
    };
    const char *const DICT_PATH = "cppjieba/dict/jieba.dict.utf8";
    const char *const HMM_PATH = "cppjieba/dict/hmm_model.utf8";
    const char *const USER_DICT_PATH = "cppjieba/dict/user.dict.utf8";
    const char *const IDF_PATH = "cppjieba/dict/idf.utf8";
    const char *const STOP_WORD_PATH = "cppjieba/dict/stop_words.utf8";
    cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH); // 构造函数,静态对象属于这个类,但是要放在类外初始化

    struct DocInfo // 正排索引
    {
        string title; // 文档的标题
        string content;
        string url;
        uint64_t doc_id; // 文档的id
    };
    struct InvertedElem // 倒排索引
    {
        string word;
        uint64_t doc_id;
        int weight; // 权重
    };
    class JsonUtil
    {
    public:
        static string ResponseSerialize(const DocInfo *doc, InvertedElem &item)
        {
            // 把服务端构建的响应进行序列化
            Json::Value root;
            root["title"] = doc->title;

            root["content"] = StringUtil::GetDesc(doc->content, item.word);
            root["url"] = doc->url;
            // root["id"]=(int)doc->doc_id;
            // root["weight"]=item.weight;
            Json::StyledWriter writer;
            string sendwriter = writer.write(root);
            return sendwriter;
        }
    };

};