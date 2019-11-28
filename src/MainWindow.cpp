#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "ImageLoader.h"
#include "ImageMosaicMaker.h"
#include "Utility.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QGraphicsEllipseItem>

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->tabWidget);

	m_imageScene = new MyQGraphicsScene(this);
	m_gridScene = new QGraphicsScene(this);
	m_mosaicScene = new QGraphicsScene(this);

	ui->imageView->setScene(m_imageScene);
	ui->gridView->setScene(m_gridScene);
	ui->mosaicView->setScene(m_mosaicScene);

	imagePixmapItem = m_imageScene->addPixmap(QPixmap());
	gridPixmapItem = m_gridScene->addPixmap(QPixmap());
	mosaicPixmapItem = m_mosaicScene->addPixmap(QPixmap());

	ellipseItem = m_imageScene->addEllipse(0, 0, 100, 100, QPen(QColor(0, 0, 0, 0)), QBrush(QColor(0, 0, 0, 0)));

	connect(m_imageScene, &MyQGraphicsScene::mouseMoved, this, &MainWindow::onImageSceneMouseMoved);
	connect(m_imageScene, &MyQGraphicsScene::mousePressed, this, &MainWindow::onImageSceneMousePressed);
    connect(ui->loadImagePushButton, &QPushButton::clicked, this, &MainWindow::on_actionLoadImage_triggered);
    connect(ui->resizeImagePushButton, &QPushButton::clicked, this, &MainWindow::on_actionResizeImage_triggered);
    connect(ui->generateGridPushButton, &QPushButton::clicked, this, &MainWindow::on_actionGenerateGrid_triggered);
    connect(ui->saveGridPushButton, &QPushButton::clicked, this, &MainWindow::on_actionSaveGrid_triggered);
    connect(ui->resizeGridPushButton, &QPushButton::clicked, this, &MainWindow::on_actionResizeGrid_triggered);
    connect(ui->loadTilesPushButton, &QPushButton::clicked, this, &MainWindow::on_actionLoadTiles_triggered);
    connect(ui->deleteSelectedTilesPushButton, &QPushButton::clicked, this, &MainWindow::on_actionDeleteSelectedTiles_triggered);
    connect(ui->deleteTilesPushButton, &QPushButton::clicked, this, &MainWindow::on_actionDeleteTiles_triggered);
    connect(ui->generateMosaicPushButton, &QPushButton::clicked, this, &MainWindow::on_actionGenerateMosaic_triggered);
    connect(ui->saveMosaicPushButton, &QPushButton::clicked, this, &MainWindow::on_actionSaveMosaic_triggered);
    connect(ui->resizeMosaicPushButton, &QPushButton::clicked, this, &MainWindow::on_actionResizeMosaic_triggered);

	ImageMosaicMaker::m_concurrencyHandler.init(&ImageMosaicMaker::m_tileSet, ui->tilesListWidget);

    ui->imageView->addAction(ui->actionLoadImage);
    ui->imageView->addAction(ui->actionResizeImage);
    ui->gridView->addAction(ui->actionGenerateGrid);
    ui->gridView->addAction(ui->actionSaveGrid);
    ui->gridView->addAction(ui->actionResizeGrid);
    ui->tilesListWidget->addAction(ui->actionLoadTiles);
    ui->tilesListWidget->addAction(ui->actionDeleteSelectedTiles);
    ui->tilesListWidget->addAction(ui->actionDeleteTiles);
    ui->mosaicView->addAction(ui->actionGenerateMosaic);
    ui->mosaicView->addAction(ui->actionSaveMosaic);
    ui->mosaicView->addAction(ui->actionResizeMosaic);

    ui->tilesListWidget->setMovement(QListWidget::Static);

	updateValues();
	updateEnabledElements();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onImageSceneMouseMoved(QGraphicsSceneMouseEvent* event)
{
	if (!ImageMosaicMaker::m_inputImage.isEmpty())
	{
		QPointF point = event->scenePos();

		if (event->buttons() == Qt::MouseButton::LeftButton)
		{
			if (ui->brushPushButton->isChecked()) 
			{
				ImageMosaicMaker::m_inputImage.addStencil(cv::Point(previousPoint.x(), previousPoint.y()), cv::Point(point.x(), point.y()), ui->radiusSlider->value(), false);
				
				updateInputImage();
			}
			else if (ui->eraserPushButton->isChecked()) 
			{
				ImageMosaicMaker::m_inputImage.removeStencil(cv::Point(previousPoint.x(), previousPoint.y()), cv::Point(point.x(), point.y()), ui->radiusSlider->value(), false);
				
				updateInputImage();
			}

			previousPoint = point;
		}

		if (ui->brushPushButton->isChecked() || ui->eraserPushButton->isChecked())
		{
			ellipseItem->setPos(point - QPointF(ui->radiusSlider->value(), ui->radiusSlider->value()));
		}
	}
}

void MainWindow::onImageSceneMousePressed(QGraphicsSceneMouseEvent* event)
{
	if (!ImageMosaicMaker::m_inputImage.isEmpty() && event->buttons() == Qt::MouseButton::LeftButton)
	{
		QPointF point = event->scenePos();

		if (ui->brushPushButton->isChecked()) 
		{
			ImageMosaicMaker::m_inputImage.addStencil(cv::Point(point.x(), point.y()), ui->radiusSlider->value(), true);
			
			updateInputImage();
			updateEnabledElements();
		}
		else if (ui->eraserPushButton->isChecked()) 
		{
			ImageMosaicMaker::m_inputImage.removeStencil(cv::Point(point.x(), point.y()), ui->radiusSlider->value(), true);
			
			updateInputImage();
			updateEnabledElements();
		}

		previousPoint = point;
	}
}

void MainWindow::on_imageView_mouseEntered(QEvent* event)
{
	if (!ImageMosaicMaker::m_inputImage.isEmpty())
	{
		if (ui->brushPushButton->isChecked())
		{
			QColor color = Utility::vecToQColor(ImageMosaicMaker::m_inputImage.getStencilColor());
			color.setAlpha(255 * InputImage::STENCIL_ALPHA);

			ellipseItem->setBrush(color);
			ellipseItem->setPen(color);
		}
		else if (ui->eraserPushButton->isChecked())
		{
			ellipseItem->setBrush(m_eraserColor);
			ellipseItem->setPen(m_eraserColor);
		}
	}
}

void MainWindow::on_imageView_mouseLeft(QEvent* event)
{
	if (!ImageMosaicMaker::m_inputImage.isEmpty())
	{
		if (ui->brushPushButton->isChecked() || ui->eraserPushButton->isChecked())
		{
			ellipseItem->setBrush(QColor(0, 0, 0, 0));
			ellipseItem->setPen(QColor(0, 0, 0, 0));
		}
	}
}

void MainWindow::on_imageView_resized(QResizeEvent* event)
{
	updateInputImage();
}

void MainWindow::on_gridView_resized(QResizeEvent* event)
{
	updateGridImage();
}

void MainWindow::on_mosaicView_resized(QResizeEvent* event)
{
	updateMosaicImage();
}

qreal MainWindow::getRelativeScrollValue(const QScrollBar* scrollBar) const
{
	int value = scrollBar->value();
	int min = scrollBar->minimum();
	int max = scrollBar->maximum();

	return static_cast<qreal>(value - min) / static_cast<qreal>(max - min);
}

void MainWindow::setRelativeScrollValue(QScrollBar* scrollBar, qreal value)
{
	int min = scrollBar->minimum();
	int max = scrollBar->maximum();

	scrollBar->setValue(min + value * (max - min));
}

void MainWindow::updateInputImage()
{
	QPixmap pixmap = Utility::cvMatToQPixmap(ImageMosaicMaker::m_inputImage.getStencilImage());
	imagePixmapItem->setPixmap(pixmap);
	ui->imageView->setSceneRect(QRectF(QPointF(0, 0), QPointF(pixmap.width(), pixmap.height())));
	m_imageScene->setSceneRect(QRectF(QPointF(0, 0), QPointF(pixmap.width(), pixmap.height())));
	if (ui->resizeImagePushButton->isChecked())
	{
		ui->imageView->fitInView(m_imageScene->sceneRect(), Qt::KeepAspectRatio);
	}
	else
	{
		ui->imageView->resetTransform();
	}

	if (ImageMosaicMaker::m_inputImage.getImage().size().width != 0)
	{
		ui->gridCellWidthSpinBox->setMaximum(ImageMosaicMaker::m_inputImage.getImage().size().width);
	}

	if (ImageMosaicMaker::m_inputImage.getImage().size().height != 0)
	{
		ui->gridCellHeightSpinBox->setMaximum(ImageMosaicMaker::m_inputImage.getImage().size().height);
	}
}

void MainWindow::updateGridImage()
{
	QPixmap pixmap = Utility::cvMatToQPixmap(ImageMosaicMaker::m_grid.getGridImage());
	gridPixmapItem->setPixmap(pixmap);
	ui->gridView->setSceneRect(QRectF(QPointF(0, 0), QPointF(pixmap.width(), pixmap.height())));
	m_gridScene->setSceneRect(QRectF(QPointF(0, 0), QPointF(pixmap.width(), pixmap.height())));
	if (ui->resizeGridPushButton->isChecked())
	{
		ui->gridView->fitInView(m_gridScene->sceneRect(), Qt::KeepAspectRatio);
	}
	else
	{
		ui->gridView->resetTransform();
	}
}

void MainWindow::updateMosaicImage()
{
	QPixmap pixmap = Utility::cvMatToQPixmap(ImageMosaicMaker::m_mosaic.getMosaicImage());
	mosaicPixmapItem->setPixmap(pixmap);
	ui->mosaicView->setSceneRect(QRectF(QPointF(0, 0), QPointF(pixmap.width(), pixmap.height())));
	m_mosaicScene->setSceneRect(QRectF(QPointF(0, 0), QPointF(pixmap.width(), pixmap.height())));
	if (ui->resizeMosaicPushButton->isChecked())
	{
		ui->mosaicView->fitInView(m_mosaicScene->sceneRect(), Qt::KeepAspectRatio);
	}
	else
	{
		ui->mosaicView->resetTransform();
	}
}

void MainWindow::updateEnabledElements()
{
	ui->resizeImagePushButton->setEnabled(!ImageMosaicMaker::m_inputImage.isEmpty());
	ui->actionResizeImage->setEnabled(!ImageMosaicMaker::m_inputImage.isEmpty());
	ui->undoPushButton->setEnabled(!ImageMosaicMaker::m_inputImage.isFirstState());
	ui->redoPushButton->setEnabled(!ImageMosaicMaker::m_inputImage.isLastState());

	ui->generateGridPushButton->setEnabled(!ImageMosaicMaker::m_inputImage.isEmpty() && !ImageMosaicMaker::m_concurrencyHandler.isGeneratingGrid());
	ui->actionGenerateGrid->setEnabled(!ImageMosaicMaker::m_inputImage.isEmpty() && !ImageMosaicMaker::m_concurrencyHandler.isGeneratingGrid());
	ui->saveGridPushButton->setEnabled(!ImageMosaicMaker::m_grid.isEmpty());
	ui->actionSaveGrid->setEnabled(!ImageMosaicMaker::m_grid.isEmpty());
	ui->resizeGridPushButton->setEnabled(!ImageMosaicMaker::m_grid.isEmpty());
	ui->actionResizeGrid->setEnabled(!ImageMosaicMaker::m_grid.isEmpty());
	
	ui->generateMosaicPushButton->setEnabled(!ImageMosaicMaker::m_grid.isEmpty() && !ImageMosaicMaker::m_tileSet.isEmpty() && !ImageMosaicMaker::m_concurrencyHandler.isGeneratingMosaic() && !ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->actionGenerateMosaic->setEnabled(!ImageMosaicMaker::m_grid.isEmpty() && !ImageMosaicMaker::m_tileSet.isEmpty() && !ImageMosaicMaker::m_concurrencyHandler.isGeneratingMosaic() && !ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->saveMosaicPushButton->setEnabled(!ImageMosaicMaker::m_mosaic.isEmpty());
	ui->actionSaveMosaic->setEnabled(!ImageMosaicMaker::m_mosaic.isEmpty());
	ui->resizeMosaicPushButton->setEnabled(!ImageMosaicMaker::m_mosaic.isEmpty());
	ui->actionResizeMosaic->setEnabled(!ImageMosaicMaker::m_mosaic.isEmpty());
	
	ui->loadTilesPushButton->setEnabled(!ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->actionLoadTiles->setEnabled(!ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->deleteTilesPushButton->setEnabled(ui->tilesListWidget->count() > 0 && !ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->actionDeleteTiles->setEnabled(ui->tilesListWidget->count() > 0 && !ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->deleteSelectedTilesPushButton->setEnabled(ui->tilesListWidget->selectedItems().count() > 0 && !ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
	ui->actionDeleteSelectedTiles->setEnabled(ui->tilesListWidget->selectedItems().count() > 0 && !ImageMosaicMaker::m_concurrencyHandler.isLoadingTiles());
}

void MainWindow::updateValues()
{
	cv::Size imageSize = ImageMosaicMaker::m_inputImage.getImage().size();
	QColor stencilColor = Utility::vecToQColor(ImageMosaicMaker::m_inputImage.getStencilColor());
	ui->imageSizeLabel->setText(QString::number(imageSize.width) + " x " + QString::number(imageSize.height));
	ui->radiusLabel->setText(QString::number(ui->radiusSlider->value()) + " px");
	ui->brushColorPushButton->setStyleSheet("border: 1px solid black; background-color: " + stencilColor.name());
	ui->eraserColorPushButton->setStyleSheet("border: 1px solid black; background-color: " + m_eraserColor.name());

	QColor gridSpaceColor = Utility::vecToQColor(ImageMosaicMaker::m_gridGenerator.getBorderColor());
	const cv::Size& gridCellSize = ImageMosaicMaker::m_gridGenerator.getCellSize();
	const cv::Size& gridSpaceSizes = ImageMosaicMaker::m_gridGenerator.getBorderSizes();
	cv::Size gridSize = ImageMosaicMaker::m_gridGenerator.getGridImageSize(ImageMosaicMaker::m_inputImage);
	cv::Size numGridCells = ImageMosaicMaker::m_gridGenerator.getCellCount(ImageMosaicMaker::m_inputImage);
	ui->gridSpacesColorPushButton->setStyleSheet("border: 1px solid black; background-color: " + gridSpaceColor.name());
	ui->gridCellWidthSpinBox->setValue(gridCellSize.width);
	ui->gridCellHeightSpinBox->setValue(gridCellSize.height);
	ui->maxMergedDiffSpinBox->setValue(ImageMosaicMaker::m_gridGenerator.getMaxMergingDifference());
	ui->maxMergedSizeSpinBox->setValue(ImageMosaicMaker::m_gridGenerator.getMaxMergedCells());
	ui->gridXSpacesSpinBox->setValue(gridSpaceSizes.width);
	ui->gridYSpacesSpinBox->setValue(gridSpaceSizes.height);
	ui->imageSizeLabel_2->setText(QString::number(imageSize.width) + " x " + QString::number(imageSize.height));
	ui->gridSizeLabel->setText(QString::number(gridSize.width) + " x " + QString::number(gridSize.height));
	ui->numGridCellsLabel->setText(QString::number(numGridCells.width) + " x " + QString::number(numGridCells.height));
	
	cv::Size mosaicSize = ImageMosaicMaker::m_mosaicGenerator.getMosaicImageSize(ImageMosaicMaker::m_inputImage, ImageMosaicMaker::m_grid);
	QColor mosaicSpaceColor = Utility::vecToQColor(ImageMosaicMaker::m_mosaicGenerator.getBorderColor());
	const cv::Size& tileSize = ImageMosaicMaker::m_mosaicGenerator.getTileSize();
	const cv::Size& mosaicSpaceSizes = ImageMosaicMaker::m_mosaicGenerator.getBorderSizes();
	const cv::Size& gridCellSize2 = ImageMosaicMaker::m_grid.getCellSize();
	const cv::Size& numGridCells2 = ImageMosaicMaker::m_grid.getCellCount();
	ui->mosaicSizeLabel->setText(QString::number(mosaicSize.width) + " x " + QString::number(mosaicSize.height));
	ui->gridCellSizeLabel->setText(QString::number(gridCellSize2.width) + " x " + QString::number(gridCellSize2.height));
	ui->numGridCellsLabel_2->setText(QString::number(numGridCells2.width) + " x " + QString::number(numGridCells2.height));
	ui->numSwappedSpinBox->setValue(ImageMosaicMaker::m_gridGenerator.getCellSwapRatio() * ui->numSwappedSpinBox->maximum());
	ui->blendingSpinBox->setValue(ImageMosaicMaker::m_mosaicGenerator.getBlendingFactor() * ui->blendingSpinBox->maximum());
	ui->blendGridRadioButton->setChecked(ImageMosaicMaker::m_mosaicGenerator.getBlendingMode() == MosaicGenerator::BlendingMode::MEANS);
	ui->blendImageRadioButton->setChecked(ImageMosaicMaker::m_mosaicGenerator.getBlendingMode() == MosaicGenerator::BlendingMode::IMAGE);
	ui->mosaicSpacesColorPushButton->setStyleSheet("border: 1px solid black; background-color: " + mosaicSpaceColor.name());
	ui->mosaicCellWidthSpinBox->setValue(tileSize.width);
	ui->mosaicCellHeightSpinBox->setValue(tileSize.height);
	ui->mosaicXSpacesSpinBox->setValue(mosaicSpaceSizes.width);
	ui->mosaicYSpacesSpinBox->setValue(mosaicSpaceSizes.height);
	ui->numNearestSpinBox->setValue(ImageMosaicMaker::m_mosaicGenerator.getNumNearest());

	ui->numTilesLabel->setText(QString::number(ui->tilesListWidget->count()));
}

void MainWindow::on_brushPushButton_clicked()
{
    if (ui->brushPushButton->isChecked())
    {
		ui->eraserPushButton->setChecked(false);
    }
}

void MainWindow::on_eraserPushButton_clicked()
{
    if (ui->eraserPushButton->isChecked())
    {
		ui->brushPushButton->setChecked(false);
    }
}

void MainWindow::on_undoPushButton_clicked()
{
	ImageMosaicMaker::m_inputImage.undo();

	updateInputImage();
	updateEnabledElements();
}

void MainWindow::on_redoPushButton_clicked()
{
	ImageMosaicMaker::m_inputImage.redo();

	updateInputImage();
	updateEnabledElements();
}

void MainWindow::on_radiusSlider_valueChanged(int value)
{
	QPointF pos = ellipseItem->scenePos();
	ellipseItem->setRect(0, 0, value * 2, value * 2);
	ellipseItem->setPos(pos);

	updateValues();
}

void MainWindow::on_brushColorPushButton_clicked()
{
	QColor oldColor = Utility::vecToQColor(ImageMosaicMaker::m_inputImage.getStencilColor());

    QColor color = QColorDialog::getColor(oldColor, this, QString());
    if (color.isValid())
    {
        ImageMosaicMaker::m_inputImage.setStencilColor(Utility::QColorToVec(color));

		updateInputImage();
		updateValues();
    }
}

void MainWindow::on_eraserColorPushButton_clicked()
{
	QColor color = QColorDialog::getColor(m_eraserColor, this, QString());
	if (color.isValid())
	{
		m_eraserColor = color;

		updateValues();
	}
}

void MainWindow::on_gridSpacesColorPushButton_clicked()
{
	QColor oldColor = Utility::vecToQColor(ImageMosaicMaker::m_gridGenerator.getBorderColor());

    QColor color = QColorDialog::getColor(oldColor, this, QString());
    if (color.isValid())
    {
        ImageMosaicMaker::m_gridGenerator.setBorderColor(Utility::QColorToVec(color));
		
		updateInputImage();
		updateValues();
    }
}

void MainWindow::on_gridCellWidthSpinBox_valueChanged(int arg1)
{
	cv::Size cellSize = ImageMosaicMaker::m_gridGenerator.getCellSize();
	cellSize.width = arg1;
    ImageMosaicMaker::m_gridGenerator.setCellSize(cellSize);

	updateValues();
}

void MainWindow::on_gridCellHeightSpinBox_valueChanged(int arg1)
{
	cv::Size cellSize = ImageMosaicMaker::m_gridGenerator.getCellSize();
	cellSize.height = arg1;
	ImageMosaicMaker::m_gridGenerator.setCellSize(cellSize);
    
	updateValues();
}

void MainWindow::on_gridXSpacesSpinBox_valueChanged(int arg1)
{
	cv::Size size = ImageMosaicMaker::m_gridGenerator.getBorderSizes();
	size.width = arg1;
	ImageMosaicMaker::m_gridGenerator.setBorderSizes(size);
	
	updateValues();
}

void MainWindow::on_gridYSpacesSpinBox_valueChanged(int arg1)
{
	cv::Size size = ImageMosaicMaker::m_gridGenerator.getBorderSizes();
	size.height = arg1;
	ImageMosaicMaker::m_gridGenerator.setBorderSizes(size);
	
	updateValues();
}

void MainWindow::on_maxMergedSizeSpinBox_valueChanged(int arg1) 
{
    ImageMosaicMaker::m_gridGenerator.setMaxMergedCells(arg1);
}

void MainWindow::on_maxMergedDiffSpinBox_valueChanged(int arg1)
{
    ImageMosaicMaker::m_gridGenerator.setMaxMergingDifference(arg1);
}

void MainWindow::on_numSwappedSpinBox_valueChanged(int arg1) 
{
    ImageMosaicMaker::m_gridGenerator.setCellSwapRatio(static_cast<float>(arg1) / ui->numSwappedSpinBox->maximum());
}

void MainWindow::onGridProgress(float progress)
{
    ui->gridProgressBar->setValue(static_cast<int>(progress * ui->gridProgressBar->maximum()));
}

void MainWindow::onMosaicProgress(float progress)
{
    ui->mosaicProgressBar->setValue(static_cast<int>(progress * ui->mosaicProgressBar->maximum()));
}

void MainWindow::onTilesLoadingProgress(float progress)
{
	ui->tilesProgressBar->setValue(static_cast<int>(progress * ui->tilesProgressBar->maximum()));
}

void MainWindow::onGridGenerated(bool success)
{
	if (success = false)
	{
		ui->gridProgressBar->setValue(0);
		QMessageBox::critical(this, "Error!", "Unable to create grid of size " +
			QString::number(ImageMosaicMaker::m_gridGenerator.getGridImageSize(ImageMosaicMaker::m_inputImage).width) + " x " +
			QString::number(ImageMosaicMaker::m_gridGenerator.getGridImageSize(ImageMosaicMaker::m_inputImage).height));
	}
	else
	{
		updateGridImage();
		updateValues();
	}

	updateEnabledElements();
}

void MainWindow::onMosaicGenerated(bool success)
{
	if (success == false)
	{
		ui->mosaicProgressBar->setValue(0);
		QMessageBox::critical(this, "Error!", "Unable to create mosaic of size " +
			QString::number(ImageMosaicMaker::m_mosaicGenerator.getMosaicImageSize(ImageMosaicMaker::m_inputImage, ImageMosaicMaker::m_grid).width) + " x " +
			QString::number(ImageMosaicMaker::m_mosaicGenerator.getMosaicImageSize(ImageMosaicMaker::m_inputImage, ImageMosaicMaker::m_grid).height));
	}
	else
	{
		updateMosaicImage();
	}

	updateEnabledElements();
}

void MainWindow::onTilesLoaded()
{
	updateValues();
	updateEnabledElements();
}

void MainWindow::on_tilesListWidget_itemSelectionChanged()
{
	updateEnabledElements();
}

void MainWindow::on_actionLoadTiles_triggered()
{
    QString selectedFilter = "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff)";

    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                  "Load Tiles",
                                  QDir::currentPath(),
                                  "All files (*.*);;"
                                  "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff);;"
                                  "bitmap image (*.bmp);; "
                                  "JPEG (*.jpg;*.jpeg);;"
                                  "PNG (*.png);;"
                                  "GIF (*.gif);;"
                                  "TIF (*.tif;*.tiff)",
                                  &selectedFilter);
	if (fileList.empty())
	{
		return;
	}

	emit loadTiles(fileList, ui->tilesListWidget);

	updateEnabledElements();
}

void MainWindow::on_actionDeleteSelectedTiles_triggered()
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                               "Image Mosaic Maker",
                                                               tr("Are you sure you want to remove the selected tiles from the list?\n"),
                                                               QMessageBox::No | QMessageBox::Yes);
    if (resBtn == QMessageBox::No)
    {
        return;
    }

    if (ui->tilesListWidget->selectedItems().count() == ui->tilesListWidget->count()) 
    {
       ui->tilesListWidget->clear();
	   ImageMosaicMaker::m_tileSet.clear();
    }
    else
    {
        for (int i = 0; i < ui->tilesListWidget->count(); ++i)
        {
            if (!ui->tilesListWidget->item(i)->isSelected())
            {
                continue; 
            }

			ImageMosaicMaker::m_tileSet.deleteTile(i);
            delete ui->tilesListWidget->takeItem(i);

            i--;
       }
    }

	updateValues();
	updateEnabledElements();
}

void MainWindow::on_actionDeleteTiles_triggered()
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                               "Image Mosaic Maker",
                                                               tr("Are you sure you want to remove all tiles from the list?\n"),
                                                               QMessageBox::No | QMessageBox::Yes);
    if (resBtn == QMessageBox::No)
    {
        return;
    }

    ui->tilesListWidget->clear();
	ImageMosaicMaker::m_tileSet.clear();
    
	updateValues();
	updateEnabledElements();
}

void MainWindow::on_actionLoadImage_triggered()
{
    QString selectedFilter = "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff)";

    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Load Image",
                                                    QDir::currentPath(),
                                                    "All files (*.*);;"
                                                    "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff);;"
                                                    "bitmap image (*.bmp);; "
                                                    "JPEG (*.jpg;*.jpeg);;"
                                                    "PNG (*.png);;"
                                                    "GIF (*.gif);;"
                                                    "TIF (*.tif;*.tiff)",
                                                    &selectedFilter);

	if (!filename.isEmpty())
	{
		cv::Mat image = ImageLoader::loadImage(filename.toStdString());

		if (image.empty())
		{
			QMessageBox::critical(this, "Error!", "Unable to open file\n" + filename);
		}
		else
		{
			ImageMosaicMaker::m_inputImage.setImage(image);

			ImageMosaicMaker::m_grid = Grid();

			updateInputImage();
			updateValues();
			updateEnabledElements();
		}
    }
}

void MainWindow::on_actionResizeImage_triggered()
{
	m_imageResized = !m_imageResized;
	ui->resizeImagePushButton->setChecked(m_imageResized);

	if (ui->resizeImagePushButton->isChecked())
	{
		QScrollBar* horizntalScrollBar = ui->imageView->horizontalScrollBar();
		QScrollBar* verticalScrollBar = ui->imageView->verticalScrollBar();

		m_relativeImageScroll.setWidth(getRelativeScrollValue(horizntalScrollBar));
		m_relativeImageScroll.setHeight(getRelativeScrollValue(verticalScrollBar));

		updateInputImage();
	}
	else
	{
		updateInputImage();

		QScrollBar* horizntalScrollBar = ui->imageView->horizontalScrollBar();
		QScrollBar* verticalScrollBar = ui->imageView->verticalScrollBar();

		setRelativeScrollValue(horizntalScrollBar, m_relativeImageScroll.width());
		setRelativeScrollValue(verticalScrollBar, m_relativeImageScroll.height());
	}
}

void MainWindow::on_actionGenerateGrid_triggered()
{
	emit generateGrid();

	updateEnabledElements();
}

void MainWindow::on_actionSaveGrid_triggered()
{
    QString selectedFilter = "PNG (*.png)";

    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Save Grid",
                                                    QDir::currentPath(),
                                                    "All files (*.*);;"
                                                    "bitmap image (*.bmp);; "
                                                    "JPEG (*.jpg;*.jpeg);;"
                                                    "PNG (*.png);;"
                                                    "GIF (*.gif);;"
                                                    "TIF (*.tif;*.tiff)",
                                                    &selectedFilter);


    if (!filename.isEmpty())
    {
        if (!cv::imwrite(filename.toStdString(), ImageMosaicMaker::m_grid.getGridImage()))
        {
            QMessageBox::critical(this, "Error!", "Unable to save to file\n" + filename);
        }
    }
}

void MainWindow::on_actionResizeGrid_triggered()
{
	m_gridResized = !m_gridResized;
	ui->resizeGridPushButton->setChecked(m_gridResized);

	if (ui->resizeGridPushButton->isChecked())
	{
		QScrollBar* horizntalScrollBar = ui->gridView->horizontalScrollBar();
		QScrollBar* verticalScrollBar = ui->gridView->verticalScrollBar();

		m_relativeGridScroll.setWidth(getRelativeScrollValue(horizntalScrollBar));
		m_relativeGridScroll.setHeight(getRelativeScrollValue(verticalScrollBar));

		updateGridImage();
	}
	else
	{
		updateGridImage();

		QScrollBar* horizntalScrollBar = ui->gridView->horizontalScrollBar();
		QScrollBar* verticalScrollBar = ui->gridView->verticalScrollBar();

		setRelativeScrollValue(horizntalScrollBar, m_relativeGridScroll.width());
		setRelativeScrollValue(verticalScrollBar, m_relativeGridScroll.height());
	}
}

void MainWindow::on_actionGenerateMosaic_triggered()
{
	emit generateMosaic();

	updateEnabledElements();
}

void MainWindow::on_actionSaveMosaic_triggered()
{
    QString selectedFilter = "PNG (*.png)";

    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Save Mosaic",
                                                    QDir::currentPath(),
                                                    "All files (*.*);;"
                                                    "bitmap image (*.bmp);; "
                                                    "JPEG (*.jpg;*.jpeg);;"
                                                    "PNG (*.png);;"
                                                    "GIF (*.gif);;"
                                                    "TIF (*.tif;*.tiff)",
                                                    &selectedFilter);


    if (!filename.isEmpty())
    {
        if (!cv::imwrite(filename.toStdString(), ImageMosaicMaker::m_mosaic.getMosaicImage()))
        {
            QMessageBox::critical(this, "Error!", "Unable to save to file\n" + filename);
        }
    }
}

void MainWindow::on_actionResizeMosaic_triggered()
{
	m_mosaicResized = !m_mosaicResized;
	ui->resizeMosaicPushButton->setChecked(m_mosaicResized);

	if (ui->resizeMosaicPushButton->isChecked())
	{
		QScrollBar* horizntalScrollBar = ui->mosaicView->horizontalScrollBar();
		QScrollBar* verticalScrollBar = ui->mosaicView->verticalScrollBar();

		m_relativeMosaicScroll.setWidth(getRelativeScrollValue(horizntalScrollBar));
		m_relativeMosaicScroll.setHeight(getRelativeScrollValue(verticalScrollBar));

		updateMosaicImage();
	}
	else
	{
		updateMosaicImage();

		QScrollBar* horizntalScrollBar = ui->mosaicView->horizontalScrollBar();
		QScrollBar* verticalScrollBar = ui->mosaicView->verticalScrollBar();

		setRelativeScrollValue(horizntalScrollBar, m_relativeMosaicScroll.width());
		setRelativeScrollValue(verticalScrollBar, m_relativeMosaicScroll.height());
	}
}

void MainWindow::on_mosaicSpacesColorPushButton_clicked()
{
	QColor oldColor = Utility::vecToQColor(ImageMosaicMaker::m_mosaicGenerator.getBorderColor());

    QColor color = QColorDialog::getColor(oldColor, this, QString());
    if (color.isValid())
    {
        ImageMosaicMaker::m_mosaicGenerator.setBorderColor(Utility::QColorToVec(color));
		updateValues();
    }
}

void MainWindow::on_mosaicCellWidthSpinBox_valueChanged(int arg1)
{
    cv::Size size = ImageMosaicMaker::m_mosaicGenerator.getTileSize();
    size.width = arg1;
	ImageMosaicMaker::m_mosaicGenerator.setTileSize(size);

	updateValues();
}

void MainWindow::on_mosaicCellHeightSpinBox_valueChanged(int arg1)
{
    cv::Size size = ImageMosaicMaker::m_mosaicGenerator.getTileSize();
    size.height = arg1;
	ImageMosaicMaker::m_mosaicGenerator.setTileSize(size);
    
	updateValues();
}

void MainWindow::on_mosaicXSpacesSpinBox_valueChanged(int arg1)
{
	cv::Size size = ImageMosaicMaker::m_mosaicGenerator.getBorderSizes();
	size.width = arg1;
	ImageMosaicMaker::m_mosaicGenerator.setBorderSizes(size);
	
	updateValues();
}

void MainWindow::on_mosaicYSpacesSpinBox_valueChanged(int arg1)
{
	cv::Size size = ImageMosaicMaker::m_mosaicGenerator.getBorderSizes();
	size.height = arg1;
	ImageMosaicMaker::m_mosaicGenerator.setBorderSizes(size);
	
	updateValues();
}

void MainWindow::on_numNearestSpinBox_valueChanged(int arg1)
{
    ImageMosaicMaker::m_mosaicGenerator.setNumNearest(arg1);
}

void MainWindow::on_blendingSpinBox_valueChanged(int arg1)
{
	ImageMosaicMaker::m_mosaicGenerator.setBlendingFactor(static_cast<float>(arg1) / ui->blendingSpinBox->maximum());
}

void MainWindow::on_blendGridRadioButton_toggled(bool checked)
{
	if (checked)
	{
		ImageMosaicMaker::m_mosaicGenerator.setBlendingMode(MosaicGenerator::BlendingMode::MEANS);
	}
	else
	{
		ImageMosaicMaker::m_mosaicGenerator.setBlendingMode(MosaicGenerator::BlendingMode::IMAGE);
	}
}

void MainWindow::on_blendImageRadioButton_toggled(bool checked)
{
	if (checked)
	{
		ImageMosaicMaker::m_mosaicGenerator.setBlendingMode(MosaicGenerator::BlendingMode::IMAGE);
	}
	else
	{
		ImageMosaicMaker::m_mosaicGenerator.setBlendingMode(MosaicGenerator::BlendingMode::MEANS);
	}
}
