# Snowing

弦语蝶梦 第二代游戏引擎 Snowing   

[![Build status](https://ci.appveyor.com/api/projects/status/xsg86whgh0d00lma?svg=true)](https://ci.appveyor.com/project/SmallLuma/snowing)

[快速入门](https://www.bilibili.com/video/av43701922/)

### 基本组件
- Core - 引擎核心代码
- Assets - 基本素材与示例素材
- WindowsImpl - Windows下的实现
- AssetsBuilder - 素材转换、打包、部署工具
- TestWindow - Windows下的单元测试
- TestWindowsInput - Windows下的基本输入设备测试
- FX11(submodule) - Microsoft Effects 11 框架运行时


### 插件
- WindowsSteamSDK - Steam插件
- TestWindowsSteam - Steam插件的单元测试测试
- WindowsDInput - 用于手柄输入的DInput插件
- TestWindowsDInput - DInput插件的单元测试
- WindowsXInput - 用于手柄输入的XInput插件
- TestWindowsXInput - XInput插件的单元测试
- Yukimi - AVG游戏组件库
- TestYukimi - AVG游戏组件库的单元测试
- WindowsLive2D - Windows下的Live2D集成
- TestWindowsLive2D - Windows下的Live2D集成的测试
- Fyee - 动态BGM系统

### 范例
- BGMPlayer - BGM播放器
- HelloWorld - 基本范例
- Particle - 2D粒子范例
- TextRendering - 2D文本范例
- XInputControllerVibration - XInput控制器震动特效范例
- FyeeDemo - Fyee动态BGM系统的Demo

### FAQ

##### 我使用Visual Studio 2017来编译该项目时，需要安装哪些组件
可以使用Visual Studio 2017安装程序的“导入配置”，导入[Snowing_VS2017_Install_Config.vsconfig](Snowing_VS2017_Install_Config.vsconfig)，来安装所有需要的组件。

##### 关于git-lfs
Clone此项目需要确保安装[git-lfs](https://git-lfs.github.com/)，并已经执行以下命令：
```shell
git-lfs install
```

##### 资源构建器的用法
AssetsBuilder 目标路径
它将读取工作目录下的BuildScript.txt，根据此文件构建内容。
关于BuildScript.txt的写法，参见Assets\BuildScript.txt例子。

需要安装[Erget TextureMerger](https://www.egret.com/products/others.html#egret-texture)到X:\Program Files\Egret\TextureMerger\下。
X可以为任意盘符，该工具用于打包纹理。

##### 关于Effects11 SDK错误
使用子模块克隆后，如果提示找不到WinSDK，右击重定向到8.1 SDK。

##### 关于Windows下的XAudio2
对于Snowing引擎来说，音频是可选模块。
如果需要使用Windows下的XAudio2，需要在包含Snowing.h之前，定义宏USE_XAUDIO2。

##### Live2D支持遇到了编译错误
由于Live2D的开发商的要求，我们不能提前做好Live2D的开发环境，如果你不需要Live2D，可以直接右键点击卸载项目。
如果你需要使用Live2D支持，需要确保：
1. 从Live2D处获取了使用许可，弦语蝶梦团队不提供任何Live2D许可，也没受到Live2D的许可来开发游戏，仅仅做了引擎与Live2D的适配工作。
2. 如果你尚未获取许可，请前往[Live2D官网](https://www.live2d.com/en/products/releaselicense)获取使用许可。
3. 如果你已经获取许可，请前往[SDK下载页面](https://live2d.github.io/)下载“Cubism SDK for Native”。
4. 将其中的“Core”文件夹解压到WindowsLive2D目录下，使得WindowsLive2D\Core\include\Live2DCubismCore.h及其他文件可用。
5. 确保这两个Git Submodule可用：CubismNativeFramework、CubismNativeSamples
6. 编译运行TestWindowsLive2D，使得测试通过。
7. 在你的项目中引用WindowsLive2D项目。
