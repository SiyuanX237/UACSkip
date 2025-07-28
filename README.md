# UACSkip

### 新版UAC白名单工具，支持UI配置界面
### 作者：[Bilibili - 个人隐思](https://space.bilibili.com/1081364881 "来我主页玩玩ヾ(^∀^)ﾉ")
### 爱发电主页：[ThinkAlone](https://afdian.com/a/X1415 "您赞助的每一分都是我前进的动力")
编程不易，打赏随意：

<img src="/images/afdian-ThinkAlone.jpg" height="300" /> <img src="/images/mm_reward.png" height="300" />

## 功能特点

 - **使用纯Win32编写**：确保高性能与兼容性
 - **利用任务计划**：保证高兼容性和可用性
 - **独创带环境变量Lnk创建**：确保软件便携性，方便后续管理维护
 - **利用共享内存IPC通信**：实现免临时文件创建传参，提高了并发性

## 快速开始

#### 任务计划模式：
![image](/images/config.png)
 - 首次使用先安装服务
 - 再选取一个exe、bat或lnk文件，并调整参数
 - 点击创建按钮即可快捷生成免弹窗快捷方式

配置完成后可在任意需要管理员权限的程序路径前加上 **%UACSkip%** 以调用本功能
![image](/images/application.png)

```c++
%UACSkip% 任意程序 任意参数
```
我们还支持把功能集成到exe右键上

<img src="/images/before.png" height="300" /> => <img src="/images/after.png" height="300" />

注册表位置如下：
![image](/images/reg-detail.png)

**修复操作：**
如果您有移动本程序的需求，需要再次运行本程序并执行修复操作即可，您之前配置的快捷方式及%UACSkip%快捷调用将不受影响（环境变量的修改可能需要重启以更新路径数据）

<img src="/images/repair1.png" height="200" /> <img src="/images/repair2.png" height="200" />

#### 注册表模式（不推荐）
![image](/images/regmode.png)
如果您有软件属实用不到管理员权限，但每次运行还是需要授权，我们可以用这个方法强制剥夺它的这个权利，同样可以免弹

如果需要移除，需要进入注册表并删除该项，即刻生效：
![image](/images/regedit.png)
## 视频介绍

[新一代UAC白名单工具](https://www.bilibili.com/video/BV1hpudz7EN1/)


# 祝您使用愉快！
