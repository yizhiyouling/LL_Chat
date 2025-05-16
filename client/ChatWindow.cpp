#include "ChatWindow.h"
#include "AddFriendDialog.h"

#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonArray>
#include <QMessageBox>

ChatWindow::ChatWindow(const QString &username, WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_username(username),
    m_client(client)
{
    setWindowTitle(QString("LL_Chat — %1").arg(username));
    resize(600, 400);

    QLabel *lblContacts = new QLabel("联系人", this);
    m_contactList = new QListWidget(this);

    QLabel *lblChat = new QLabel("聊天窗口", this);
    m_chatView = new QListWidget(this);

    m_messageEdit = new QLineEdit(this);
    m_sendBtn = new QPushButton("发送", this);
    m_addFriendBtn = new QPushButton("添加好友", this);

    auto *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(lblContacts);
    leftLayout->addWidget(m_contactList);
    leftLayout->addWidget(m_addFriendBtn);

    auto *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(lblChat);
    rightLayout->addWidget(m_chatView);
    auto *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(m_messageEdit);
    inputLayout->addWidget(m_sendBtn);
    rightLayout->addLayout(inputLayout);

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 3);

    connect(m_sendBtn, &QPushButton::clicked, this, &ChatWindow::onSendClicked);
    connect(m_addFriendBtn, &QPushButton::clicked, this, &ChatWindow::onAddFriendClicked);
    connect(m_client, &WebSocketClient::messageReceived,
            this, &ChatWindow::onServerMessage);

    // 请求联系人列表
    QJsonObject req;
    req["type"] = "get_contacts";
    req["username"] = m_username;
    m_client->sendJson(req);
}

ChatWindow::~ChatWindow() = default;

void ChatWindow::onSendClicked()
{
    if (!m_contactList->currentItem()) {
        QMessageBox::warning(this, "提示", "请先选择联系人！");
        return;
    }
    const QString to = m_contactList->currentItem()->text();
    const QString msg = m_messageEdit->text().trimmed();
    if (msg.isEmpty()) return;

    QJsonObject req;
    req["type"] = "chat";
    req["from"] = m_username;
    req["to"] = to;
    req["message"] = msg;
    m_client->sendJson(req);

    m_chatView->addItem(QString("我 -> %1: %2").arg(to, msg));
    m_messageEdit->clear();
}

void ChatWindow::onAddFriendClicked()
{
    AddFriendDialog dlg(m_username, m_client, this);
    if (dlg.exec() == QDialog::Accepted) {
        QJsonObject req;
        req["type"] = "get_contacts";
        req["username"] = m_username;
        m_client->sendJson(req);
    }
}

void ChatWindow::onServerMessage(const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();
    if (type == "chat") {
        const QString from = obj.value("from").toString();
        const QString msg  = obj.value("message").toString();
        m_chatView->addItem(QString("%1: %2").arg(from, msg));
    }
    else if (type == "contacts_list") {
        m_contactList->clear();
        for (auto v : obj.value("list").toArray())
            m_contactList->addItem(v.toString());
    }
}
