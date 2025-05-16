#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>

// 所有窗口的抽象基类
class BaseWindow : public QWidget {
    Q_OBJECT
public:
    explicit BaseWindow(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual void setupUI() = 0;
};

#endif // BASEWINDOW_H
