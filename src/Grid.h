#pragma once

#include "InputImage.h"

class Grid 
{
	friend class GridGenerator;

private:

	cv::Mat m_gridImage = cv::Mat(0, 0, CV_8UC3); 

	/**
	*   Contains as many elements as there are cells in the grid.
	*   Each element contains 3 values:
	*   v0: number of cells to the bottom of this cell which are merged (including this cell)
	*   v1: number of cells to the right of this cell which are merged (including this cell)
	*   v2: 1 if the cell contains a mean color, 0 if the cell contains input image data
	*/
	cv::Mat m_gridStructure = cv::Mat(0, 0, CV_32SC3);  

	cv::Size m_cellSize = cv::Size(0, 0);    //!< size of base cell
	cv::Size m_cellCount = cv::Size(0, 0);   //!< number of base cells which can fit in each dimension
	cv::Size m_borderSizes = cv::Size(0, 0); //!< thickness of borders between cells in each dimension

public:

	Grid() = default;

	Grid(const Grid& other);

	Grid(Grid&& other) = default;

	Grid& operator=(const Grid& other);

	Grid& operator=(Grid&& other) = default;

	~Grid() = default;

	const cv::Mat& getGridImage() const;

	const cv::Mat& getGridStructure() const;

	const cv::Size& getCellSize() const;

	const cv::Size& getCellCount() const;

	const cv::Size& getBorderSizes() const;

	bool isEmpty() const;

};

