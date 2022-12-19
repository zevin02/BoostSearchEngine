#pragma once
#include<string>
#include<iostream>
#include<fstream>
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
};