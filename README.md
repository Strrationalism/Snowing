# Snowing

弦语蝶梦 第二代游戏引擎 Snowing

[![Build status](https://ci.appveyor.com/api/projects/status/33si48fa3f2tbt1y?svg=true)](https://ci.appveyor.com/project/SmallLuma/snowing)

[快速入门](https://www.bilibili.com/video/av43701922/)

- Core - 引擎核心代码
- WindowsImpl - Windows下的实现
- AssetsBuilder - 资源构建器
- WindowsSteamSDK - 引擎的Steam扩展

## 资源构建器的用法
AssetsBuilder 目标路径
它将读取工作目录下的BuildScript.txt，根据此文件构建内容。
关于BuildScript.txt的写法，参见Assets\BuildScript.txt例子。

需要安装Erget TextureMerger到X:\Program Files\Egret\TextureMerger\下。
X可以为任意盘符，该工具用于打包纹理。

## 如何配置Steam扩展的编译环境
下载SteamworksSDK，并将其解压到WindowsSteamSDK\SteamSDK，使得WindowsSteamSDK\SteamSDK\public目录可用。

## 关于Effects11
使用子模块克隆后，如果提示找不到WinSDK，右击重定向到8.1 SDK。

## 关于Windows下的XAudio2
对于Snowing引擎来说，音频是可选模块。
如果需要使用Windows下的XAudio2，需要在包含PlatformImpls.h之前，定义宏USE_XAUDIO2。
