##   SimpleGL

#### 一.简介

​	SimpleGL是一个基于QT（https://www.qt.io/） 和openGL编写的静态场景渲染程序，它具备如下的功能：

- 通过json格式的配置文件在场景中定义一些简单的几何体（球，立方体，圆柱等），设置它们的基本参数（旋转平移，颜色，材质），以及设置天空盒。
- 加载外部静态模型文件（如.obj）或者带有骨骼动画的模型文件（如.fbx）并在场景中渲染或生成动画。
- 利用CPU对静态场景以光线追踪的方式进行渲染

#### 二.效果展示

​	**骨骼动画**

 <img src="img\animation.gif" width="500px" />

​	**光线追踪**

 <img src="img\RT1.png" alt="RT1" width="500px" />

#### 三.环境搭建

​	本项目使用CMake进行搭建，需要的最低cmake版本为3.1.0。本项目有如下的几个依赖库，其中大部分的库在项目根目录下的`libraries`文件夹下：

- assimp：提供模型文件加载和解析的功能，需要编译成静态链接库
- eigen：一个C++线性代数运算库，纯头文件库，不需要单独编译
- jsoncpp：提供json解析的功能，需要编译成静态链接库
- libigl：C++几何处理库，不需要单独编译
- qt5：提供UI界面显示以及封装过的opengl函数，需要自行下载和安装

需要编译成静态链接库的两个库`assimp`和`jsoncpp`已经在cmake中设置好了与主项目的依赖关系，不必单独从源代码构建。

​	以下的具体流程基于Windows操作系统和Visual Studio开发环境，如果你是其它系统和开发环境，可以遵循前几步cmake构建的部分，后面的步骤可能不完全一致。

###### 	具体搭建流程：

1. 在项目根目录下运行cmake（强烈推荐使用cmake-gui）

2. 如果显示==Qt5_DIR-NOTFOUND==，那么设置Qt5_DIR变量为`QtConfig.cmake`所在的文件夹路径，一般来说为`qt根目录/5.X.X/编译器名称/lib/cmake/Qt5`。例如我使用的是VS2019，那么该变量为`D:/develop/Qt5.15/5.15.2/msvc2019_64/lib/cmake/Qt5`

3. 关闭依赖库中一些不必要的构建，其中的一部分源代码为了节省空间被删除了，如果不取消会报错，需要设为false的选项如下：

   - ASSIMP_BUILD_ASSIMP_TOOLS（必须）
   - ASSIMP_BUILD_TESTS（必须）
   - BUILD_OBJECT_LIBS
   - BUILD_SHARED_LIBS
   - BUILD_TESTING
   - JSONCPP_WITH_TESTS

4. 构建项目并打开

5. 设置zlibstatic项目的警告等级为3（/W3），否则可能无法通过编译

6. 设置配置为Release x64模式，设置simpleGL为启动项目。

7. 在构建目录的`bin`文件夹下进行qt部署，windows下的命令为（需要先打开相应编译器的qt命令行程序）:

   ```
   windeployqt --release <your exe>
   ```

8. 在开发环境中运行程序

#### 四.命令行选项

```
simpleGL [-useRT {0|1}] [-t thread_num] [-scene scene_filename]
```

-useRT 是否使用光线追踪渲染 0 - 否 1 - 是，默认为0

-t 在使用光线追踪时，调度的CPU线程个数，最少为1，默认为4

-scene 场景文件路径，默认为`resources/scene/MyScene.json`

**暂不支持利用光线追踪对骨骼动画进行渲染**
