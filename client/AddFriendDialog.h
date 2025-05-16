#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include "WebSocketClient.h"

class QLineEdit;
class QPushButton;

class AddFriendDialog : public QDialog {
    Q_OBJECT
public:
    AddFriendDialog(const QString &currentUser, WebSocketClient *client, QWidget *parent = nullptr);
    ~AddFriendDialog() override;

private slots:
    void onAddClicked();
    void onCancelClicked();
    void onServerMessage(const QJsonObject &obj);

private:
    QString m_currentUser;
    WebSocketClient *m_client;
    QLineEdit *m_friendEdit;
    QPushButton *m_addBtn;
    QPushButton *m_cancelBtn;
};

#endif // ADDFRIENDDIALOG_H
