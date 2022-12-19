// 这个地方就是要去标签化， 就是去掉所有<>里面的信息，保留 <>外面的的数据,<>里面的数据对我们是没有用的
// 把每个文档都去标签，都写入到一个文件中，每个文档内容占一行，以\3来区分
#include <iostream>
#include <string>
#include <vector>
// 这里我们需要引入boost 库
#include <boost/filesystem.hpp>
#include "util.hpp"

using namespace std;
// 这里是一个目录，下面就是所有的html的网页
const std::string src_path = "data/input/";
const std::string output = "data/raw_html/raw.txt";

// 一个文档的信息，就包括标题,内容,url
struct DocInfo_t
{
     string title;   // 网页中的标题
     string content; // 文档的内容
     string url;     // 文档官方中url
};
bool EnumFile(const string &src_path, vector<string> *file_list);

bool ParseHtml(const vector<string> &files_list, vector<DocInfo_t> *result);
bool SaveHtml(const vector<DocInfo_t> &result, const string &output);

int main()
{
     vector<string> file_list;
     // 递归式的把每个html文件名带路径，保存到file_list,方便后期一个一个对文件进行读取
     if (!EnumFile(src_path, &file_list))
     {
          cerr << "enum file name error!" << endl;
          return 1;
     }
     // 按照files_list读取每个文件的内容，进行解析
     vector<DocInfo_t> result;
     if (!ParseHtml(file_list, &result))
     {
          cerr << "parse html error" << endl;
          return 2;
     }
     // 把解析完的文件内容，写入到output,按照\3作为每个文档的分隔符
     if (!SaveHtml(result, output))
     {
          cerr << "save html error" << endl;
          return 3;
     }
     // 这里就要对文件进行递归遍历了

     return 0;
}

bool EnumFile(const string &src_path, vector<string> *file_list)
{
     namespace fs = boost::filesystem; // filesystem 的命名空间
     fs::path root_path(src_path);     // 定义一个path对象，遍历就从这个路径开始
     if (!fs::exists(root_path))       // 判断给定路径是否存在
     {
          cerr << src_path << "not exists" << endl; // 路径不存在
          return false;
     }
     // 定义一个空的递归迭代器,用来进行判断递归结束
     fs::recursive_directory_iterator end;
     for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++)
     {
          // 必须是普通文件
          if (!fs::is_regular_file(*iter))
          {
               continue;
          }
          // 一定是普通文件
          // 必须要是.html后缀
          if (iter->path().extension() != ".html") // 提取路径字符串
          {
               // 检测后缀不是.html
               continue;
          }
          // 当前的路径一定是合法的，以.html结束的普通网页
          //  cout<<"debug:"<<iter->path().string()<<endl;
          file_list->push_back(iter->path().string()); // 以字符串的形式呈现出来
     }
     return true;
}
static bool ParseTitle(const string &result, string *title)
{

     // 解析title，在文档中去检索<title> </title>,来查找他们的位置，我们要这之间的位置
     size_t begin = result.find("<title>");
     if (begin == string::npos)
     {
          // 没有找到
          return false;
     }
     size_t end = result.find("</title>");
     if (end == string::npos)
     {
          // 没有找到
          return false;
     }
     begin += strlen("<title>");
     if (begin > end) // begin错误，在end的后面
     {
          return false;
     }
     *title = result.substr(begin, end - begin); // 从begin开始提取，提取end-begin这么多数据

     return true;
}

static bool ParseContent(const string file, string *content)//解析文档
{
     // 把整个文档中的双标签都要去掉，在标签内的都去掉,我们要基于一个简易的状态机器来编写
     enum status
     {
          LABEL,
          CONTENT
     };
     enum status s = LABEL;
     for (char c : file)
     {
          // 在进行遍历的时候，只要碰到了>,就意味着，当前的标签都被处理完成了
          // 遇到一个<,说明content读取完成
          switch (s)
          {
          case LABEL:
               if (c == '>')
                    s = CONTENT;
               break;
          case CONTENT:
               if (c == '<')
                    s = LABEL;
               else
               {
                    // 此时是读取内容
                    // 我们不想保留原始文件中的\n,因为我们想用\n,作文html文本的分隔符
                    if (c == '\n')
                         c = ' '; // 设置成一个字符
                    content->push_back(c);
               }
               break;
          default:
               break;
          }
     }

     return true;
}
int n=0;
static bool ParseUrl(string file, string *url)
{
     //构建URL
     //https://www.boost.org/doc/libs/1_80_0/doc/html/accumulators.html,这个是网页中对应的url
     //而因为我们把数据拷贝到了data/input下面
     //我们把doc/html的数据拷贝到了data/input之下
     //而我们实际的是在data/input/accumulators.html中，所以我们要去掉前面的data/input，把后面的数据拼接到https://www.boost.org/doc/libs/1_80_0/doc/html,后面
     //url_head=https://www.boost.org/doc/libs/1_80_0/doc/html
     //url_tail=/accumulators.html
     //url=url_head+url_tail,这样就形成了一个官网链接
     string url_head="https://www.boost.org/doc/libs/1_80_0/doc/html";
     file.erase(0,strlen("data/input"));
     url_head+=file;
     if(!n)
     cout<<url_head<<endl;
     n++;
     return true;
}
bool ParseHtml(const vector<string> &files_list, vector<DocInfo_t> *results)
{
     // 读取文件，read()
     // 解析指定为文件，提取title，content，url
     for (const std::string &file : files_list)
     {
 
          string result;
          if (!ns_util::FileUtil::ReadFile(file, &result)) // 把数据从文件中读取到result中
          {
               continue;
          }
          DocInfo_t doc;
          // 提取title
          if (!ParseTitle(result, &doc.title))
          {
               continue;
          }
          // 提取content
          // 获得内容，去标签
          if (!ParseContent(result, &doc.content))
          {
               continue;
          }
          // 提取url

          if (!ParseUrl(file, &doc.url))
          {
               continue;
          }
          // 这些都完成了解析任务，当前文档的结构都在Doc中，
          results->push_back(doc);
     }
     return true;
}
bool SaveHtml(const vector<DocInfo_t> &result, const string &output)
{
     return true;
}