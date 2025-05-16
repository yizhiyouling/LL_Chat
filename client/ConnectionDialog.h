#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QUrl>
#include "WebSocketClient.h"

class QLabel;
class QPushButton;

class ConnectionDialog : public QDialog {
    Q_OBJECT
public:
    // 新增：传入 serverUrl
    explicit ConnectionDialog(WebSocketClient *client,
                              const QUrl &serverUrl,
                              QWidget *parent = nullptr);

private slots:
    void tryConnect();
    void onConnected();
    void onError(QAbstractSocket::SocketError error);

private:
    WebSocketClient *m_client;
    QUrl              m_serverUrl;
    QLabel           *m_statusLabel;
    QPushButton      *m_retryBtn;
};

#endif // CONNECTIONDIALOG_H
