#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class MyQGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:

	explicit MyQGraphicsScene(QObject* parent = nullptr);

	MyQGraphicsScene(const MyQGraphicsScene& other) = default;

	MyQGraphicsScene(MyQGraphicsScene&& other) = default;

	MyQGraphicsScene& operator=(const MyQGraphicsScene& other) = default;

	MyQGraphicsScene& operator=(MyQGraphicsScene&& other) = default;

	~MyQGraphicsScene() override = default;

protected:

	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

signals:

	void mouseMoved(QGraphicsSceneMouseEvent* event);

	void mousePressed(QGraphicsSceneMouseEvent* event);

};
