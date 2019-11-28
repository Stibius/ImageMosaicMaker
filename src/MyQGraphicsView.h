#pragma once

#include <QGraphicsView>

class MyQGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:

	explicit MyQGraphicsView(QWidget* parent = nullptr);

	MyQGraphicsView(const MyQGraphicsView& other) = default;

	MyQGraphicsView(MyQGraphicsView&& other) = default;

	MyQGraphicsView& operator=(const MyQGraphicsView& other) = default;

	MyQGraphicsView& operator=(MyQGraphicsView&& other) = default;

	~MyQGraphicsView() override = default;

protected:

	void mouseMoveEvent(QMouseEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;

	void enterEvent(QEvent* event) override;

	void leaveEvent(QEvent* event) override;

	void resizeEvent(QResizeEvent* event) override;

signals:

	void mouseMoved(QMouseEvent* event);

	void mousePressed(QMouseEvent* event);

	void mouseEntered(QEvent* event);

	void mouseLeft(QEvent* event);

	void resized(QResizeEvent* event);
};
