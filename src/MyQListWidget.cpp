#include "MyQListWidget.h"

MyQListWidget::MyQListWidget(QWidget* parent) 
	: QListWidget(parent)
{
}

void MyQListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->clearSelection();
    }

    QListWidget::mousePressEvent(event);
}

