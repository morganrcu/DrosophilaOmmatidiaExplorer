#ifndef CELLLISTDOCKWIDGET_H
#define CELLLISTDOCKWIDGET_H

#include <QDockWidget>
#include <QSignalMapper>
#include "CellGraph.h"
#include "Cell.h"
#include "DrosophilaOmmatidiaJSONProject.h"
namespace Ui {
class CellListDockWidget;
}

class CellListDockWidget : public QDockWidget
{
    Q_OBJECT

    typedef DrosophilaOmmatidiaJSONProject::CellGraphType CellsContainer;

    typename CellsContainer::Pointer m_Cells;

public:
    virtual void Draw();
    inline void SetCells(typename CellsContainer::Pointer & cells){
        m_Cells=cells;
    }

    typename CellsContainer::Pointer GetCells(){
    	return m_Cells;
    }

    virtual void SetSelectedCell(const CellsContainer::CellVertexHandler &);
    virtual void ClearSelection();

    explicit CellListDockWidget(QWidget *parent = 0);
    ~CellListDockWidget();

    public slots:
        void slotCellTableSelectionChanged(int row, int column,int,int);
        void slotCellModified(const QString & position);
    signals:
        void SelectedCellChanged(CellsContainer::CellVertexHandler);
        void ModifiedCells();

private:
    Ui::CellListDockWidget *m_pUI;

    QSignalMapper * m_SignalMapper;
};

#endif // CELLLISTDOCKWIDGET_H
