#include "Grid.h"

Grid::Grid(const Grid& other)
	: m_gridImage(other.m_gridImage.clone())
	, m_gridStructure(other.m_gridStructure.clone())
	, m_cellSize(other.m_cellSize)
	, m_cellCount(other.m_cellCount)
	, m_borderSizes(other.m_borderSizes)
{
}

Grid& Grid::operator=(const Grid& other)
{
	if (&other == this)
	{
		return *this;
	}

	m_gridImage = other.m_gridImage.clone();
	m_gridStructure = other.m_gridStructure.clone();
	m_cellSize = other.m_cellSize;
	m_cellCount = other.m_cellCount;
	m_borderSizes = other.m_borderSizes;

	return *this;
}

const cv::Mat& Grid::getGridImage() const 
{ 
	return m_gridImage; 
}

const cv::Mat& Grid::getGridStructure() const 
{ 
	return m_gridStructure; 
}

const cv::Size& Grid::getCellSize() const 
{ 
	return m_cellSize; 
}

const cv::Size& Grid::getCellCount() const 
{ 
	return m_cellCount; 
}

const cv::Size& Grid::getBorderSizes() const 
{ 
	return m_borderSizes; 
}

bool Grid::isEmpty() const 
{ 
	return m_gridImage.empty(); 
}
