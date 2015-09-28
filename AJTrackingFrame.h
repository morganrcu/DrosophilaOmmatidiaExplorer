#ifndef AJTRACKINGFRAME_H
#define AJTRACKINGFRAME_H

#include <QFrame>

namespace Ui {
class AJTrackingFrame;
}

class AJTrackingFrame : public QFrame
{
    Q_OBJECT

public:
    explicit AJTrackingFrame(QWidget *parent = 0);
    ~AJTrackingFrame();

private:
    Ui::AJTrackingFrame *ui;
};

#endif // AJTRACKINGFRAME_H
