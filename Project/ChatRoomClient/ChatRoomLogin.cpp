#include "ChatRoomLogin.h"
#include <QMessageBox>

ChatRoomLogin::ChatRoomLogin(QWidget * parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //WindowStaysOnTopHint窗口顶置
    this->setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    const auto backSize = ui.m_labAnimation->size();
    m_ptrBack = new QMovie("./res/login/back.gif");
    m_ptrBack->setScaledSize(backSize);
    ui.m_labAnimation->setMovie(m_ptrBack);
    m_ptrBack->start();

    QIcon ico("./res/login/logo.ico");
    QPixmap logo = ico.pixmap(QSize(73, 47));
    ui.m_labLogo->setPixmap(logo);
    ui.m_labLogo->resize(logo.width(), logo.height());
    ui.m_labLogo->show();

    m_imgLogin = new QImage;
    if (m_imgLogin->load("./res/login/profile.png"))
    {
        auto labSize = ui.m_labProfile->size();
        QImage c = m_imgLogin->scaled(labSize.width(), labSize.height(), Qt::KeepAspectRatio);
        ui.m_labProfile->setPixmap(QPixmap::fromImage(c));
    }

    ui.m_btnSetting->setToolTip(QStringLiteral("设置"));
    ui.m_btnMin->setToolTip(QStringLiteral("最小化"));
    ui.m_btnClose->setToolTip(QStringLiteral("关闭"));

    ui.m_chkRemPasswd->setChecked(true);
    connect(ui.m_btnLogin, SIGNAL(clicked()), this, SLOT(onBtnClickedLogin()));
    connect(ui.m_btnRegister, SIGNAL(clicked()), this, SLOT(onBtnClickedRegister()));
    connect(ui.m_btnClose, SIGNAL(clicked()), this, SLOT(onBtnClickedClose()));

}

ChatRoomLogin::~ChatRoomLogin()
{}

void ChatRoomLogin::mousePressEvent(QMouseEvent * event)
{
    m_blPressed = true; // 当前鼠标按下的即是QWidget而非界面上布局的其它控件
    last = event->globalPos();
}

void ChatRoomLogin::mouseMoveEvent(QMouseEvent * event)
{
    if (m_blPressed)
    {
        int dx = event->globalX() - last.x();
        int dy = event->globalY() - last.y();
        last = event->globalPos();
        move(x() + dx, y() + dy);
    }
}

void ChatRoomLogin::mouseReleaseEvent(QMouseEvent * event)
{
    int dx = event->globalX() - last.x();
    int dy = event->globalY() - last.y();
    move(x() + dx, y() + dy);
    m_blPressed = false; // 鼠标松开时，置为false
}

void ChatRoomLogin::onBtnClickedClose()
{
    QApplication * app;
    app->quit();
}

void ChatRoomLogin::onBtnClickedLogin()
{
    QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("登录！"), QMessageBox::NoButton);
}

void ChatRoomLogin::onBtnClickedRegister()
{
    QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("注册账号！"), QMessageBox::NoButton);
}
