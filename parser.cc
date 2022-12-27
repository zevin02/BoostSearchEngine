#include "parser.hpp"
// 这个地方就是要去标签化， 就是去掉所有<>里面的信息，保留 <>外面的的数据,<>里面的数据对我们是没有用的
// 把每个文档都去标签，都写入到一个文件中，每个文档内容占一行，以\3来区分

// 这里是一个目录，下面就是所有的html的网页

// 一个文档的信息，就包括标题,内容,url

int main()
{
     spdlog::info("Parse data source start");
     vector<string> File_Name_Container;
     spdlog::info("Retrieve all files with suffix .html start");
     // 递归式的把每个html文件名带路径，保存到file_list,方便后期一个一个对文件进行读取
     if (!CollectHtmlFile(src_path, File_Name_Container))
     {
          spdlog::info("Retrieve all files with suffix .html fail");
          return 1;
     }
     spdlog::info("Retrieve all files with suffix .html success");

     // 按照files_list读取每个文件的内容，进行解析
     vector<DocInfo> DocList;
     spdlog::info("Extract title,content,url from html file start");
     if (!ExtractElem(File_Name_Container, DocList))
     {
          spdlog::info("Extract title,content,url from html file fail");
          return 2;
     }
     spdlog::info("Extract title,content,url from html file success");

     // 把解析完的文件内容，写入到output,按照\3作为每个文档的分隔符
     spdlog::info("Save data to file {} ",output);
     if (!SaveToFile(DocList, output))
     {
          spdlog::info("Save data to file {} fail",output);
          return 3;
     }
     spdlog::info("Save data to file {} success",output);

     spdlog::info("Parse data source ok");
     return 0;
}

