#include <QtGui/QApplication>
#include <QPixmap>
#include <QPicture>
#include <cassert>
#include "exif-scout_window.h"

using namespace std;

// define the program title
const char* version = "Exif-Scout v1.2";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ExifScout window;
    window.show();
    return app.exec();
}
