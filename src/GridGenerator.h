#pragma once

#include <QObject>

#include <random>

#include "InputImage.h"
#include "Grid.h"

class GridGenerator : public QObject
{
	Q_OBJECT

public:

	const cv::Size& getCellSize() const;

	int getMaxMergedCells() const;

	int getMaxMergingDifference() const;

	float getCellSwapRatio() const;

	const cv::Size& getBorderSizes() const;

	const cv::Vec3b& getBorderColor() const;

	cv::Size getCellCount(const InputImage& inputImage) const;

	cv::Size getGridImageSize(const InputImage& inputImage) const;

	void setCellSize(const cv::Size& cellSize);

	void setMaxMergedCells(int maxMergedCells);

	void setMaxMergingDifference(int maxMergingDifference);

	void setCellSwapRatio(float cellSwapRatio);

	void setBorderSizes(const cv::Size& borderSizes);

	void setBorderColor(const cv::Vec3b& borderColor);

	Grid generate(InputImage inputImage);

signals:

	void progress(float progress);

private:

	cv::Size m_cellSize = cv::Size(10, 10);       //!< size of base cell
	int m_maxMergedCells = 16;                    //!< maximum number of base cells which can be merged in each dimension
	int m_maxMergingDifference = 20;              //!< maximum difference of two cells so they can be merged 
	float m_cellSwapRatio = 0.0f;                 //!< fraction of cells for which colors will be randomly swapped
	cv::Size m_borderSizes = cv::Size(1, 1);      //!< thickness of borders between cells in each dimension
	cv::Vec3b m_borderColor = cv::Vec3b(0, 0, 0); //!< color of borders between cells

	static std::default_random_engine m_randomEngine;
};
