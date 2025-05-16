#include "ChatWindow.h"
#include "AddFriendDialog.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

ChatWindow::ChatWindow(const QString &username, WebSocketClient *client, QWidget *parent)
    : QWidget(parent),
    m_username(username),
    m_client(client)
{
    setWindowTitle(QString("LL_Chat — %1").arg(username));
    resize(600, 400);

    // 创建控件
    m_contactList   = new QListWidget(this);
    m_chatView      = new QListWidget(this);
    m_messageEdit   = new QLineEdit(this);
    m_sendBtn       = new QPushButton("发送", this);
    m_addFriendBtn  = new QPushButton("添加好友", this);

    // 布局
    auto *mainLayout  = new QHBoxLayout(this);
    auto *leftLayout  = new QVBoxLayout();
    auto *rightLayout = new QVBoxLayout();
    auto *inputLayout = new QHBoxLayout();

    leftLayout->addWidget(new QLabel("联系人："));
    leftLayout->addWidget(m_contactList);
    leftLayout->addWidget(m_addFriendBtn);

    rightLayout->addWidget(new QLabel("聊天记录："));
    rightLayout->addWidget(m_chatView);
    inputLayout->addWidget(m_messageEdit);
    inputLayout->addWidget(m_sendBtn);
    rightLayout->addLayout(inputLayout);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 2);
    setLayout(mainLayout);

    // 信号与槽
    connect(m_sendBtn, &QPushButton::clicked, this, &ChatWindow::onSendClicked);
    connect(m_addFriendBtn, &QPushButton::clicked, this, &ChatWindow::onAddFriendClicked);
    connect(m_contactList, &QListWidget::itemClicked, this, &ChatWindow::onContactSelected);
    connect(m_client, &WebSocketClient::messageReceived, this, &ChatWindow::onServerMessage);

    // 初始化：请求联系人列表
    QJsonObject reqContacts{ {"type", "get_contacts"}, {"username", m_username} };
    m_client->sendJson(reqContacts);
}

ChatWindow::~ChatWindow() = default;

void ChatWindow::onSendClicked()
{
    auto *item = m_contactList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "提示", "请先选择联系人！");
        return;
    }

    QString to  = item->text();
    QString msg = m_messageEdit->text().trimmed();
    if (msg.isEmpty()) return;

    // 发送消息
    QJsonObject reqChat{
        {"type",    "chat"},
        {"from",    m_username},
        {"to",      to},
        {"message", msg}
    };
    m_client->sendJson(reqChat);

    // 本地保存并显示
    m_history[to].append(QString("我 -> %1: %2").arg(to, msg));
    showHistoryFor(to);
    m_messageEdit->clear();
}

void ChatWindow::onAddFriendClicked()
{
    AddFriendDialog dlg(m_username, m_client, this);
    dlg.exec();
}

void ChatWindow::onContactSelected(QListWidgetItem *item)
{
    if (!item) return;
    showHistoryFor(item->text());
}

void ChatWindow::showHistoryFor(const QString &contact)
{
    m_chatView->clear();
    for (const QString &line : m_history.value(contact)) {
        m_chatView->addItem(line);
    }
}

void ChatWindow::onServerMessage(const QJsonObject &obj)
{
    QString type = obj.value("type").toString();

    if (type == "chat") {
        QString from = obj.value("from").toString();
        QString msg  = obj.value("message").toString();
        QString line = QString("%1: %2").arg(from, msg);
        m_history[from].append(line);

        // 只有在当前聊天窗口与该用户时才刷新
        auto *cur = m_contactList->currentItem();
        if (cur && cur->text() == from) {
            showHistoryFor(from);
        }
    }
    else if (type == "contacts_list") {
        m_contactList->clear();
        for (const auto &v : obj.value("list").toArray()) {
            m_contactList->addItem(v.toString());
        }
    }
    else if (type == "friend_request") {
        QString from = obj.value("from").toString();
        QMessageBox::information(this, "好友请求",
                                 QString("用户 %1 请求添加你为好友！").arg(from));
    }
    else if (type == "add_friend_result") {
        bool ok = obj.value("success").toBool();
        QString reason = obj.value("reason").toString();
        QMessageBox::information(this,
                                 ok ? "申请已发送" : "申请失败",
                                 ok ? "好友申请已发送" : reason);
    }
    else if (type == "accept_friend_result") {
        bool ok = obj.value("success").toBool();
        QString reason = obj.value("reason").toString();
        if (ok) {
            QMessageBox::information(this, "已成为好友", "你已同意好友请求！");
            // 主动拉取最新联系人列表
            QJsonObject req{ {"type", "get_contacts"}, {"username", m_username} };
            m_client->sendJson(req);
        } else {
            QMessageBox::warning(this, "同意失败", reason);
        }
    }
    else if (type == "friend_accepted") {
        QString from = obj.value("from").toString();
        QMessageBox::information(this, "好友通过",
                                 QString("用户 %1 接受了你的好友申请！").arg(from));
        // 主动拉取最新联系人列表
        QJsonObject req{ {"type", "get_contacts"}, {"username", m_username} };
        m_client->sendJson(req);
    }
}
