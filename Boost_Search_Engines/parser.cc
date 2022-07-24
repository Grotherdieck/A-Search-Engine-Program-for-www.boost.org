#include <iostream>
#include <string>
#include <vector>
#include <cerrno>
#include <boost/filesystem.hpp>
#include <cstring>
#include "usetools.hpp"

// 所有的源html文件路径
const std::string src_path = "data/input";
// 处理完放的地方
const std::string out = "data/output/out.txt";
// 官网链接前缀
const std::string url_head = "https://www.boost.org/doc/libs/1_78_0/doc/html";

typedef struct DocInfo
{
    std::string title;   // 标题
    std::string content; // 文件简要内容
    std::string url;     // 该文件官网的url
}DocInfo_t;


bool EnumFile(const std::string& src_path, std::vector<std::string>& file_list);

bool ParseHtml(const std::vector<std::string>& file_list, std::vector<DocInfo_t>& results);

static bool ParseTitle(const std::string& str, std::string& out);

static bool ParseContent(const std::string& str, std::string& out);

static bool ParseUrl(const std::string& url_head, const std::string& file_path, std::string& url);

bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output);

int main()
{
    // 先把所有目标路径下的html网页路径都读到这个vector中
    std::vector<std::string> file_list;
    // 1.递归式的把每个html文件的文件名带路径放到file_list中 方便后续对文件进行读取
    if (!EnumFile(src_path, file_list))
    {
        // 如果失败
        std::cerr << "EnumFile failed, error code : " << errno << std::endl;
        return 1;
    }
    // 2.按照file_list读取文件内容，并进行解析
    std::vector<DocInfo_t> results;
    if (!ParseHtml(file_list, results))
    {
        std::cerr << "ParseHtml failed, error code : " << errno << std::endl;
        return 2;
    }
    // 3.把解析完毕的文件内容写入到out去 按照'\3'作为每个文档的分隔符
    if (!SaveHtml(results, out))
    {
        std::cerr << "SaveHtml failed, error code : " << errno << std::endl;
        return 3;
    }
    return 0;
}

bool EnumFile(const std::string& src_path, std::vector<std::string>& file_list)
{
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);
    // 如果路径不存在
    if (!fs::exists(root_path))
    {
        std::cerr << src_path << " don't exist..." << std::endl;
        return false;
    }
    // 定义一个空的递归迭代器 表示递归结束
    fs::recursive_directory_iterator end;
    // 开始递归遍历
    for (fs::recursive_directory_iterator it(root_path); it != end; ++it)
    {
        // 首先得是普通文件 而非目录
        if (!fs::is_regular_file(*it))
        {
            continue;
        }
        // 然后后缀得是.html 不是也给我删
        else if (it->path().extension() != ".html")
        {
            continue;
        }
        // 走到这里说名就是一个普通的.html文件
        file_list.push_back(it->path().string());
        // std::cout << "debug : " << it->path().string() << std::endl;
    }
    return true;
}

// for debug
// static void showdoc(const DocInfo_t& doc)
// {
//     std::cout << "title : " << doc.title << std::endl;
//     std::cout << "content : " << doc.content << std::endl;
//     std::cout << "url : " << doc.url << std::endl;
// }

bool ParseHtml(const std::vector<std::string>& file_list, std::vector<DocInfo_t>& results)
{
    // int cnt = 0;
    // file就是每个html的路径
    for (const auto& file : file_list)
    {
        //1.读取文件
        std::string out;
        if (!ust::FileTools::Read(file, out))
        {
            // std::cout << "Read file " << file << "failed" << std::endl;
            continue;
        }
        // std::cout << "1" << std::endl;
        DocInfo_t doc;
        //2.解析指定的文件，提取title 就是找<titile>标签
        if (!ParseTitle(out, doc.title))
        {
            continue;
        }
        // std::cout << "2" << std::endl;
        //3.解析指定的文件，提取content 就是去标签
        if (!ParseContent(out, doc.content))
        {
            continue;
        }
        // std::cout << "3" << std::endl;
        //4.解析指定的文件路径，构建url 
        if (!ParseUrl(url_head, file, doc.url))
        {
            continue;
        }
        // 走到这里就完成了解析任务 当前文档的信息已经在doc中
        // 用右值引用 效率更高
        // debug 
        // showdoc(doc);
        // break;
        results.push_back(std::move(doc));
        // ++cnt;
    }
    // std::cout << cnt << std::endl;
    return true;
}

static bool ParseTitle(const std::string& str, std::string& out)
{
    size_t begin = str.find("<title>");
    if (begin == std::string::npos)
    {
        return false;
    }
    size_t end = str.find("</title>");
    if (end == std::string::npos)
    {
        return false;
    }
    begin += std::string("<title>").size();
    // [begin, end)就是现在的要求的内容范围
    if (begin > end)
    {
        // 出错了
        return false;
    }
    out = str.substr(begin, end - begin);
    return true;
}

static bool ParseContent(const std::string& str, std::string& out)
{
    // 就是去标签
    enum status
    {
        LABLE,
        CONTENT 
    };
    enum status s = LABLE;// 一开始一定在标签内
    for (char c : str)
    {
        switch (s)
        {
        case LABLE:
            if (c == '>') s = CONTENT;// 标签结束了
            break;
        case CONTENT:
            if (c == '<') s = LABLE;// 新的标签开始了
            else 
            {
                // 是内容状态且不是'<'
                // 不保留'\n'
                if (c == '\n') c = ' ';
                out.push_back(c);
            }
            break;
        default:
            break;
        }
    }
    return true;
}

static bool ParseUrl(const std::string& url_head, const std::string& file_path, std::string& url)
{
    // file_path = src_path + url_tail
    url = url_head + file_path.substr(src_path.size());
    return true;
}




bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output)
{
    // 利用ofstream类进行二进制写入
    std::ofstream ofs(output, std::ios::out | std::ios::binary);
    if (!ofs.is_open())
    {
        std::cerr << "open file " << output << "failed" << std::endl;
        return false;
    }
    // 格式：title\3content\3url\n 
    for (auto&& doc : results)
    {
        ofs << doc.title << '\3' << doc.content << '\3' << doc.url << '\n';
    }
    return true;
}
