#ifndef CELLLISTDOCKWIDGET_H
#define CELLLISTDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class CellListDockWidget;
}

class CellListDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CellListDockWidget(QWidget *parent = 0);
    ~CellListDockWidget();

private:
    Ui::CellListDockWidget *ui;
};

#endif // CELLLISTDOCKWIDGET_H
