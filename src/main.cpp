#include <QApplication>
#include <QFont>
#include "MainWindow.h"
#include <QIcon>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("LeafMorph");
    app.setWindowIcon(QIcon(":/assets/icon.png"));
    app.setApplicationVersion("1.0");
    app.setOrganizationName("LeafMorph");

    // Use a clean system font
    QFont font("Inter");
    if (!font.exactMatch()) {
        font = QFont("SF Pro Display");
        if (!font.exactMatch()) {
            font = QFont("Segoe UI");
            if (!font.exactMatch()) {
                font = QApplication::font();
            }
        }
    }
    font.setPointSize(10);
    app.setFont(font);

    MainWindow w;
    w.show();
    return app.exec();
}
