#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include "WebSocketClient.h"

class QLineEdit;
class QPushButton;
class QListWidget;

class AddFriendDialog : public QDialog {
    Q_OBJECT
public:
    AddFriendDialog(const QString &currentUser, WebSocketClient *client, QWidget *parent = nullptr);
    ~AddFriendDialog() override;

private slots:
    void onAddClicked();
    void onRefreshRequests();
    void onAcceptClicked();
    void onServerMessage(const QJsonObject &obj);

private:
    QString m_currentUser;
    WebSocketClient *m_client;

    // 左侧：添加好友
    QLineEdit   *m_friendEdit;
    QPushButton *m_addBtn;

    // 右侧：待处理申请列表及按钮
    QListWidget *m_requestList;
    QPushButton *m_acceptBtn;
};

#endif // ADDFRIENDDIALOG_H
