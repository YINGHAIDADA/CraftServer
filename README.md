# CraftServer
从头写一个c++高性能服务器框架


项目采用Premake5管理，可以很简单的生成Makefile文件，管理依赖
```shell
//使用命令生成Makefile文件
premake5 --os=linux gmake

//make来生成项目
make config=debug Test
```