#ifndef GRAPHPLOTTERDOCKWIDGET_H
#define GRAPHPLOTTERDOCKWIDGET_H

#include <QDockWidget>
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <itkFixedArray.h>
namespace Ui {
class GraphPlotterDockWidget;
}

class GraphPlotterDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphPlotterDockWidget(QWidget *parent = 0);
    ~GraphPlotterDockWidget();

    virtual void Draw();
    //void SetTemporalReference(const vtkSmartPointer<vtkDoubleArray> & tScale);
    void SetLength(unsigned int samples);
    void AddPlot(const vtkSmartPointer<vtkDoubleArray> & series ,itk::FixedArray<double,3>  & color,bool dashed);

private slots:
	void slotClear();
	void slotExportData();
private:
    Ui::GraphPlotterDockWidget *m_pUI;
    vtkSmartPointer<vtkContextView> m_View;
    vtkSmartPointer<vtkChartXY> m_Chart;
    vtkSmartPointer<vtkTable> m_Table;
    std::vector<itk::FixedArray<double,3> > m_Colors;
    std::vector<bool> m_Dashed;
};

#endif // GRAPHPLOTTERDOCKWIDGET_H
