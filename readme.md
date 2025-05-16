# LL_Chat

一个基于 Qt 和 WebSocket 的远程聊天程序，支持注册/登录、好友添加（双向确认）、点对点实时聊天功能。项目分为 `client`（客户端）和 `server`（服务端）两个子目录，使用 CMake 构建。

## 目录结构

```
LL_Chat/
├── client/ 客户端代码（Qt GUI + WebSocketClient）
│ ├── AddFriendDialog.* 好友管理对话框（发送/接收好友申请、同意）
│ ├── BaseWindow.h 窗口基类
│ ├── ChatWindow.* 主聊天窗口
│ ├── ConnectionDialog.* 服务器连接对话框（自动重试）
│ ├── CMakeLists.txt 客户端构建脚本
│ ├── LoginWindow.* 登录界面
│ ├── main.cpp 客户端入口
│ ├── RegisterWindow.* 注册界面（含确认密码、显示密码）
│ └── WebSocketClient.* WebSocket 封装
├── server/ 服务端代码（Qt WebSocketServer + 纯控制台）
│ ├── ChatRoom.* 消息分发与在线会话管理
│ ├── ChatServer.* WebSocket 服务器入口与连接管理
│ ├── ClientConnection.* 单个客户端连接与消息处理
│ ├── UserManager.* 用户注册/登录/好友管理逻辑
│ ├── CMakeLists.txt 服务端构建脚本
│ └── main.cpp 服务端入口
└── README.md 项目说明文档
```

## 功能特性

- **用户管理**  
  - 注册（含“确认密码”、“显示/隐藏密码”）  
  - 登录（用户名/密码验证）  
- **好友系统**  
  - 发送好友申请（不能添加自己、不能重复申请）  
  - 查看好友申请列表  
  - 同意好友申请（双向添加）  
- **聊天功能**  
  - 点对点实时文字聊天  
  - 切换联系人仅显示与当前联系人的历史记录  
- **连接管理**  
  - 客户端启动时自动尝试连接服务器，支持重试  
- **技术栈**  
  - C++20、Qt6（Widgets, WebSockets）  
  - CMake 构建  
  - Alibaba Cloud 3（服务端部署环境示例）

## 环境与依赖

- **Qt**: ≥ 6.0，需包含 Widgets 和 WebSockets 模块  
- **CMake**: ≥ 3.16  
- **编译器**: 支持 C++17 的 GCC/Clang/MSVC  
- （可选）部署服务器需要放行 WebSocket 端口（默认 9000）

## 构建与运行

###  克隆仓库

```bash
git clone https://github.com/yizhiyouling/LL_Chat.git
cd LL_Chat
```

### 构建服务端

```
cd server
mkdir build && cd build
cmake .. 
cmake --build .    # 或者使用 make / cmake --build . --config Release
# 运行
./server          # 默认监听 ws://localhost:9000

```

### 构建客户端

在新终端或标签页中：

```
bash复制编辑cd LL_Chat/client
mkdir build && cd build
cmake .. 
cmake --build .
# 运行
./client          # 会弹出“连接服务器”对话框，输入服务端地址后登录
```

**提示**：如果服务端不在本地，请在客户端弹出的连接框中输入 `ws://<服务器IP或域名>:9000`。

## 使用说明

1. 启动服务端后，运行客户端。
2. 在“连接服务器”对话框中确认能连通后，进入登录/注册界面。
3. 注册新用户时，输入用户名、密码和确认密码；可勾选“显示密码”。
4. 登录成功后进入主聊天窗口：
   - 左侧联系人列表：双击联系人即可查看并发送消息。
   - 点击“添加好友”打开好友管理对话框，左侧发起申请，右侧同意申请。
5. 好友双方添加成功后，自动刷新联系人列表并可开始聊天。