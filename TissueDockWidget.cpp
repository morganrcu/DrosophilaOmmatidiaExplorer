#include "TissueDockWidget.h"
#include "ui_TissueDockWidget.h"

TissueDockWidget::TissueDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TissueDockWidget)
{
    ui->setupUi(this);
}

TissueDockWidget::~TissueDockWidget()
{
    delete ui;
}
