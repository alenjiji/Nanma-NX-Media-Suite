#include <QApplication>
#include "app/QtApp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QtApp qtApp;
    qtApp.show();
    
    return app.exec();
}