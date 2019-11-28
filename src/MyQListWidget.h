#pragma once

#include <QListWidget>
#include <QMouseEvent>

class MyQListWidget : public QListWidget
{
    Q_OBJECT

public:

    explicit MyQListWidget(QWidget* parent = nullptr);

	MyQListWidget(const MyQListWidget& other) = default;

	MyQListWidget(MyQListWidget&& other) = default;

	MyQListWidget& operator=(const MyQListWidget& other) = default;

	MyQListWidget& operator=(MyQListWidget&& other) = default;

	~MyQListWidget() override = default;

protected:

    void mousePressEvent(QMouseEvent* event) override;
};

