#include "ImageMosaicMaker.h"

bool ImageMosaicMaker::m_initialized = false;
InputImage ImageMosaicMaker::m_inputImage;
Grid ImageMosaicMaker::m_grid;
TileSet ImageMosaicMaker::m_tileSet;
Mosaic ImageMosaicMaker::m_mosaic;
GridGenerator ImageMosaicMaker::m_gridGenerator;
MosaicGenerator ImageMosaicMaker::m_mosaicGenerator;
ConcurrencyHandler ImageMosaicMaker::m_concurrencyHandler;
std::unique_ptr<QApplication> ImageMosaicMaker::m_qApplication;
std::unique_ptr<MainWindow> ImageMosaicMaker::m_mainWindow;

void ImageMosaicMaker::init(int& argc, char* argv[])
{
	if (m_initialized == true)
	{
		return;
	}
	m_initialized = true;

	m_qApplication = std::make_unique<QApplication>(argc, argv);
	m_mainWindow = std::make_unique<MainWindow>();

	QObject::connect(m_mainWindow.get(), &MainWindow::generateGrid, &m_concurrencyHandler, &ConcurrencyHandler::onGenerateGrid);
	QObject::connect(m_mainWindow.get(), &MainWindow::generateMosaic, &m_concurrencyHandler, &ConcurrencyHandler::onGenerateMosaic);
	QObject::connect(m_mainWindow.get(), &MainWindow::loadTiles, &m_concurrencyHandler, &ConcurrencyHandler::onLoadTiles);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::gridGenerated, m_mainWindow.get(), &MainWindow::onGridGenerated);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::mosaicGenerated, m_mainWindow.get(), &MainWindow::onMosaicGenerated);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::tilesLoaded, m_mainWindow.get(), &MainWindow::onTilesLoaded);
	QObject::connect(&m_gridGenerator, &GridGenerator::progress, m_mainWindow.get(), &MainWindow::onGridProgress);
	QObject::connect(&m_mosaicGenerator, &MosaicGenerator::progress, m_mainWindow.get(), &MainWindow::onMosaicProgress);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::tilesLoadingProgress, m_mainWindow.get(), &MainWindow::onTilesLoadingProgress);
}

int ImageMosaicMaker::run()
{
	if (m_initialized == false)
	{
		return -1;
	}

	m_mainWindow->showMaximized();

	return m_qApplication->exec();
}

void ImageMosaicMaker::cleanUp()
{
	if (m_initialized == false)
	{
		return;
	}
	m_initialized = false;

	m_qApplication = nullptr;
	m_mainWindow = nullptr;
}
