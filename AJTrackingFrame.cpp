#include "AJTrackingFrame.h"
#include "ui_AJTrackingFrame.h"

AJTrackingFrame::AJTrackingFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AJTrackingFrame)
{
    ui->setupUi(this);
}

AJTrackingFrame::~AJTrackingFrame()
{
    delete ui;
}
