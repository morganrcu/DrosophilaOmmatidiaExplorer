#ifndef GRAPHPLOTTERDOCKWIDGET_H
#define GRAPHPLOTTERDOCKWIDGET_H

#include <QDockWidget>
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
namespace Ui {
class GraphPlotterDockWidget;
}

class GraphPlotterDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphPlotterDockWidget(QWidget *parent = 0);
    ~GraphPlotterDockWidget();

    void Draw();
    void SetTemporalReference(const vtkSmartPointer<vtkDoubleArray> & tScale);
    void AddPlot(const vtkSmartPointer<vtkDoubleArray> & series );

private:
    Ui::GraphPlotterDockWidget *m_pUI;
    vtkSmartPointer<vtkContextView> m_View;
    vtkSmartPointer<vtkChartXY> m_Chart;
    vtkSmartPointer<vtkTable> m_Table;
};

#endif // GRAPHPLOTTERDOCKWIDGET_H
