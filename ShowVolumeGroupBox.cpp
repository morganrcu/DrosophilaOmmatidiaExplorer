#include "ShowVolumeGroupBox.h"
#include "ui_ShowVolumeGroupBox.h"

ShowVolumeGroupBox::ShowVolumeGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ShowVolumeGroupBox)
{
    ui->setupUi(this);
}

ShowVolumeGroupBox::~ShowVolumeGroupBox()
{
    delete ui;
}
