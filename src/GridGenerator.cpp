#include "GridGenerator.h"
#include "Utility.h"

#include <chrono>

std::default_random_engine GridGenerator::m_randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

const cv::Size& GridGenerator::getCellSize() const 
{ 
	return m_cellSize; 
}

int GridGenerator::getMaxMergedCells() const 
{ 
	return m_maxMergedCells; 
}

int GridGenerator::getMaxMergingDifference() const 
{ 
	return m_maxMergingDifference; 
}

float GridGenerator::getCellSwapRatio() const 
{ 
	return m_cellSwapRatio; 
}

const cv::Size& GridGenerator::getBorderSizes() const 
{ 
	return m_borderSizes; 
}

const cv::Vec3b& GridGenerator::getBorderColor() const 
{ 
	return m_borderColor; 
}

cv::Size GridGenerator::getCellCount(const InputImage& inputImage) const
{
	cv::Size cellCount;

	//maximum size of merged cell
	cv::Size maxCellSize = m_cellSize * m_maxMergedCells;

	const cv::Mat& image = inputImage.getImage();

	//number of base cells which can fit in each dimension

	if (image.cols % maxCellSize.width < maxCellSize.width / 2)
		cellCount.width = (image.cols / maxCellSize.width) * m_maxMergedCells;
	else
		cellCount.width = ((image.cols / maxCellSize.width) + 1) * m_maxMergedCells;

	if (image.rows % maxCellSize.height < maxCellSize.height / 2)
		cellCount.height = (image.rows / maxCellSize.height) * m_maxMergedCells;
	else
		cellCount.height = ((image.rows / maxCellSize.height) + 1) * m_maxMergedCells;

	return cellCount;
}

cv::Size GridGenerator::getGridImageSize(const InputImage& inputImage) const
{
	cv::Size cellCount = getCellCount(inputImage);

	cv::Size imageSize;

	imageSize.height = (m_cellSize.height * cellCount.height) + ((cellCount.height + 1) * m_borderSizes.height);
	imageSize.width = (m_cellSize.width * cellCount.width) + ((cellCount.width + 1) * m_borderSizes.width);

	return imageSize;
}

void GridGenerator::setCellSize(const cv::Size& cellSize) 
{ 
	m_cellSize = cellSize; 
}

void GridGenerator::setMaxMergedCells(int maxMergedCells) 
{ 
	m_maxMergedCells = maxMergedCells; 
}

void GridGenerator::setMaxMergingDifference(int maxMergingDifference) 
{ 
	m_maxMergingDifference = maxMergingDifference; 
}

void GridGenerator::setCellSwapRatio(float cellSwapRatio) 
{ 
	m_cellSwapRatio = cellSwapRatio; 
}

void GridGenerator::setBorderSizes(const cv::Size& borderSizes)
{ 
	m_borderSizes = borderSizes;
}

void GridGenerator::setBorderColor(const cv::Vec3b& borderColor)
{ 
	m_borderColor = borderColor;
}

Grid GridGenerator::generate(InputImage inputImage)
{
	Grid grid; 

	const cv::Mat& image = inputImage.getImage(); 

	//compute mean values for all cells if there is no stencil
	bool useAllCells = true; 
	if (Utility::contains(inputImage.getStencil(), InputImage::STENCIL))
	{
		useAllCells = false;
	}

	cv::Size cellCount = getCellCount(inputImage); 
	cv::Size gridImageSize = getGridImageSize(inputImage);  

	try
	{
		grid.m_gridImage = cv::Mat(gridImageSize.height, gridImageSize.width, CV_8UC3);
		grid.m_gridStructure = cv::Mat(cellCount.height, cellCount.width, CV_32SC3);
		grid.m_gridStructure.setTo(cv::Scalar(0, 0, 0));
	}
	catch (cv::Exception e)
	{
		grid.m_gridImage = cv::Mat(0, 0, CV_8UC3);
		grid.m_gridStructure = cv::Mat(0, 0, CV_32SC3);

		return grid;
	}

	//resize stencil and input image so they are the same size as the grid image without borders

	const cv::Mat& stencil = inputImage.getStencil();
	cv::Mat resizedStencil; 
	if (!Utility::resizeMat(stencil, resizedStencil, m_cellSize.width * cellCount.width, m_cellSize.height * cellCount.height))
	{
		grid.m_gridImage = cv::Mat(0, 0, CV_8UC3);
		grid.m_gridStructure = cv::Mat(0, 0, CV_32SC3);

		return grid;
	}

	cv::Mat resizedImage;
	if (!Utility::resizeMat(image, resizedImage, m_cellSize.width * cellCount.width, m_cellSize.height * cellCount.height))
	{
		grid.m_gridImage = cv::Mat(0, 0, CV_8UC3);
		grid.m_gridStructure = cv::Mat(0, 0, CV_32SC3);

		return grid;
	}

	int mergedCellSize = m_maxMergedCells; //size of merged cell in each dimension for the current iteration (in base cells)

	//number of iterations until we get to merged cell of size 1 (base cell size) (the size halves in each iteration)
	int iterationCount = 0;
	for (int j = mergedCellSize; j >= 1; j /= 2)
	{
		iterationCount++;
	}

	int iteration = 0; 

	//cells of color means
	//first is the coordinate of the upper left corner of the cell 
	//second is the mean color value for the cell
	std::vector<std::pair<cv::Point, cv::Scalar>> meanCells;

	//in each iteration search for places where to put merged cells of current size 
	while (mergedCellSize >= 1) 
	{
		//go through all regions of given size
		for (int gridRow = 0; gridRow < cellCount.height; gridRow += mergedCellSize)
		{
			for (int gridCol = 0; gridCol < cellCount.width; gridCol += mergedCellSize)
			{
				bool fits = true;      //does a merged cell of current size fit in this region
				bool original = false; //will this region be taken from the input image (not used for the mosaic)

				//go through all subcells of the current region
				for (int gridRow2 = gridRow; gridRow2 < gridRow + mergedCellSize; ++gridRow2)
				{
					for (int gridCol2 = gridCol; gridCol2 < gridCol + mergedCellSize; ++gridCol2)
					{
						//get the corresponding stencil cell

						cv::Mat stencilCell = Utility::getSubMat(resizedStencil, gridCol2 * m_cellSize.width, gridRow2 * m_cellSize.height, m_cellSize.width, m_cellSize.height);

						if (mergedCellSize == 1) //region is of size 1
						{
							if (grid.m_gridStructure.at<cv::Vec3i>(gridRow2, gridCol2) != cv::Vec3i(0, 0, 0))
							{
								//this subcell is already set
								fits = false;
								break;
							}
							else if (!useAllCells && !Utility::contains(stencilCell, InputImage::STENCIL))
							{
								//this subcell is not yet set and is not part of stencil
								original = true;
							}
						}
						else //region is of size greater than 1
						{
							if (grid.m_gridStructure.at<cv::Vec3i>(gridRow2, gridCol2) != cv::Vec3i(0, 0, 0) || (!useAllCells && !Utility::contains(stencilCell, InputImage::STENCIL)))
							{
								//this subcell is already set or is not part of stencil
								fits = false;
								break;
							}
						}
					}

					if (!fits) break;
				}

				if (!fits) continue;

				//merged cell of current size fits in this region

				//get the corresponding part of the input image

				int x = gridCol * m_cellSize.width;
				int y = gridRow * m_cellSize.height;
				int width = m_cellSize.width * mergedCellSize;
				int height = m_cellSize.height * mergedCellSize;

				cv::Mat imageCell = Utility::getSubMat(resizedImage, x, y, width, height);

				bool done = false; //gets set to true if the current region is resolved in this iteration

				if (original == true) //this region is not part of stencil, cell will contain part of input image
				{
					//get the corresponding part of the grid image 

					int x, y, width, height;
				   
					if (gridRow == 0)
					{
						y = 0;
						height = ((m_borderSizes.height + m_cellSize.height) * mergedCellSize) + m_borderSizes.height;
					}
					else
					{
						y = (gridRow * (m_borderSizes.height + m_cellSize.height)) + m_borderSizes.height;
						height = (m_cellSize.height + m_borderSizes.height) * mergedCellSize;
					}

					if (gridRow + 1 < cellCount.height && grid.m_gridStructure.at<cv::Vec3i>(gridRow + 1, gridCol)[2] != 0)
					{
						height -= m_borderSizes.height;
					}

					if (gridCol == 0)
					{
						x = 0;
						width = ((m_borderSizes.width + m_cellSize.width) * mergedCellSize) + m_borderSizes.width;
					}
					else
					{
						x = (gridCol * (m_borderSizes.width + m_cellSize.width)) + m_borderSizes.width;
						width = (m_cellSize.width + m_borderSizes.width) * mergedCellSize;
					}

					if (gridCol + 1 < cellCount.width && grid.m_gridStructure.at<cv::Vec3i>(gridRow, gridCol + 1)[2] != 0)
					{
						width -= m_borderSizes.width;
					}

					cv::Mat gridCell = Utility::getSubMat(grid.m_gridImage, x, y, width, height);

					//resize corresponding part of input image accordingly

					cv::Mat resizedImageCell;
					if (!Utility::resizeMat(imageCell, resizedImageCell, width, height))
					{
						grid.m_gridImage = cv::Mat(0, 0, CV_8UC3);
						grid.m_gridStructure = cv::Mat(0, 0, CV_32SC3);

						return grid;
					}

					//copy corresponding part of input image into this region of the grid image

					resizedImageCell.copyTo(gridCell);
					done = true; //region resolved
				}
				else //this region is part of stencil, cell will contain mean color
				{
					cv::Scalar mean;   //mean color in this region of input image
					cv::Scalar stddev; //standard deviation of color in this region of input image
					cv::meanStdDev(imageCell, mean, stddev);

					//if this region is of size 1 or the standard deviation of color within this region is under the given threshold make this a mean cell
					if (mergedCellSize == 1 || sqrt(pow(stddev[0], 2.0) + pow(stddev[1], 2.0) + pow(stddev[2], 2.0)) <= m_maxMergingDifference)
					{
						meanCells.push_back(std::make_pair(cv::Point(gridCol, gridRow), mean));
						done = true; //region resolved
					}
				}

				if (!done) continue; //couldn't resolve this region yet

				//update the grid structure

				for (int gridRow2 = gridRow; gridRow2 < gridRow + mergedCellSize; ++gridRow2)
				{
					for (int gridCol2 = gridCol; gridCol2 < gridCol + mergedCellSize; ++gridCol2)
					{
						if (original == true)
						{
							grid.m_gridStructure.at<cv::Vec3i>(gridRow2, gridCol2) = cv::Vec3i(mergedCellSize - (gridRow2 - gridRow), mergedCellSize - (gridCol2 - gridCol), 0);
						}
						else
						{
							grid.m_gridStructure.at<cv::Vec3i>(gridRow2, gridCol2) = cv::Vec3i(mergedCellSize - (gridRow2 - gridRow), mergedCellSize - (gridCol2 - gridCol), 1);
						}
					}
				}
			}
		}

		emit progress(static_cast<float>(iteration) / iterationCount);

		iteration++;
		mergedCellSize /= 2;
	}

	//swap mean values of the given portion of mean cells

	int swapCount = static_cast<int>(m_cellSwapRatio * meanCells.size());

	std::shuffle(meanCells.begin(), meanCells.end(), m_randomEngine);

	std::vector<cv::Scalar> means;
	means.reserve(swapCount);
	for (int i = 0; i < swapCount; ++i)
	{
		means.push_back(meanCells[i].second);
	}

	std::shuffle(means.begin(), means.end(), m_randomEngine);

	for (int i = 0; i < swapCount; ++i)
	{
		meanCells[i].second = means[i];
	}

	//create the grid image

	for (auto meanCell : meanCells)
	{
		int gridRow = meanCell.first.y;
		int gridCol = meanCell.first.x;
		int mergedCellSize = grid.m_gridStructure.at<cv::Vec3i>(gridRow, gridCol)[0];
		cv::Scalar mean = meanCell.second;

		cv::Mat dstCell = Utility::getSubMat(
			grid.m_gridImage, 
			gridCol * (m_cellSize.width + m_borderSizes.width), 
			gridRow * (m_borderSizes.height + m_cellSize.height), 
			((m_borderSizes.width + m_cellSize.width) * mergedCellSize) + m_borderSizes.width, 
			((m_borderSizes.height + m_cellSize.height) * mergedCellSize) + m_borderSizes.height);

		cv::Mat srcCell = cv::Mat(
			((m_cellSize.height + m_borderSizes.height) * mergedCellSize) - m_borderSizes.height,
			((m_cellSize.width + m_borderSizes.width) * mergedCellSize) - m_borderSizes.width,
			CV_8UC3, 
			mean);

		cv::copyMakeBorder(srcCell, srcCell, m_borderSizes.height, m_borderSizes.height, m_borderSizes.width, m_borderSizes.width, cv::BORDER_CONSTANT, cv::Scalar(m_borderColor[0], m_borderColor[1], m_borderColor[2]));
		
		srcCell.copyTo(dstCell);
	}

	grid.m_cellSize = m_cellSize;
	grid.m_cellCount = cellCount;
	grid.m_borderSizes = m_borderSizes;

	emit progress(1.0f);

	return grid;
}
