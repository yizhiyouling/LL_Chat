#include "ConnectionDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

// 构造函数：初始化 UI 元素、信号槽，并立即尝试连接
ConnectionDialog::ConnectionDialog(WebSocketClient *client,
                                   const QUrl &serverUrl,
                                   QWidget *parent)
    : QDialog(parent),
    m_client(client),
    m_serverUrl(serverUrl)
{
    // 设置窗口标题和大小
    setWindowTitle("连接服务器");
    resize(300, 120);

    // 状态标签与重试按钮
    m_statusLabel = new QLabel("正在连接服务器...", this);
    m_retryBtn    = new QPushButton("重试", this);
    m_retryBtn->setEnabled(false);  // 初始禁用

    // 垂直布局：状态标签在上，按钮居中
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(m_statusLabel);
    lay->addWidget(m_retryBtn, 0, Qt::AlignCenter);

    // 信号槽连接
    connect(m_retryBtn, &QPushButton::clicked, this, &ConnectionDialog::tryConnect);
    connect(m_client, &WebSocketClient::connected, this, &ConnectionDialog::onConnected);
    connect(m_client, &WebSocketClient::errorOccurred, this, &ConnectionDialog::onError);

    // 启动时立即尝试连接
    tryConnect();
}

// 尝试连接服务器：更新 UI 并调用 WebSocketClient
void ConnectionDialog::tryConnect()
{
    // 更新状态并禁用重试按钮
    m_statusLabel->setText("正在连接服务器...");
    m_retryBtn->setEnabled(false);

    // 调试输出目标 URL
    qDebug() << "Connecting to" << m_serverUrl.toString();

    // 调用 WebSocketClient 连接
    m_client->connectToServer(m_serverUrl);
}

// 连接成功时回调：提示并关闭对话框
void ConnectionDialog::onConnected()
{
    QMessageBox::information(this, "提示", "连接成功");
    accept();
}

// 连接失败时回调：打印错误码、更新状态并启用重试按钮
void ConnectionDialog::onError(QAbstractSocket::SocketError error)
{
    qWarning() << "WebSocket 连接错误，错误码 =" << error;
    m_statusLabel->setText("连接失败，请点击重试");
    m_retryBtn->setEnabled(true);
}
