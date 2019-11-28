#include "Mosaic.h"

Mosaic::Mosaic(const Mosaic& other)
	: m_mosaic(other.m_mosaic.clone())
{
}

Mosaic& Mosaic::operator=(const Mosaic& other)
{
	if (this == &other)
	{
		return *this;
	}

	m_mosaic = other.m_mosaic.clone();

	return *this;
}

const cv::Mat& Mosaic::getMosaicImage() const 
{ 
	return m_mosaic; 
}

bool Mosaic::isEmpty() const 
{ 
	return m_mosaic.empty(); 
}
