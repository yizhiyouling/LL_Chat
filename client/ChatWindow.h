#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include "WebSocketClient.h"

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

private:
    QString m_username;
    WebSocketClient *m_client;

    QListWidget *m_contactList;
    QListWidget *m_chatView;
    QLineEdit *m_messageEdit;
    QPushButton *m_sendBtn;
    QPushButton *m_addFriendBtn;
};

#endif // CHATWINDOW_H
