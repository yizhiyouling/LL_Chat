#include "AddFriendDialog.h"
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QJsonArray>

// 构造函数：初始化 UI、信号/槽，并向服务器请求好友申请列表
AddFriendDialog::AddFriendDialog(const QString &currentUser,
                                 WebSocketClient *client,
                                 QWidget *parent)
    : QDialog(parent),
    m_currentUser(currentUser),
    m_client(client)
{
    // 设置窗口标题和大小
    setWindowTitle("好友管理");
    resize(400, 200);

    // 左侧布局：添加好友输入框和按钮
    QLabel *lbl = new QLabel("好友用户名:", this);
    m_friendEdit = new QLineEdit(this);
    m_addBtn     = new QPushButton("添加", this);
    connect(m_addBtn, &QPushButton::clicked, this, &AddFriendDialog::onAddClicked);

    QVBoxLayout *leftLay = new QVBoxLayout;
    leftLay->addWidget(lbl);
    leftLay->addWidget(m_friendEdit);
    leftLay->addWidget(m_addBtn);

    // 右侧布局：展示好友申请列表和同意按钮
    QLabel *lblReq = new QLabel("好友申请:", this);
    m_requestList = new QListWidget(this);
    m_acceptBtn   = new QPushButton("同意", this);
    connect(m_acceptBtn, &QPushButton::clicked, this, &AddFriendDialog::onAcceptClicked);

    QVBoxLayout *rightLay = new QVBoxLayout;
    rightLay->addWidget(lblReq);
    rightLay->addWidget(m_requestList);
    rightLay->addWidget(m_acceptBtn);

    // 主布局：左右并列
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLay, 1);
    mainLayout->addLayout(rightLay, 1);

    // 连接 WebSocketClient 的消息接收信号，处理服务器推送
    connect(m_client, &WebSocketClient::messageReceived,
            this, &AddFriendDialog::onServerMessage);

    // 首次打开对话框时，拉取好友申请列表
    onRefreshRequests();
}

// 析构函数
AddFriendDialog::~AddFriendDialog() = default;

// 点击“添加”按钮时发送好友申请请求
void AddFriendDialog::onAddClicked()
{
    QString to = m_friendEdit->text().trimmed();
    if (to.isEmpty()) {
        // 输入为空时提示
        QMessageBox::warning(this, "提示", "请输入好友用户名！");
        return;
    }

    // 构造 JSON 请求并发送
    QJsonObject req{
        {"type", "add_friend"},
        {"from", m_currentUser},
        {"to",   to}
    };
    m_client->sendJson(req);
}

// 向服务器请求当前用户的所有待处理好友申请
void AddFriendDialog::onRefreshRequests()
{
    QJsonObject req{
        {"type",     "get_requests"},
        {"username", m_currentUser}
    };
    m_client->sendJson(req);
}

// 点击“同意”按钮时发送同意申请请求
void AddFriendDialog::onAcceptClicked()
{
    QListWidgetItem *item = m_requestList->currentItem();
    if (!item) {
        // 未选中申请时提示
        QMessageBox::warning(this, "提示", "请选择一条申请！");
        return;
    }
    QString from = item->text();  // 申请者用户名

    // 构造 JSON 请求并发送
    QJsonObject req{
        {"type", "accept_friend"},
        {"user", m_currentUser},
        {"from", from}
    };
    m_client->sendJson(req);
}

// 处理来自服务器的各种消息类型
void AddFriendDialog::onServerMessage(const QJsonObject &obj)
{
    QString type = obj.value("type").toString();

    if (type == "add_friend_result") {
        // 处理好友申请发送结果
        bool ok = obj.value("success").toBool();
        QString reason = obj.value("reason").toString();
        QMessageBox::information(this,
                                 ok ? "申请已发送" : "申请失败",
                                 ok ? "发送成功" : reason);
    }
    else if (type == "requests_list") {
        // 更新待处理申请列表
        m_requestList->clear();
        for (const auto &v : obj.value("list").toArray()) {
            m_requestList->addItem(v.toString());
        }
    }
    else if (type == "friend_request") {
        // 收到新申请通知，刷新列表
        onRefreshRequests();
    }
    else if (type == "accept_friend_result") {
        // 处理同意申请结果
        bool ok = obj.value("success").toBool();
        QString reason = obj.value("reason").toString();
        if (ok) {
            QMessageBox::information(this, "添加成功", "已成为好友");
            accept();  // 请求者对话框关闭
        } else {
            QMessageBox::warning(this, "添加失败", reason);
        }
    }
}
