#include <QApplication>
#include "engine/ui/ui.hpp"

// I will use pl/sql as the basis of sql in this project

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Ui window;
    window.show();
    return app.exec();
}