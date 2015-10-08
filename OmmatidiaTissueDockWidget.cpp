#include "OmmatidiaTissueDockWidget.h"
#include "ui_OmmatidiaTissueDockWidget.h"
#include "OmmatidiaTissueTreeModel.h"
OmmatidiaTissueDockWidget::OmmatidiaTissueDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pUI(new Ui::OmmatidiaTissueDockWidget)
{
	m_pUI->setupUi(this);
}

OmmatidiaTissueDockWidget::~OmmatidiaTissueDockWidget()
{
    delete m_pUI;
}

void OmmatidiaTissueDockWidget::Init(){

	OmmatidiaTissueTreeModel * model = new OmmatidiaTissueTreeModel(this->m_Tissue,this);
	this->m_pUI->tissueTreeView->setModel(model);
}
