#pragma once

#include <QApplication>

#include <memory>

#include "MainWindow.h"
#include "InputImage.h"
#include "Grid.h"
#include "TileSet.h"
#include "Mosaic.h"
#include "GridGenerator.h"
#include "MosaicGenerator.h"
#include "ConcurrencyHandler.h"

class ImageMosaicMaker 
{

public:

	static bool m_initialized;
	static InputImage m_inputImage;
	static Grid m_grid;
	static TileSet m_tileSet;
	static Mosaic m_mosaic;
	static GridGenerator m_gridGenerator;
	static MosaicGenerator m_mosaicGenerator;
	static ConcurrencyHandler m_concurrencyHandler;
	static std::unique_ptr<QApplication> m_qApplication;
	static std::unique_ptr<MainWindow> m_mainWindow;

	static void init(int& argc, char* argv[]);

	static int run();

	static void cleanUp();

};
