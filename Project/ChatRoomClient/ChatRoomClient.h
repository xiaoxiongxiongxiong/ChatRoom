#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ChatRoomClient.h"

class ChatRoomClient : public QMainWindow
{
    Q_OBJECT

public:
    ChatRoomClient(QWidget *parent = nullptr);
    ~ChatRoomClient();

private:
    Ui::ChatRoomClientClass ui;
};
