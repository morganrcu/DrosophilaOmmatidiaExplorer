#ifndef TISSUEDOCKWIDGET_H
#define TISSUEDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class TissueDockWidget;
}

class TissueDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit TissueDockWidget(QWidget *parent = 0);
    ~TissueDockWidget();

private:
    Ui::TissueDockWidget *ui;
};

#endif // TISSUEDOCKWIDGET_H
