# [ChatRSA](https://atomicbool.github.io/ChatRSA/)

<p align="center">
    <picture>
        <img src="./icon.png" alt="OpenClaw" width="120">
    </picture>
</p>

<p align="center">
  <strong>一个本地聊天软件加密层</strong>
</p>

<p align="center">
  <a href="https://github.com/AtomicBool/ChatRSA/releases"><img src="https://img.shields.io/github/v/release/atomicbool/ChatRSA?include_prereleases&style=for-the-badge" alt="GitHub release"></a>
</p>  
原本2025年底有的想法, 鸽了半年。  
得亏了gemini-cli和gpt的协作，三天内写完了  
但感觉架构有点问题, 后续可能不是太有时间维护

## 下载
可以使用 Visual Studio 2026 直接编译  

也可以下载最新[预编译文件](https://github.com/AtomicBool/ChatRSA/releases/latest/download/ChatRSA.exe)(可能不如直接编译新)

## 添加联系人

1. 和你的朋友交换终端打印出来的`public key`
2. `打开ui` -> `添加联系人`
4. 输入朋友的`名字`, 并粘贴`public key`并保存

*上述步骤只用做一次*

## 聊天
每次换人聊天就 `打开ui` -> `选择那个联系人`  
- 打完消息后按`F4`, 软件会按照你选定那个人的public key加密, 流程如下: (如果不适用于你的聊天软件可能需要修改一下源码)
```
全选
复制
读取剪切板
加密剪切板里的内容
粘贴
```
- 对方发来消息后按`F3`, 程序会自动按下按键来执行如下流程 (如果不适用于你的聊天软件可能需要修改一下源码)
```
全选
复制
读取剪切板
解密剪切板里的内容
在浮窗上显示
```

## 热键
F2 - 打开/关闭 UI(打开时鼠标无法穿透ui)
F3 - 解密
F4 - 加密