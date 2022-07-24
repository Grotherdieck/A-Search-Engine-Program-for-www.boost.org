# A-Search-Engine-Program-for-www.boost.org
A mini search engine cpp program for www.boost.org ,because this website has no search function.
Use open source tools : boost、cppjieba、jsoncpp、cpp-httplib、jQuery.
For details, see [My CSDN](https://blog.csdn.net/CS_COPy/article/details/125965699)

# Final results
![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724202018.png)

![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724202118.png)

![](https://router-picture-bed.oss-cn-chengdu.aliyuncs.com/img/20220724173715.png)

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


