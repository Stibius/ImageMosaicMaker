#include "MosaicGenerator.h"
#include "ImageLoader.h"
#include "Utility.h"

const cv::Size& MosaicGenerator::getTileSize() const 
{ 
	return m_tileSize; 
}

const cv::Size& MosaicGenerator::getBorderSizes() const 
{ 
	return m_borderSizes; 
}

int MosaicGenerator::getNumNearest() const 
{ 
	return m_numNearest; 
}

float MosaicGenerator::getBlendingFactor() const 
{ 
	return m_blendingFactor; 
}

MosaicGenerator::BlendingMode MosaicGenerator::getBlendingMode() const 
{ 
	return m_blendingMode; 
}

const cv::Vec3b& MosaicGenerator::getBorderColor() const 
{ 
	return m_spaceColor; 
}

cv::Mat MosaicGenerator::getBlendedTile(const cv::Mat& tile, const cv::Vec3b& color, float blendingFactor) const
{
	cv::Mat blendedTile;

	cv::Mat colorTile = cv::Mat(tile.rows, tile.cols, CV_8UC3, cv::Scalar(color[0], color[1], color[2]));
	
	cv::addWeighted(tile, 1.0f - blendingFactor, colorTile, blendingFactor, 0.0, blendedTile);

	return blendedTile;
}

cv::Mat MosaicGenerator::getBlendedTile(const cv::Mat& tile1, const cv::Mat& tile2, float blendingFactor) const
{
	cv::Mat blendedTile;

	cv::addWeighted(tile1, 1.0f - blendingFactor, tile2, blendingFactor, 0.0, blendedTile);

	return blendedTile;
}

cv::Size MosaicGenerator::getMosaicImageSize(const InputImage& inputImage, const Grid& grid) const
{
	cv::Size cellCount = cv::Size(grid.getGridStructure().cols, grid.getGridStructure().rows);

	cv::Size result;

	result.height = (m_tileSize.height * cellCount.height) + ((cellCount.height + 1) * m_borderSizes.height);
	result.width = (m_tileSize.width * cellCount.width) + ((cellCount.width + 1) * m_borderSizes.width);

	return result;
}

void MosaicGenerator::setTileSize(const cv::Size& tileSize) 
{ 
	m_tileSize = tileSize; 
}

void MosaicGenerator::setBorderSizes(const cv::Size& borderSizes) 
{ 
	m_borderSizes = borderSizes;
}

void MosaicGenerator::setNumNearest(int numNearest) 
{ 
	m_numNearest = numNearest; 
}

void MosaicGenerator::setBlendingFactor(float blending) 
{ 
	m_blendingFactor = blending; 
}

void MosaicGenerator::setBlendingMode(BlendingMode blendingMode) 
{ 
	m_blendingMode = blendingMode; 
}

void MosaicGenerator::setBorderColor(const cv::Vec3b& borderColor)
{ 
	m_spaceColor = borderColor;
}

Mosaic MosaicGenerator::generate(InputImage inputImage, Grid grid, TileSet tileSet)
{
	Mosaic mosaic;

	const cv::Mat& gridStructure = grid.getGridStructure();
	const cv::Mat& gridImage = grid.getGridImage();
	cv::Size gridCellCount = cv::Size(gridStructure.cols, gridStructure.rows);
	const cv::Size& gridCellSize = grid.getCellSize();
	const cv::Size& gridBorderSizes = grid.getBorderSizes();

	cv::Size mosaicImageSize = getMosaicImageSize(inputImage, grid);

	try
	{
		mosaic.m_mosaic = cv::Mat(mosaicImageSize.height, mosaicImageSize.width, CV_8UC3);
	}
	catch (cv::Exception e)
	{
		mosaic.m_mosaic = cv::Mat(0, 0, CV_8UC3);

		return mosaic;
	}

	//resize input image so it is the same size as the grid image without borders

	const cv::Mat& image = inputImage.getImage();
	cv::Mat resizedImage;
	if (!Utility::resizeMat(image, resizedImage, gridCellSize.width * gridCellCount.width, gridCellSize.height * gridCellCount.height))
	{
		mosaic.m_mosaic = cv::Mat(0, 0, CV_8UC3);

		return mosaic;
	}

	//go through all grid cells
	for (int gridRow = 0; gridRow < gridCellCount.height; ++gridRow)
	{
		for (int gridCol = 0; gridCol < gridCellCount.width; ++gridCol)
		{
			bool skip = true; //if this cell is part of a larger merged cell which has already been processed, this will remain true

			if (gridRow == 0 && gridCol == 0)
			{
				skip = false;
			}
			else if (gridRow == 0)
			{
				if (gridStructure.at<cv::Vec3i>(gridRow, gridCol - 1)[1] <= gridStructure.at<cv::Vec3i>(gridRow, gridCol)[1])
				{
					skip = false;
				}
			}
			else if (gridCol == 0)
			{
				if (gridStructure.at<cv::Vec3i>(gridRow - 1, gridCol)[0] <= gridStructure.at<cv::Vec3i>(gridRow, gridCol)[0])
				{
					skip = false;
				}
			}
			else
			{
				if (gridStructure.at<cv::Vec3i>(gridRow - 1, gridCol)[0] <= gridStructure.at<cv::Vec3i>(gridRow, gridCol)[0] &&
					gridStructure.at<cv::Vec3i>(gridRow, gridCol - 1)[1] <= gridStructure.at<cv::Vec3i>(gridRow, gridCol)[1])
				{
					skip = false;
				}
			}

			if (skip) continue;

			//this cell is the top left corner of larger merged cell or just a single cell

			int mergedCellSize = gridStructure.at<cv::Vec3i>(gridRow, gridCol)[0];

			if (gridStructure.at<cv::Vec3i>(gridRow, gridCol)[2] == 1) //this region of the grid contains a mean color
			{
				//get tile with nearest mean

				const cv::Vec3b& gridMean = gridImage.at<cv::Vec3b>((gridRow * (gridCellSize.height + gridBorderSizes.height)) + gridBorderSizes.height, (gridCol * (gridCellSize.width + gridBorderSizes.width)) + gridBorderSizes.width);
				cv::Mat tile = tileSet.getClosestTile(gridMean, m_numNearest).getImage().clone(); 

				//blend tile with mean

				if (m_blendingFactor != 0.0 && m_blendingMode == BlendingMode::MEANS)
				{
					tile = getBlendedTile(tile, gridMean, m_blendingFactor);
				}

				//resize the tile

				int resultTileHeight = ((m_tileSize.height + m_borderSizes.height) * mergedCellSize) - m_borderSizes.height;
				int resultTileWidth = ((m_tileSize.width + m_borderSizes.width) * mergedCellSize) - m_borderSizes.width;

				cv::Mat resizedTile;
				if (!Utility::resizeMat(tile, resizedTile, resultTileWidth, resultTileHeight))
				{
					mosaic.m_mosaic = cv::Mat(0, 0, CV_8UC3);

					return mosaic;
				}

				//blend tile with corresponding region of input image

				if (m_blendingFactor != 0.0 && m_blendingMode == BlendingMode::IMAGE)
				{
					cv::Mat imageCell = Utility::getSubMat(resizedImage, gridCol * gridCellSize.width, gridRow * gridCellSize.height, gridCellSize.width * mergedCellSize, gridCellSize.height * mergedCellSize);
					cv::Mat resizedImageCell;
					if (!Utility::resizeMat(imageCell, resizedImageCell, resizedTile.cols, resizedTile.rows))
					{
						mosaic.m_mosaic = cv::Mat(0, 0, CV_8UC3);

						return mosaic;
					}

					resizedTile = getBlendedTile(resizedTile, resizedImageCell, m_blendingFactor);
				}

				//get corresponding region of the result image

				int x = gridCol * (m_borderSizes.width + m_tileSize.width);
				int y = gridRow * (m_borderSizes.height + m_tileSize.height);
				int width = ((m_borderSizes.width + m_tileSize.width) * mergedCellSize) + m_borderSizes.width;
				int height = ((m_borderSizes.height + m_tileSize.height) * mergedCellSize) + m_borderSizes.height;

				cv::Mat resultArea = Utility::getSubMat(mosaic.m_mosaic, x, y, width, height);

				//copy resulting tile into corresponding region of the result image

				cv::copyMakeBorder(resizedTile, resizedTile, m_borderSizes.height, m_borderSizes.height, m_borderSizes.width, m_borderSizes.width, cv::BORDER_CONSTANT, cv::Scalar(m_spaceColor[0], m_spaceColor[1], m_spaceColor[2]));
				resizedTile.copyTo(resultArea);
			}
			else //this region of the grid contains input image data
			{
				//get corresponding region of the result image

				int x, y, width, height;

				if (gridRow == 0)
				{
					y = 0;
					height = ((m_borderSizes.height + m_tileSize.height) * mergedCellSize) + m_borderSizes.height;
				}
				else
				{
					y = (gridRow * (m_borderSizes.height + m_tileSize.height)) + m_borderSizes.height;
					height = (m_tileSize.height + m_borderSizes.height) * mergedCellSize;
				}

				if (gridRow + 1 < gridCellCount.height && gridStructure.at<cv::Vec3i>(gridRow + 1, gridCol)[2] != 0)
				{
					height -= m_borderSizes.height;
				}

				if (gridCol == 0)
				{
					x = 0;
					width = ((m_borderSizes.width + m_tileSize.width) * mergedCellSize) + m_borderSizes.width;
				}
				else
				{
					x = (gridCol * (m_borderSizes.width + m_tileSize.width)) + m_borderSizes.width;
					width = (m_tileSize.width + m_borderSizes.width) * mergedCellSize;
				}

				if (gridCol + 1 < gridCellCount.width && gridStructure.at<cv::Vec3i>(gridRow, gridCol + 1)[2] != 0)
				{
					width -= m_borderSizes.width;
				}

				//copy region of the input image into corresponding region of the result image

				cv::Mat resultArea = Utility::getSubMat(mosaic.m_mosaic, x, y, width, height);

				cv::Mat imageCell = Utility::getSubMat(resizedImage, gridCol * gridCellSize.width, gridRow * gridCellSize.height, gridCellSize.width * mergedCellSize, gridCellSize.height * mergedCellSize);
				cv::Mat resizedImageCell;
				if (!Utility::resizeMat(imageCell, resizedImageCell, width, height))
				{
					mosaic.m_mosaic = cv::Mat(0, 0, CV_8UC3);

					return mosaic;
				}
				resizedImageCell.copyTo(resultArea);
			}
		}

		emit progress(static_cast<float>(gridRow) / gridCellCount.height);
	}

	emit progress(1.0f);

	return mosaic;
}
