#include "AddFriendDialog.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

AddFriendDialog::AddFriendDialog(const QString &currentUser,
                                 WebSocketClient *client,
                                 QWidget *parent)
    : QDialog(parent),
    m_currentUser(currentUser),
    m_client(client)
{
    setWindowTitle("添加好友");
    resize(300, 120);

    QLabel *lbl = new QLabel("好友用户名:", this);
    m_friendEdit = new QLineEdit(this);
    m_addBtn    = new QPushButton("添加", this);
    m_cancelBtn = new QPushButton("取消", this);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(lbl);
    mainLayout->addWidget(m_friendEdit);
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_addBtn, &QPushButton::clicked, this, &AddFriendDialog::onAddClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddFriendDialog::onCancelClicked);
    connect(m_client, &WebSocketClient::messageReceived,
            this, &AddFriendDialog::onServerMessage);
}

AddFriendDialog::~AddFriendDialog() = default;

void AddFriendDialog::onAddClicked()
{
    const QString to = m_friendEdit->text().trimmed();
    if (to.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入好友用户名！");
        return;
    }
    QJsonObject req;
    req["type"] = "add_friend";
    req["from"] = m_currentUser;
    req["to"]   = to;
    m_client->sendJson(req);
}

void AddFriendDialog::onCancelClicked()
{
    reject();
}

void AddFriendDialog::onServerMessage(const QJsonObject &obj)
{
    if (obj.value("type").toString() != "add_friend_result")
        return;

    bool ok = obj.value("success").toBool();
    const QString reason = obj.value("reason").toString("未知错误");
    if (ok) {
        QMessageBox::information(this, "添加成功", "好友已添加");
        accept();
    } else {
        QMessageBox::warning(this, "添加失败", reason);
    }
}
