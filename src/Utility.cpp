#include "Utility.h"

cv::Mat Utility::getSubMat(const cv::Mat& mat, int x, int y, int width, int height)
{
	cv::Range yRange = cv::Range(y, y + height);
	cv::Range xRange = cv::Range(x, x + width);

	return mat.rowRange(yRange).colRange(xRange);
}

bool Utility::contains(const cv::Mat& mat, uchar value)
{
	for (int row = 0; row < mat.rows; ++row)
	{
		for (int col = 0; col < mat.cols; ++col)
		{
			if (mat.at<uchar>(row, col) == value)
			{
				return true;
			}
		}
	}

	return false;
}

int Utility::distance(const cv::Vec3b& color1, const cv::Vec3b& color2)
{ 
	return static_cast<int>(sqrt(pow(color1[0] - color2[0], 2.0) + pow(color1[1] - color2[1], 2.0) + pow(color1[2] - color2[2], 2.0))); 
}

bool Utility::resizeMat(const cv::Mat& src, cv::Mat& dst, int width, int height)
{
	if ((src.cols == width && src.rows == height) || (src.cols == 0 && src.rows == 0))
	{
		dst = src;
		return true;
	}

	int interpolation;

	double fx = static_cast<double>(width) / src.cols;
	double fy = static_cast<double>(height) / src.rows;

	if (width * height >= src.cols * src.rows)
	{
		interpolation = cv::INTER_LINEAR;
	}
	else
	{
		interpolation = cv::INTER_AREA;
	}

	try
	{
		cv::resize(src, dst, cv::Size(width, height), fx, fy, interpolation);
	}
	catch (cv::Exception e)
	{
		return false;
	}

	return true;
}

QPixmap Utility::cvMatToQPixmap(const cv::Mat& mat)
{
	//http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/

	switch (mat.type())
	{
	case CV_8UC4:
	{
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);

		return QPixmap::fromImage(image);
	}
	case CV_8UC3:
	{
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

		return QPixmap::fromImage(image.rgbSwapped());
	}
	case CV_8UC1:
	{
		static QVector<QRgb>  sColorTable;

		if (sColorTable.isEmpty())
		{
			for (int i = 0; i < 256; ++i)
			{
				sColorTable.push_back(qRgb(i, i, i));
			}
		}

		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);

		image.setColorTable(sColorTable);

		return QPixmap::fromImage(image);
	}
	default:
		break;
	}

	return QPixmap();
}

QColor Utility::vecToQColor(const cv::Vec3b& vec)
{ 
	return QColor(vec[2], vec[1], vec[0], 255); 
}

cv::Vec3b Utility::QColorToVec(const QColor& color)
{ 
	return cv::Vec3b(color.blue(), color.green(), color.red()); 
}
