#include "searcher.hpp"
#include "httplib.h"

const std::string root_path = "./wwwroot";
const std::string input = "/home/Router/Boost_Search_Engines/data/output/out.txt";

int main()
{
    // 创建searcher并初始化
    ns_searcher::Searcher searcher;
    searcher.InitSearch(input);
    httplib::Server svr;
    if (!svr.set_base_dir(root_path.c_str()))
    {
        LOG(FATAL, "set base dir fail.");
        exit(4);
    }
    // 仿照百度 请求的是"/s"
    svr.Get("/s", [&](const httplib::Request& req, httplib::Response& res)
    {
        // 如果请求中没有参数"word"
        if (!req.has_param("word"))
        {
            res.set_content("使用搜索引擎必须要有关键字", "text/plain; charset=utf-8");
            return;
        }
        // 获得请求参数"word"
        std::string searchword = req.get_param_value("word");
        // std::cout << "用户在搜索: " << searchword << std::endl;
        LOG(NORMAL, "用户" + req.remote_addr + "正在搜索:" + searchword);
        // 完成搜索业务
        std::string jsonstring;
        searcher.Search(searchword, jsonstring);
        res.set_content(jsonstring, "application/json");
    });
    LOG(NORMAL, "服务启动成功...");
    svr.listen("0.0.0.0", 712);
    return 0;
}