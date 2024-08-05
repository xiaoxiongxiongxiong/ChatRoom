#pragma once

#include <QWidget>
#include <QMovie>
#include <QMouseEvent>
#include "ui_ChatRoomLogin.h"

class ChatRoomLogin : public QWidget
{
    Q_OBJECT

public:
    ChatRoomLogin(QWidget * parent = nullptr);
    ~ChatRoomLogin();

    void mousePressEvent(QMouseEvent * event);//鼠标点击
    void mouseMoveEvent(QMouseEvent * event);//鼠标移动
    void mouseReleaseEvent(QMouseEvent * event);//鼠标释放

private:
    Ui::ChatRoomLoginClass ui;

    QMovie * m_ptrBack = nullptr;

    QImage * m_imgLogin = nullptr;

    QPoint last;
    bool m_blPressed = false;

private slots:
    void onBtnClickedClose();

    void onBtnClickedLogin();

    void onBtnClickedRegister();
};
