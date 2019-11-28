#include "InputImage.h"

InputImage::InputImage()
{
	m_stencils.push_back(cv::Mat(0, 0, CV_8UC1));
	m_stencil = m_stencils.begin();
	m_stencilImages.push_back(cv::Mat(0, 0, CV_8UC3));
	m_stencilImage = m_stencilImages.begin();
}

InputImage::InputImage(const cv::Mat& image)
{
	setImage(image);
}

InputImage::InputImage(const InputImage& other)
	: m_image(other.m_image.clone())
	, m_stencilColor(other.m_stencilColor)
{
	m_stencils.reserve(other.m_stencils.size());
	for (const cv::Mat& mat : other.m_stencils)
	{
		m_stencils.push_back(mat.clone());
	}
	m_stencil = m_stencils.begin() + (other.m_stencil - other.m_stencils.begin());
	
	m_stencilImages.reserve(other.m_stencilImages.size());
	for (const cv::Mat& mat : other.m_stencilImages)
	{
		m_stencilImages.push_back(mat.clone());
	}
	m_stencilImage = m_stencilImages.begin() + (other.m_stencilImage - other.m_stencilImages.begin());
}

InputImage& InputImage::operator=(const InputImage& other)
{
	if (&other == this)
	{
		return *this;
	}

	m_image = other.m_image.clone();
	m_stencilColor = other.m_stencilColor;

	m_stencils.clear();
	m_stencils.reserve(other.m_stencils.size());
	for (const cv::Mat& mat : other.m_stencils)
	{
		m_stencils.push_back(mat.clone());
	}
	m_stencil = m_stencils.begin() + (other.m_stencil - other.m_stencils.begin());

	m_stencilImages.clear();
	m_stencilImages.reserve(other.m_stencilImages.size());
	for (const cv::Mat& mat : other.m_stencilImages)
	{
		m_stencilImages.push_back(mat.clone());
	}
	m_stencilImage = m_stencilImages.begin() + (other.m_stencilImage - other.m_stencilImages.begin());

	return *this;
}

const cv::Mat& InputImage::getImage() const 
{ 
	return m_image; 
}

const cv::Mat& InputImage::getStencilImage() const 
{ 
	return *m_stencilImage; 
}

const cv::Mat& InputImage::getStencil() const 
{ 
	return *m_stencil; 
}

const cv::Vec3b& InputImage::getStencilColor() const 
{ 
	return m_stencilColor; 
}

bool InputImage::isEmpty() const 
{ 
	return m_image.empty(); 
}

bool InputImage::isFirstState() const 
{ 
	return m_stencilImage == m_stencilImages.begin(); 
}

bool InputImage::isLastState() const 
{ 
	return m_stencilImage == m_stencilImages.end() - 1; 
}

void InputImage::setImage(const cv::Mat& image)
{
	m_image = image.clone();

	m_stencils.clear();
	m_stencils.push_back(cv::Mat(m_image.rows, m_image.cols, CV_8UC1, cv::Scalar(0)));
	m_stencil = m_stencils.begin();

	m_stencilImages.clear();
	m_stencilImages.push_back(m_image.clone());
	m_stencilImage = m_stencilImages.begin();
}

void InputImage::addStencil(const cv::Point& point, int brushRadius, bool newState)
{
	if (isEmpty())
	{
		return;
	}

	if (newState == true)
	{
		m_stencils.erase(m_stencil + 1, m_stencils.end());
		m_stencils.push_back(m_stencil->clone());
		m_stencil = m_stencils.end() - 1;

		m_stencilImages.erase(m_stencilImage + 1, m_stencilImages.end());
		m_stencilImages.push_back(m_stencilImage->clone());
		m_stencilImage = m_stencilImages.end() - 1;
	}

	cv::circle(*m_stencil, point, brushRadius, cv::Scalar(STENCIL), -1);

	for (int row = point.y - brushRadius; row <= point.y + brushRadius; ++row)
	{
		if (row < 0 || row >= m_image.rows) continue;

		for (int col = point.x - brushRadius; col <= point.x + brushRadius; ++col)
		{
			if (col < 0 || col >= m_image.cols) continue;

			if (m_stencil->at<uchar>(row, col) == STENCIL)
			{
				m_stencilImage->at<cv::Vec3b>(row, col) = (m_stencilColor * STENCIL_ALPHA) + (m_image.at<cv::Vec3b>(row, col) * (1 - STENCIL_ALPHA));
			}
		}
	}	
}

void InputImage::addStencil(const cv::Point& startPoint, const cv::Point& endPoint, int brushRadius, bool newState)
{
	if (isEmpty())
	{
		return;
	}

	if (newState == true)
	{
		m_stencils.erase(m_stencil + 1, m_stencils.end());
		m_stencils.push_back(m_stencil->clone());
		m_stencil = m_stencils.end() - 1;

		m_stencilImages.erase(m_stencilImage + 1, m_stencilImages.end());
		m_stencilImages.push_back(m_stencilImage->clone());
		m_stencilImage = m_stencilImages.end() - 1;
	}

	cv::line(*m_stencil, startPoint, endPoint, cv::Scalar(STENCIL), 2 * brushRadius);

	int minX = cv::min(startPoint.x, endPoint.x);
	int maxX = cv::max(startPoint.x, endPoint.x);
	int minY = cv::min(startPoint.y, endPoint.y);
	int maxY = cv::max(startPoint.y, endPoint.y);

	for (int row = minY - brushRadius; row <= maxY + brushRadius; ++row)
	{
		if (row < 0 || row >= m_image.rows) continue;

		for (int col = minX - brushRadius; col <= maxX + brushRadius; ++col)
		{
			if (col < 0 || col >= m_image.cols) continue;

			if (m_stencil->at<uchar>(row, col) == STENCIL)
			{
				m_stencilImage->at<cv::Vec3b>(row, col) = (m_stencilColor * STENCIL_ALPHA) + (m_image.at<cv::Vec3b>(row, col) * (1 - STENCIL_ALPHA));
			}
		}
	}
}

void InputImage::removeStencil(const cv::Point& point, int brushRadius, bool newState)
{
	if (isEmpty())
	{
		return;
	}

	if (newState == true)
	{
		m_stencils.erase(m_stencil + 1, m_stencils.end());
		m_stencils.push_back(m_stencil->clone());
		m_stencil = m_stencils.end() - 1;

		m_stencilImages.erase(m_stencilImage + 1, m_stencilImages.end());
		m_stencilImages.push_back(m_stencilImage->clone());
		m_stencilImage = m_stencilImages.end() - 1;
	}

	cv::circle(*m_stencil, point, brushRadius, cv::Scalar(NO_STENCIL), -1);

	for (int row = point.y - brushRadius; row <= point.y + brushRadius; ++row)
	{
		if (row < 0 || row >= m_image.rows) continue;

		for (int col = point.x - brushRadius; col <= point.x + brushRadius; ++col)
		{
			if (col < 0 || col >= m_image.cols) continue;

			if (m_stencil->at<uchar>(row, col) == NO_STENCIL)
			{
				m_stencilImage->at<cv::Vec3b>(row, col) = m_image.at<cv::Vec3b>(row, col);
			}
		}
	}
}

void InputImage::removeStencil(const cv::Point& startPoint, const cv::Point& endPoint, int brushRadius, bool newState)
{
	if (isEmpty())
	{
		return;
	}

	if (newState == true)
	{
		m_stencils.erase(m_stencil + 1, m_stencils.end());
		m_stencils.push_back(m_stencil->clone());
		m_stencil = m_stencils.end() - 1;

		m_stencilImages.erase(m_stencilImage + 1, m_stencilImages.end());
		m_stencilImages.push_back(m_stencilImage->clone());
		m_stencilImage = m_stencilImages.end() - 1;
	}

	cv::line(*m_stencil, startPoint, endPoint, cv::Scalar(NO_STENCIL), 2 * brushRadius);

	int minX = cv::min(startPoint.x, endPoint.x);
	int maxX = cv::max(startPoint.x, endPoint.x);
	int minY = cv::min(startPoint.y, endPoint.y);
	int maxY = cv::max(startPoint.y, endPoint.y);

	for (int row = minY - brushRadius; row <= maxY + brushRadius; ++row)
	{
		if (row < 0 || row >= m_image.rows) continue;

		for (int col = minX - brushRadius; col <= maxX + brushRadius; ++col)
		{
			if (col < 0 || col >= m_image.cols) continue;

			if (m_stencil->at<uchar>(row, col) == NO_STENCIL)
			{
				m_stencilImage->at<cv::Vec3b>(row, col) = m_image.at<cv::Vec3b>(row, col);
			}
		}
	}
}

void InputImage::setStencilColor(const cv::Vec3b& color)
{
	m_stencilColor = color;

	for (int i = 0; i < m_stencils.size(); ++i)
	{
		for (int row = 0; row < m_image.rows; ++row)
		{
			for (int col = 0; col < m_image.cols; ++col)
			{
				if (m_stencils[i].at<uchar>(row, col) == STENCIL)
				{
					m_stencilImages[i].at<cv::Vec3b>(row, col) = (m_stencilColor * STENCIL_ALPHA) + (m_image.at<cv::Vec3b>(row, col) * (1 - STENCIL_ALPHA));
				}
			}
		}
	}
}

bool InputImage::undo()
{
	if (m_stencil != m_stencils.begin())
	{
		m_stencil--;
		m_stencilImage--;
	}

	if (m_stencil == m_stencils.begin())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool InputImage::redo()
{
	if (m_stencil != m_stencils.end() - 1)
	{
		m_stencil++;
		m_stencilImage++;
	}

	if (m_stencil == m_stencils.end() - 1)
	{
		return false;
	}
	else
	{
		return true;
	}
}
