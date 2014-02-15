#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <QMouseEvent>

class clickQlabel : public QLabel
{
        Q_OBJECT
public:
        clickQlabel(QWidget* parent = 0);
        ~clickQlabel(){}

signals:
        void clicked();

public slots:
        void slotClicked();

protected:
        void mousePressEvent(QMouseEvent* event);
};

#endif // MYLABEL_H
