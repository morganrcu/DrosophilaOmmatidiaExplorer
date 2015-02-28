#ifndef SHOWVOLUMEGROUPBOX_H
#define SHOWVOLUMEGROUPBOX_H

#include <QGroupBox>

namespace Ui {
class ShowVolumeGroupBox;
}

class ShowVolumeGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit ShowVolumeGroupBox(QWidget *parent = 0);
    ~ShowVolumeGroupBox();

private:
    Ui::ShowVolumeGroupBox *ui;
};

#endif // SHOWVOLUMEGROUPBOX_H
