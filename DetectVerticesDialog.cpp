#include "DetectVerticesDialog.h"
#include "ui_DetectVerticesDialog.h"

DetectVerticesDialog::DetectVerticesDialog(QWidget *parent) :
    QDialog(parent),
    m_pUI(new Ui::DetectVerticesDialog)
{
    m_pUI->setupUi(this);
}

DetectVerticesDialog::~DetectVerticesDialog()
{
    delete m_pUI;
}

void DetectVerticesDialog::SetNumFrames(int numFrames){
    this->m_pUI->referenceSpinBox->setMaximum(numFrames-1);
    this->m_pUI->referenceSpinBox->setMinimum(0);

}
