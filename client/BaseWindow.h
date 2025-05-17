#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>

// BaseWindow: 所有窗口的抽象基类，定义统一的 setupUI 接口
class BaseWindow : public QWidget {
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr)
        : QWidget(parent)
    {}

    // 纯虚函数：子类必须实现，用于构建和布局 UI 元素
    virtual void setupUI() = 0;
};

#endif
