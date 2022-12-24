#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "cppjieba/include/cppjieba/Jieba.hpp"
#include "jsoncpp/json/json.h"

const std::string src_path = "data/input/";
const std::string output = "data/raw_html/raw.txt";
using namespace std;
// static int n=0;
namespace ns_util
{
    class FileUtil
    {
    public:
        static bool ReadFile(const std::string &file_path, std::string *out)
        {

            ifstream in(file_path, ios::in);
            // 打开一个文件
            if (!in.is_open()) // 打开成功
            {
                cerr << "open file " << file_path << " fail" << endl;
                return false;
            }
            string line;
            while (getline(in, line)) // 按行读取in中的文件
            {
                *out += line;
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
        static string ResponseSerialize(const DocInfo *doc)
        {
            // 把服务端构建的响应进行序列化
            Json::Value root;
            root["title"] = doc->title;

            root["content"] = GetContent(doc->content);
            root["url"] = doc->url;
            Json::StyledWriter writer;
            string sendwriter = writer.write(root);
            return sendwriter;
        }
    };
};