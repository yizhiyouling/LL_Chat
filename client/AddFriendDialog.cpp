#include "AddFriendDialog.h"
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QJsonArray>

AddFriendDialog::AddFriendDialog(const QString &currentUser,
                                 WebSocketClient *client,
                                 QWidget *parent)
    : QDialog(parent),
    m_currentUser(currentUser),
    m_client(client)
{
    setWindowTitle("好友管理");
    resize(400, 200);

    // 左：添加好友
    QLabel *lbl = new QLabel("好友用户名:", this);
    m_friendEdit = new QLineEdit(this);
    m_addBtn     = new QPushButton("添加", this);
    connect(m_addBtn, &QPushButton::clicked, this, &AddFriendDialog::onAddClicked);

    auto *leftLay = new QVBoxLayout;
    leftLay->addWidget(lbl);
    leftLay->addWidget(m_friendEdit);
    leftLay->addWidget(m_addBtn);

    // 右：请求列表
    QLabel *lblReq = new QLabel("好友申请:", this);
    m_requestList = new QListWidget(this);
    m_acceptBtn   = new QPushButton("同意", this);
    connect(m_acceptBtn, &QPushButton::clicked, this, &AddFriendDialog::onAcceptClicked);

    auto *rightLay = new QVBoxLayout;
    rightLay->addWidget(lblReq);
    rightLay->addWidget(m_requestList);
    rightLay->addWidget(m_acceptBtn);

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLay, 1);
    mainLayout->addLayout(rightLay, 1);

    // 接收服务端推送
    connect(m_client, &WebSocketClient::messageReceived,
            this, &AddFriendDialog::onServerMessage);

    // 首次拉取申请列表
    onRefreshRequests();
}

AddFriendDialog::~AddFriendDialog() = default;

void AddFriendDialog::onAddClicked()
{
    const QString to = m_friendEdit->text().trimmed();
    if (to.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入好友用户名！");
        return;
    }
    QJsonObject req{
        {"type", "add_friend"},
        {"from", m_currentUser},
        {"to",   to}
    };
    m_client->sendJson(req);
}

void AddFriendDialog::onRefreshRequests()
{
    QJsonObject req{
        {"type",     "get_requests"},
        {"username", m_currentUser}
    };
    m_client->sendJson(req);
}

void AddFriendDialog::onAcceptClicked()
{
    auto *item = m_requestList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "提示", "请选择一条申请！");
        return;
    }
    QString from = item->text();
    QJsonObject req{
        {"type", "accept_friend"},
        {"user", m_currentUser},
        {"from", from}
    };
    m_client->sendJson(req);
}

void AddFriendDialog::onServerMessage(const QJsonObject &obj)
{
    QString type = obj.value("type").toString();
    if (type == "add_friend_result") {
        bool ok = obj.value("success").toBool();
        const QString reason = obj.value("reason").toString();
        QMessageBox::information(this, ok ? "添加发送成功" : "添加发送失败", ok ? "申请已发送" : reason);
    }
    else if (type == "requests_list") {
        m_requestList->clear();
        for (auto v : obj.value("list").toArray())
            m_requestList->addItem(v.toString());
    }
    else if (type == "friend_request") {
        // 有新申请时刷新列表
        onRefreshRequests();
    }
    else if (type == "accept_friend_result") {
        bool ok = obj.value("success").toBool();
        const QString reason = obj.value("reason").toString();
        if (ok) {
            QMessageBox::information(this, "添加成功", "已成为好友");
            accept();  // 关闭对话框
        } else {
            QMessageBox::warning(this, "添加失败", reason);
        }
    }
}
