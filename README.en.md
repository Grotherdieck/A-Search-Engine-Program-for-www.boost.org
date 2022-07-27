# 一、这是什么？
&emsp;&emsp;是一个针对[boost官网](https://www.boost.org)的简易站内搜索引擎项目，并且支持下载网页的HTML文件更新搜索引擎内容，boost作为cpp的准标准库，但是它的官网没有搜索功能，用起来不是很方便。

&emsp;&emsp;主要使用的语言：
- 服务端:C++:对下载的HTML文件去标签并提取正文摘要拼接url、根据清洗后的数据并利用分词工具[cppjieba](https://github.com/yanyiwu/cppjieba)建立正排索引和倒排索引、提供搜索功能、利用[cpp-httplib](https://github.com/yhirose/cpp-httplib)接受http请求并利用[jsoncpp](https://github.com/open-source-parsers/jsoncpp)序列化后的搜索结果.
- 网页端:Javascript:控制html页面动画效果、获取用户搜索内容、利用ajax将用户搜索内容作为http请求发送给服务端、获取服务端返回的json串并展示结果。

&emsp;&emsp;使用的第三方开源工具:[cpp准标准库boost](https://www.boost.org/)、[cppjieba](https://github.com/yanyiwu/cppjieba)、[jsoncpp](https://github.com/open-source-parsers/jsoncpp)、[cpp-httplib](https://github.com/yhirose/cpp-httplib)、[jQuery](https://github.com/jquery/jquery)。

&emsp;&emsp;对于实现的详细细节，可以看我的博客:[Router的博客](http://www.routeryt.com/)。

# 二、最终效果

![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724202018.png)

![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724202118.png)

# 三、如何在你的服务器上部署它

&emsp;&emsp;我们的服务端环境是Centos7.6，建议保持这个版本。

## I、安装boost库

&emsp;&emsp;一条shell命令:
```shell
sudo yum install boost-devel -y
```

## II、安装jsoncpp

&emsp;&emsp;一条shell命令:
```shell
sudo yum install -y jsoncpp-devel
```

## III、更新gcc

&emsp;&emsp;因为cpp-httplib的源代码用了些比较新的cpp语法，而centos7.6原版的gcc版本为4.8.5，需要至少更新到7，可以使用以下shell命令:
```shell
sudo yum -y install centos-release-scl scl-utils-build
sudo yum install -y devtoolset-9-gcc devtoolset-9-gcc-c++
scl enable devtoolset-9 bash // 仅本次对话有效
```

## IV、编译项目

&emsp;&emsp;使用make：
```shell
make clean;make
```

## V、运行

&emsp;&emsp;我们这里使用的是712端口，你也可以在http_sever.cc文件中在``svr.listen("0.0.0.0", port)``调整端口为其他端口，记得要开放你选的端口。

&emsp;&emsp;然后使用以下shell命令：

```shell
./parser// 数据清洗
nohup ./http_server > log/log.txt 2>&1 &// 后台运行服务端程序
```

&emsp;&emsp;在浏览器中输入ip:port即可访问。

## VI、更新搜索引擎的网页资源

&emsp;&emsp;下载网页的html文件到目录``newhtml``下，命名html文件为``url.html``，这里的``url``不包括诸如``http://``这种前缀。

&emsp;&emsp;关闭原进程并重新运行：

```shell
// 启动更新程序
./update
// 利用PID关掉原进程
// Through ps -axj | head -1 && ps -axj | grep '/ http_ server'command find PID off
// 重启服务
nohup ./http_server > log/log.txt 2>&1 &// restart
```







