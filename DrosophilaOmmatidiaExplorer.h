#ifndef DROSOPHILAOMMATIDIAEXPLORER_H
#define DROSOPHILAOMMATIDIAEXPLORER_H

#include <QMainWindow>

namespace Ui {
class DrosophilaOmmatidiaExplorer;
}

class DrosophilaOmmatidiaExplorer : public QMainWindow
{
    Q_OBJECT

public:
    explicit DrosophilaOmmatidiaExplorer(QWidget *parent = 0);
    ~DrosophilaOmmatidiaExplorer();

private:
    Ui::DrosophilaOmmatidiaExplorer *ui;
};

#endif // DROSOPHILAOMMATIDIAEXPLORER_H
