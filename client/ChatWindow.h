#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include "WebSocketClient.h"
#include <QMap>
#include <QListWidgetItem>
#include <QStringList>

class QListWidget;
class QLineEdit;
class QPushButton;

// ChatWindow: 主聊天界面，包含联系人列表、聊天视图和消息输入
class ChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatWindow(const QString &username, WebSocketClient *client, QWidget *parent = nullptr);
    ~ChatWindow() override;

private slots:
    void onSendClicked();                        // 点击“发送”时调用
    void onAddFriendClicked();                   // 点击“添加好友”时打开对话框
    void onServerMessage(const QJsonObject &obj);// 处理从服务器接收的消息
    void onContactSelected(QListWidgetItem *item);// 选中联系人时切换聊天记录

private:
    QString m_username;                          // 当前登录用户名
    WebSocketClient *m_client;                   // WebSocket 客户端实例

    QListWidget *m_contactList;                  // 联系人列表控件
    QListWidget *m_chatView;                     // 聊天记录显示控件
    QLineEdit   *m_messageEdit;                  // 消息输入框
    QPushButton *m_sendBtn;                      // 发送按钮
    QPushButton *m_addFriendBtn;                 // 添加好友按钮

    QMap<QString, QStringList> m_history;        // 存储每个联系人对应的聊天历史
    void showHistoryFor(const QString &contact); // 切换联系人时展示对应聊天历史
};

#endif
