#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QCheckBox>
#include "WebSocketClient.h"

class QPushButton;
class QLineEdit;

class RegisterWindow : public QWidget {
    Q_OBJECT
public:
    explicit RegisterWindow(WebSocketClient *client, QWidget *parent = nullptr);
    ~RegisterWindow() override;

private slots:
    void onRegisterClicked();
    void onCancelClicked();
    void onServerMessage(const QJsonObject &obj);

private:
    WebSocketClient *m_client;
    QLineEdit      *m_usernameEdit;
    QLineEdit      *m_passwordEdit;
    QLineEdit      *m_confirmEdit;   // 确认密码
    QCheckBox      *m_showPwdChk;    // 显示密码
    QPushButton    *m_registerBtn;
    QPushButton    *m_cancelBtn;
};

#endif // REGISTERWINDOW_H
