# Enjoy-The-Beach
SYSU CG Final Project -- Group 11

## 简介

此项目实现了一个在沙滩与雪山场景下的游戏，按下`3`键可切换场景中的光源，按下`1`与`2`键可以关闭与开启通过粒子系统实现的飞雪效果，按下`4`键可切换 Gamma 校正的效果。

## 玩法

玩家通过 `WASD` 键与鼠标控制鸟在场景中进行移动，在一个场景中找到另一场景中的4个关键物体后走到 [TARDIS(Time And Relative Dimension In Space)](https://en.wikipedia.org/wiki/TARDIS) 中即可切换到另一场景。

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

   场景中添加了太阳光源，使用了 Blinn-Phong 光照模型来为场景添加光照。

   对同一个视角切换光照位置后光照与阴影的效果如下：

   *沙滩：*

   ![](images/light-1.png)

   ![](images/light-2.png)

   *雪山：*

   ![](images/light-3.png)

   ![](images/light-4.png)

3. **Texture Mapping: **

   为模型添加不同的纹理，使模型可显示出更加真实的表面细节：

   ![](images/texture-1.png)

4. **Model import & Mesh viewing:**

   通过加载模型来实现两个场景中不同实体的实现。例如如下的 TARDIS：

   ![](images/tardis.png)

5. **Shadow Mapping:**

   通过光源视角渲染获得深度贴图，在渲染时可判断点是否在光照范围之内，从而生成阴影效果。在两个场景中的阴影效果如下：

   *沙滩：*

   ![](images/shadow-1.png)

   *雪山：*

   ![](images/shadow-2.png)

6. Gamma Correction: 

   可通过 OpenGL 内建的方式开启 Gamma Correction, 也可通过着色器启用 Gamma 校正，其效果分别如下：

   未开启 Gamma 校正时：

   ![](images/no-gamma.png)

   使用 OpenGL 内建的`GL_FRAMEBUFFER_SRGB`进行 Gamma 校正效果：

   ![](images/gl-gamma.png)

   在着色器中对纹理进行 Gamma 校正的效果：

   ![](images/shader-gamma.png)

7. 天空盒

8. 字体显示

9. 骨骼动画（沙滩上跑步的人）

10. 粒子系统（雪山场景中的落雪）

11. 流体模拟（场景周围的水，具有流动与倒影的效果）

      