1. 由于本人对CMake的理解不深，目前只能手动添加CEF相应的头文件到项目中(将CEF里include文件夹整个拷贝到项目中，或者是修改CMakeLists.txt中引用的CEF头文件的路径)
2. CEF相应的库也要拷贝到项目文件夹下参考上面一条的头文件的处理
3. 项目为了做xp的兼容，目前我使用的CEF CHROME_VERSION_BUILD 为2623的，是最后一个支持XP的版本，和最新版本的CEF