#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    //Создаем приложение.
    QApplication app{ argc, argv };

    //Создаем объект окна приложения.
    MainWindow mainWindow{};
    mainWindow.show();

    //Запускаем приложения.
    return app.exec();
}