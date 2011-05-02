lib_dll为第三方动态链接库
lis_static为静态链接库
src为源文件目录
vstudio10为Visual Studio 2010工程文件夹
Help为帮助文档
所有配置的Release版本均生成在bin目录下，Debug则在Debug目录

To compile NaviFirmEx, you should use vs2010
and, set Additional Library Directories to your lib folder
(Property>Configuration Properties>Linker>General>Additional Library Directories)