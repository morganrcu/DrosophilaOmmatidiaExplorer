#ifndef OMMATIDIATISSUEDOCKWIDGET_H
#define OMMATIDIATISSUEDOCKWIDGET_H

#include <QDockWidget>
#include "OmmatidiaTissue.h"
namespace Ui {
class OmmatidiaTissueDockWidget;
}

class OmmatidiaTissueDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit OmmatidiaTissueDockWidget(QWidget *parent = 0);
    ~OmmatidiaTissueDockWidget();

    void SetTissue(const OmmatidiaTissue<3>::Pointer & tissue){
    	m_Tissue=tissue;
    }
    void Init();
private:
    Ui::OmmatidiaTissueDockWidget *m_pUI;

    OmmatidiaTissue<3>::Pointer m_Tissue;


};

#endif // OMMATIDIATISSUEDOCKWIDGET_H
