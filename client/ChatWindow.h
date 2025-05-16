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

class ChatWindow : public QWidget {
    Q_OBJECT
public:
    explicit ChatWindow(const QString &username, WebSocketClient *client, QWidget *parent = nullptr);
    ~ChatWindow() override;

private slots:
    void onSendClicked();
    void onAddFriendClicked();
    void onServerMessage(const QJsonObject &obj);
    void onContactSelected(QListWidgetItem *item);  // 新增

private:
    QString m_username;
    WebSocketClient *m_client;

    QListWidget *m_contactList;
    QListWidget *m_chatView;
    QLineEdit   *m_messageEdit;
    QPushButton *m_sendBtn;
    QPushButton *m_addFriendBtn;

    // —— 新增：按联系人存储聊天历史 ——
    QMap<QString, QStringList> m_history;
    void showHistoryFor(const QString &contact);   // 新增
};

#endif // CHATWINDOW_H
