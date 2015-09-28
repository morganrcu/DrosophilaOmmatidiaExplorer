#include "CellListDockWidget.h"
#include "ui_CellListDockWidget.h"

CellListDockWidget::CellListDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CellListDockWidget)
{
    ui->setupUi(this);
}

CellListDockWidget::~CellListDockWidget()
{
    delete ui;
}
