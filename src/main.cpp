#include "ImageMosaicMaker.h"

int main(int argc, char** argv)
{
	int exitCode = 0;
	ImageMosaicMaker::init(argc, argv);
	exitCode = ImageMosaicMaker::run();
	ImageMosaicMaker::cleanUp();
	return exitCode;
}
