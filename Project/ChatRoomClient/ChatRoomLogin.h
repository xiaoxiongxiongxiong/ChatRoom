#pragma once

#include <QWidget>
#include <QMovie>
#include "ui_ChatRoomLogin.h"

class ChatRoomLogin : public QWidget
{
    Q_OBJECT

public:
    ChatRoomLogin(QWidget * parent = nullptr);
    ~ChatRoomLogin();

private:
    Ui::ChatRoomLoginClass ui;

    QMovie * m_ptrBack = nullptr;
};
