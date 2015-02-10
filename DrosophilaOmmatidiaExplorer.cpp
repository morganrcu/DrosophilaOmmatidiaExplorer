#include "drosophilaommatidiaexplorer.h"
#include "ui_drosophilaommatidiaexplorer.h"

DrosophilaOmmatidiaExplorer::DrosophilaOmmatidiaExplorer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DrosophilaOmmatidiaExplorer)
{
    ui->setupUi(this);
}

DrosophilaOmmatidiaExplorer::~DrosophilaOmmatidiaExplorer()
{
    delete ui;
}
