#include "ChatRoomLogin.h"

ChatRoomLogin::ChatRoomLogin(QWidget * parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    this->setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);//WindowStaysOnTopHint窗口顶置

    const auto backSize = ui.m_labAnimation->size();
    m_ptrBack = new QMovie("./res/login/back.gif");
    m_ptrBack->setScaledSize(backSize);

    ui.m_labAnimation->setMovie(m_ptrBack);
    m_ptrBack->start();


    const auto & wndSize = this->size();

    QImage * img = new QImage;
    if (img->load("./res/login/profile.png"))
    {
        auto b = ui.m_labProfile->size();
        QImage c = img->scaled(b.width(), b.height(), Qt::KeepAspectRatio);
        ui.m_labProfile->setPixmap(QPixmap::fromImage(c));
    }

    ui.m_chkAutoLogin->setStyleSheet("QCheckBox{ "
                                     " color: rgb(168, 168, 168);}"
                                     "QCheckBox::indicator {"
                                     " border: 1px solid #A8A8A8; "
                                     "}");
    ui.m_chkRemPasswd->setStyleSheet("QCheckBox{ "
                                     " color: rgb(168, 168, 168);}"
                                     " QCheckBox::indicator {"
                                     " border: 1px solid #A8A8A8; "
                                     " color: rgb(168, 168, 168);}"
                                     "}");
    ui.m_chkRemPasswd->setChecked(true);

    ui.m_btnLogin->setStyleSheet("QPushButton {"
                                 "  border-radius: 3px;"
                                 "  background-color: rgb(7, 189, 253);"
                                 "  color: rgb(255, 255, 255);"
                                 "}");
}

ChatRoomLogin::~ChatRoomLogin()
{}
