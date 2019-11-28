#include "MyQGraphicsView.h"

MyQGraphicsView::MyQGraphicsView(QWidget* parent) 
	: QGraphicsView(parent)
{

}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	emit mouseMoved(event);

	QGraphicsView::mouseMoveEvent(event);
}

void MyQGraphicsView::mousePressEvent(QMouseEvent *event)
{
	emit mousePressed(event);

	QGraphicsView::mousePressEvent(event);
}

void MyQGraphicsView::enterEvent(QEvent * event)
{
	emit mouseEntered(event);

	QGraphicsView::enterEvent(event);
}

void MyQGraphicsView::leaveEvent(QEvent *event)
{
	emit mouseLeft(event);

	QGraphicsView::leaveEvent(event);
}

void MyQGraphicsView::resizeEvent(QResizeEvent* event)
{
	emit resized(event);

	QGraphicsView::resizeEvent(event);
}
