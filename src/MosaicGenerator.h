#pragma once

#include <QObject>

#include "InputImage.h"
#include "Grid.h"
#include "TileSet.h"
#include "Mosaic.h"

class MosaicGenerator : public QObject
{
	Q_OBJECT

public:

	enum class BlendingMode 
	{ 
		MEANS, //!< blend with input image
		IMAGE  //!< blend with mean colors
	};

	const cv::Size& getTileSize() const;

	const cv::Size& getBorderSizes() const;

	const cv::Vec3b& getBorderColor() const;

	int getNumNearest() const;

	float getBlendingFactor() const;

	BlendingMode getBlendingMode() const;

	cv::Size getMosaicImageSize(const InputImage& inputImage, const Grid& grid) const;

	void setTileSize(const cv::Size& tileSize);

	void setBorderSizes(const cv::Size& borderSizes);

	void setBorderColor(const cv::Vec3b& borderColor);

	void setNumNearest(int numNearest);

	void setBlendingFactor(float blending);

	void setBlendingMode(BlendingMode blendingMode);

	Mosaic generate(InputImage inputImage, Grid grid, TileSet tileSet);

signals:

	void progress(float progress);

private:

	cv::Size m_tileSize = cv::Size(10, 10);  //!< size of base-sized tile
	cv::Size m_borderSizes = cv::Size(1, 1); //!< thickness of borders between tiles in each dimension
	int m_numNearest = 1;                    //!< number of most similar images to choose from for each tile
	float m_blendingFactor = 0.0f;           //!< degree to which the mosaic will be blended with input image or mean colors
	BlendingMode m_blendingMode = BlendingMode::MEANS; //!< blend with input image or mean colors
	cv::Vec3b m_spaceColor = cv::Vec3b(0, 0, 0); //!< color of borders between tiles

	cv::Mat getBlendedTile(const cv::Mat& tile, const cv::Vec3b& color, float blendingFactor) const;

	cv::Mat getBlendedTile(const cv::Mat& tile1, const cv::Mat& tile2, float blendingFactor) const;
};

