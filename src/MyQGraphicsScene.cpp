#include "MyQGraphicsScene.h"

MyQGraphicsScene::MyQGraphicsScene(QObject* parent)
	: QGraphicsScene(parent)
{

}

void MyQGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	emit mouseMoved(event);

	QGraphicsScene::mouseMoveEvent(event);
}

void MyQGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	emit mousePressed(event);

	QGraphicsScene::mousePressEvent(event);
}