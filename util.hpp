#pragma once
#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include<boost/algorithm/string.hpp>
using namespace std;
// static int n=0;
namespace ns_util
{
    class FileUtil
    {
        public:
        static bool ReadFile(const std::string & file_path,std::string* out)
        {
  
            ifstream in(file_path,ios::in);
            //打开一个文件
            if(!in.is_open())//打开成功
            {
                cerr<<"open file "<<file_path<<" fail"<<endl;
                return false;
            }
            string line;
            while(getline(in,line))//按行读取in中的文件
            {
                *out+=line;
            }

            in.close();//关闭文件
            return true;
        }
    };
    class StringUtil
    {
        public:
        static void CutString(const string &target,vector<string> *out,string sep)
        {
            boost::split(*out,target,boost::is_any_of(sep),boost::token_compress_on);//压缩中间的分隔符,把所有的压缩成一个\3
            
        }
    };
};