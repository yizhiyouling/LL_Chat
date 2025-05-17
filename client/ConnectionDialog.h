#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QUrl>
#include "WebSocketClient.h"
class QLabel;
class QPushButton;

// ConnectionDialog: 在客户端启动时尝试连接服务器，对连接结果进行提示和重试
class ConnectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConnectionDialog(WebSocketClient *client,
                              const QUrl &serverUrl,
                              QWidget *parent = nullptr);

private slots:
    void tryConnect();                                // 开始或重试连接服务器
    void onConnected();                               // 连接成功时调用
    void onError(QAbstractSocket::SocketError error); // 连接失败时调用，显示错误

private:
    WebSocketClient *m_client;  // WebSocket 客户端实例，用于建立连接
    QUrl             m_serverUrl;   // 要连接的服务器地址
    QLabel          *m_statusLabel; // 显示连接状态的标签
    QPushButton     *m_retryBtn;     // 重试连接按钮
};

#endif
