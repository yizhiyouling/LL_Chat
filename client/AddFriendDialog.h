#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include "WebSocketClient.h"

class QLineEdit;
class QPushButton;
class QListWidget;

// AddFriendDialog: 好友管理对话框，支持发送申请、刷新申请列表、同意申请
class AddFriendDialog : public QDialog {
    Q_OBJECT

public:
    AddFriendDialog(const QString &currentUser, WebSocketClient *client, QWidget *parent = nullptr);
    ~AddFriendDialog() override;

private slots:
    void onAddClicked();           // 点击“添加”按钮时发送好友申请
    void onRefreshRequests();      // 刷新并获取最新的好友申请列表
    void onAcceptClicked();        // 点击“同意”按钮时同意选中的好友申请
    void onServerMessage(const QJsonObject &obj);  // 接收并处理服务器返回的 JSON 消息

private:
    QString m_currentUser;         // 当前登录用户名
    WebSocketClient *m_client;     // WebSocket 客户端实例

    // 添加好友输入框和按钮
    QLineEdit   *m_friendEdit;
    QPushButton *m_addBtn;

    // 待处理申请列表和同意按钮
    QListWidget *m_requestList;
    QPushButton *m_acceptBtn;
};

#endif
