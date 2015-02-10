#include "drosophilaommatidiaexplorer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DrosophilaOmmatidiaExplorer w;
    w.show();

    return a.exec();
}
