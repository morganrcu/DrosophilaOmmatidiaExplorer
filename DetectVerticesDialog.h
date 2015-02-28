#ifndef DETECTVERTICESDIALOG_H
#define DETECTVERTICESDIALOG_H

#include <QDialog>
#include <ui_DetectVerticesDialog.h>
namespace Ui {
class DetectVerticesDialog;
}

class DetectVerticesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetectVerticesDialog(QWidget *parent = 0);
    ~DetectVerticesDialog();

    void SetNumFrames(int numFrames);

    int GetReferenceFrame(){
        return this->m_pUI->referenceSpinBox->value();
    }
    double GetThreshold(){
        return this->m_pUI->thresholdDoubleSpinBox->value();
    }


private:
    Ui::DetectVerticesDialog *m_pUI;
    int m_NumFrames;
};

#endif // DETECTVERTICESDIALOG_H
