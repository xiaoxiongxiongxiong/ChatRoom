#include "ChatRoomClient.h"
#include "ChatRoomLogin.h"
#include <QtWidgets/QApplication>

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    ChatRoomLogin w;
    w.show();

    //ChatRoomClient w;
    //w.show();
    return a.exec();
}
