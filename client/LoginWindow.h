#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "WebSocketClient.h"

class QPushButton;
class QLineEdit;

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(WebSocketClient *client, QWidget *parent = nullptr);
    ~LoginWindow() override;

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onServerMessage(const QJsonObject &obj);

private:
    WebSocketClient *m_client;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_registerBtn;
};

#endif // LOGINWINDOW_H
