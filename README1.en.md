# What's this ?

A mini **search engine** cpp program for www.boost.org , because this website has no search function.

Use open source tools : boost、cppjieba、jsoncpp、cpp-httplib、jQuery.

Language:

**C++** : clean data source(HTML tags)、build forward index & inverted index、provide search function、build httpServer.



**Javascript** : Animate page、Get Search word、use Ajax send search word to httpserver、Get jsonstring and Generate search results page.

For details, see [My CSDN BLOG(Language:Chinese)](https://blog.csdn.net/CS_COPy/article/details/125965699).

# Final results
![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724202018.png)

![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724202118.png)

# How to use it ?
First, our environment is Centos7.6, Your server should try to maintain this version.
## I.Install boost
```shell
sudo yum install boost-devel -y
```
## II.Install jsoncpp
```shell
sudo yum install -y jsoncpp-devel
```
## III.Upgrade gcc
```shell
sudo yum -y install centos-release-scl scl-utils-build
sudo yum install -y devtoolset-9-gcc devtoolset-9-gcc-c++

scl enable devtoolset-9 bash // only this session
```

## IV.Compile project
```shell
make clean;make
```

## V.Run
We use 712 port. You should open this port?(in default).

You can also set other port in http_sever.cc
```shell
./parser
nohup ./http_server > log/log.txt 2>&1 &
```

Enter ServerIp:port in the browser to search.

## VI.Update web resources of search engines

Download the HTML file of the target web page and put it in the newhtml folder.

Name the HTML file "url.html" (url does not contain a prefix like http://)

run 
```shell
./update
// close http_sever 
// Through ps -axj | head -1 && ps -axj | grep '/ http_ server'command find PID off

nohup ./http_server > log/log.txt 2>&1 &// restart
```



