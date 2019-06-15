# Enjoy-The-Beach
SYSU CG Final Project -- Group 11

## 简介

此项目实现了一个在沙滩与雪山场景下的游戏，用户通过穿过场景物体来进行场景切换。玩家通过 `WASD` 键在场景中进行移动，按下`3`键可切换场景中的光源，按下`1`与`2`键可以关闭与开启通过粒子系统实现的飞雪效果。

## Build & Run

通过如下命令拉取项目与依赖的源码：

```shell
git clone https://github.com/sysu-cg-group11/Enjoy-The-Beach.git --recursive
```

项目可以在 Windows 或 macOS 下运行，使用 cmake 进行构建，在 macOS 下的构建与运行的命令如下：

```shell
mkdir build && cd build
cmake ..
make -j 8
./bin/Enjoy-Beach-macos
```

在 Windows 下创建并进入 build 目录后，在目录下通过如下命令创建 Visual Studio 解决方案：

```shell
cmake .. -G "Visual Studio 15 2017 Win64"
```

创建后，将`build\dependencies\assimp\include\assimp\config.h`拷贝至项目根目录中的`dependencies\assimp\include\assimp`下。最后打开解决方案将`Enjoy-Beach-windows`设为启动项目便可编译运行。

## 实现知识点

1. **Camera Roaming**

   用户通过键盘输入移动摄像机的位置，并可通过鼠标移动来控制摄像机视角，以实现从不同角度观察场景。

2. **Light and Shading**

   场景中添加了太阳光源，使用了 Blinn-Phong 光照模型来为场景添加光照。对同一个视角切换光照位置的效果如下：

   *沙滩：*

   ![](screenshoot/light-1.png)

   ![](screenshoot/light-2.png)

   *雪山：*

   ![](screenshoot/light-3.png)

   ![](screenshoot/light-4.png)

3. **Texture Mapping: **

   为模型添加不同的纹理，使模型可显示出更加真实的表面细节：

   ![](screenshoot/texture-1.png)

4. **Model import & Mesh viewing:**

   