#include "ConcurrencyHandler.h"
#include "ImageMosaicMaker.h"
#include "ImageLoader.h"
#include "Utility.h"

ConcurrencyHandler::ConcurrencyHandler(QObject* parent)
	: QObject(parent)
{

}

void ConcurrencyHandler::init(TileSet* tileSet, QListWidget* tileListWidget)
{
	m_tileSet = tileSet;
	m_tileListWidget = tileListWidget;
}

bool ConcurrencyHandler::isGeneratingGrid() const
{
	return m_generateGridFuture.isRunning();
}

bool ConcurrencyHandler::isGeneratingMosaic() const
{
	return m_generateMosaicFuture.isRunning();
}

bool ConcurrencyHandler::isLoadingTiles() const
{
	return m_loadTilesFuture.isRunning();
}

void ConcurrencyHandler::onGenerateGrid()
{
	if (m_connected == false)
	{
		connect();
	}

	m_generateGridFuture = QtConcurrent::run([]()
	{
		return ImageMosaicMaker::m_gridGenerator.generate(ImageMosaicMaker::m_inputImage);
	});

	m_generateGridFutureWatcher.setFuture(m_generateGridFuture);
}

void ConcurrencyHandler::onGenerateMosaic()
{
	if (m_connected == false)
	{
		connect();
	}

	m_generateMosaicFuture = QtConcurrent::run([]()
	{
		return ImageMosaicMaker::m_mosaicGenerator.generate(ImageMosaicMaker::m_inputImage, ImageMosaicMaker::m_grid, ImageMosaicMaker::m_tileSet);
	});

	m_generateMosaicFutureWatcher.setFuture(m_generateMosaicFuture);
}

void ConcurrencyHandler::onLoadTiles(const QStringList& fileList)
{
	if (m_connected == false)
	{
		connect();
	}

	m_loadTilesFuture = QtConcurrent::run([fileList, this]()
	{
		loadTiles(fileList);
	});

	m_loadTilesFutureWatcher.setFuture(m_loadTilesFuture);
}

void ConcurrencyHandler::onGridGenerated()
{
	Grid grid = m_generateGridFuture.result();
	if (!grid.isEmpty())
	{
		ImageMosaicMaker::m_grid = grid;
		emit gridGenerated(true);
	}
	else
	{
		emit gridGenerated(false);
	}
}

void ConcurrencyHandler::onMosaicGenerated()
{
	Mosaic mosaic = m_generateMosaicFuture.result();
	if (!mosaic.isEmpty())
	{
		ImageMosaicMaker::m_mosaic = mosaic;
		emit mosaicGenerated(true);
	}
	else
	{
		emit mosaicGenerated(false);
	}
}

void ConcurrencyHandler::onTilesLoaded()
{
	emit tilesLoaded();
}

void ConcurrencyHandler::connect()
{
	QObject::connect(&m_generateGridFutureWatcher, &QFutureWatcher<Grid>::finished, this, &ConcurrencyHandler::onGridGenerated);
	QObject::connect(&m_generateMosaicFutureWatcher, &QFutureWatcher<Mosaic>::finished, this, &ConcurrencyHandler::onMosaicGenerated);
	QObject::connect(&m_loadTilesFutureWatcher, &QFutureWatcher<Mosaic>::finished, this, &ConcurrencyHandler::onTilesLoaded);

	m_connected = true;
}

void ConcurrencyHandler::loadTiles(const QStringList& fileList) 
{
	for (int i = 0; i < fileList.size(); ++i)
	{
		const QString& filename = fileList[i];

		Tile newTile = ImageLoader::loadTile(filename.toStdString(), m_tileListWidget->iconSize().width(), m_tileListWidget->iconSize().height());

		if (!newTile.getImage().empty())
		{
			if (ImageMosaicMaker::m_tileSet.addTile(newTile) == true)
			{
				QIcon icon = QIcon(Utility::cvMatToQPixmap(newTile.getIcon()));
				QString name = filename.mid(filename.lastIndexOf("/") + 1);
				QListWidgetItem* it = new QListWidgetItem(icon, name);
				m_tileListWidget->addItem(it);
			}
		}

		emit tilesLoadingProgress(static_cast<float>(i) / fileList.size());
	}

	emit tilesLoadingProgress(1.0f);
}
