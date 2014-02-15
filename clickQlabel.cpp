#include "clickQlabel.h"

clickQlabel::clickQlabel(QWidget* parent) : QLabel(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

void clickQlabel::slotClicked()
{
}

void clickQlabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}
