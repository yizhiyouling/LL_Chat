#include "ConnectionDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

ConnectionDialog::ConnectionDialog(WebSocketClient *client,
                                   const QUrl &serverUrl,
                                   QWidget *parent)
    : QDialog(parent),
    m_client(client),
    m_serverUrl(serverUrl)   // 保存 URL
{
    setWindowTitle("连接服务器");
    resize(300, 120);

    m_statusLabel = new QLabel("正在连接服务器...", this);
    m_retryBtn    = new QPushButton("重试", this);
    m_retryBtn->setEnabled(false);

    auto *lay = new QVBoxLayout(this);
    lay->addWidget(m_statusLabel);
    lay->addWidget(m_retryBtn, 0, Qt::AlignCenter);

    connect(m_retryBtn, &QPushButton::clicked, this, &ConnectionDialog::tryConnect);
    connect(m_client, &WebSocketClient::connected, this, &ConnectionDialog::onConnected);
    connect(m_client, &WebSocketClient::errorOccurred, this, &ConnectionDialog::onError);

    tryConnect();
}

void ConnectionDialog::tryConnect()
{
    m_statusLabel->setText("正在连接服务器...");
    m_retryBtn->setEnabled(false);
    // 使用传入的 m_serverUrl 进行连接
    qDebug() << "Connecting to" << m_serverUrl.toString();
    m_client->connectToServer(m_serverUrl);
}

void ConnectionDialog::onConnected()
{
    QMessageBox::information(this, "提示", "连接成功");
    accept();
}

void ConnectionDialog::onError(QAbstractSocket::SocketError error)
{
    qWarning() << "WebSocket 连接错误，错误码 =" << error;
    m_statusLabel->setText("连接失败，请点击重试");
    m_retryBtn->setEnabled(true);
}
