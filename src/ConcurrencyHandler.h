#pragma once

#include <QObject>
#include <QtConcurrent>
#include <QStringList>
#include <QListWidget>

#include "Grid.h"
#include "TileSet.h"
#include "Mosaic.h"

class ConcurrencyHandler : public QObject
{
	Q_OBJECT

public:

	explicit ConcurrencyHandler(QObject* parent = nullptr);

	ConcurrencyHandler(const ConcurrencyHandler& other) = default;

	ConcurrencyHandler(ConcurrencyHandler&& other) = default;

	ConcurrencyHandler& operator=(const ConcurrencyHandler& other) = default;

	ConcurrencyHandler& operator=(ConcurrencyHandler&& other) = default;

	~ConcurrencyHandler() override = default;

	void init(TileSet* tileSet, QListWidget* tileListWidget);

	bool isGeneratingGrid() const;

	bool isGeneratingMosaic() const;

	bool isLoadingTiles() const;

public slots:

	void onGenerateGrid();

	void onGenerateMosaic();

	void onLoadTiles(const QStringList& fileList);

private slots:

	void onGridGenerated();

	void onMosaicGenerated();

	void onTilesLoaded();

signals:

	void gridGenerated(bool success);

	void mosaicGenerated(bool success);

	void tilesLoaded();

	void tilesLoadingProgress(float progress);

private:

	void connect();

	void loadTiles(const QStringList& fileList);

	bool m_connected = false;
	TileSet* m_tileSet = nullptr;
	QListWidget* m_tileListWidget = nullptr;

	QFuture<Grid> m_generateGridFuture;
	QFutureWatcher<Grid> m_generateGridFutureWatcher;
	QFuture<Mosaic> m_generateMosaicFuture;
	QFutureWatcher<Mosaic> m_generateMosaicFutureWatcher;
	QFuture<void> m_loadTilesFuture;
	QFutureWatcher<void> m_loadTilesFutureWatcher;
};

