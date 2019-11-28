#pragma once

#include <QMainWindow>
#include <QScrollBar>
#include <QListWidget>

#include "InputImage.h"
#include "MyQGraphicsScene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget* parent = nullptr);

	MainWindow(const MainWindow& other) = delete;

	MainWindow(MainWindow&& other) = delete;

	MainWindow& operator=(const MainWindow& other) = delete;

	MainWindow& operator=(MainWindow&& other) = delete;

    ~MainWindow() override;

public slots:

	void onGridProgress(float progress);

	void onMosaicProgress(float progress);

	void onTilesLoadingProgress(float progress);

	void onGridGenerated(bool success);

	void onMosaicGenerated(bool success);

	void onTilesLoaded();

private slots:

	void onImageSceneMouseMoved(QGraphicsSceneMouseEvent* event);

	void onImageSceneMousePressed(QGraphicsSceneMouseEvent* event);

	void on_imageView_mouseEntered(QEvent* event);

    void on_imageView_mouseLeft(QEvent* event);

	void on_imageView_resized(QResizeEvent* event);

	void on_gridView_resized(QResizeEvent* event);

	void on_mosaicView_resized(QResizeEvent* event);

    void on_brushPushButton_clicked();

    void on_eraserPushButton_clicked();

    void on_undoPushButton_clicked();

    void on_redoPushButton_clicked();

    void on_radiusSlider_valueChanged(int value);

    void on_brushColorPushButton_clicked();

	void on_eraserColorPushButton_clicked();

    void on_gridSpacesColorPushButton_clicked();

    void on_gridCellWidthSpinBox_valueChanged(int arg1);

    void on_gridCellHeightSpinBox_valueChanged(int arg1);

    void on_gridXSpacesSpinBox_valueChanged(int arg1);

    void on_gridYSpacesSpinBox_valueChanged(int arg1);

    void on_maxMergedSizeSpinBox_valueChanged(int arg1);

    void on_maxMergedDiffSpinBox_valueChanged(int arg1);

    void on_numSwappedSpinBox_valueChanged(int arg1);

    void on_tilesListWidget_itemSelectionChanged();

    void on_actionLoadTiles_triggered();

    void on_actionDeleteSelectedTiles_triggered();

    void on_actionDeleteTiles_triggered();

    void on_actionLoadImage_triggered();

    void on_actionResizeImage_triggered();

    void on_actionGenerateGrid_triggered();

    void on_actionSaveGrid_triggered();

    void on_actionResizeGrid_triggered();

    void on_actionGenerateMosaic_triggered();

    void on_actionSaveMosaic_triggered();

    void on_actionResizeMosaic_triggered();

    void on_mosaicSpacesColorPushButton_clicked();

    void on_mosaicCellWidthSpinBox_valueChanged(int arg1);

    void on_mosaicCellHeightSpinBox_valueChanged(int arg1);

    void on_mosaicXSpacesSpinBox_valueChanged(int arg1);

    void on_mosaicYSpacesSpinBox_valueChanged(int arg1);

    void on_numNearestSpinBox_valueChanged(int arg1);

    void on_blendingSpinBox_valueChanged(int arg1);

	void on_blendGridRadioButton_toggled(bool checked);

	void on_blendImageRadioButton_toggled(bool checked);

signals:

	void generateGrid();

	void generateMosaic();

	void loadTiles(const QStringList& fileList, QListWidget* tileListWidget);

private:

    Ui::MainWindow* ui;

	QPointF previousPoint;

	QSizeF m_relativeImageScroll;
	QSizeF m_relativeGridScroll;
	QSizeF m_relativeMosaicScroll;

	MyQGraphicsScene* m_imageScene = nullptr;
	QGraphicsScene* m_gridScene = nullptr;
	QGraphicsScene* m_mosaicScene = nullptr;

	bool m_imageResized = false;
	bool m_gridResized = false;
	bool m_mosaicResized = false;

	QGraphicsPixmapItem* imagePixmapItem = nullptr;
	QGraphicsPixmapItem* gridPixmapItem = nullptr;
	QGraphicsPixmapItem* mosaicPixmapItem = nullptr;

	QGraphicsEllipseItem* ellipseItem = nullptr;

	QColor m_eraserColor = QColor(255, 255, 255, 255 * InputImage::STENCIL_ALPHA);

	qreal getRelativeScrollValue(const QScrollBar* scrollBar) const;

	void setRelativeScrollValue(QScrollBar* scrollBar, qreal value);

	void updateInputImage();

	void updateGridImage();

	void updateMosaicImage();

	void updateEnabledElements();

	void updateValues();
};


