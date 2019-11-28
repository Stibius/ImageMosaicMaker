#include "ImageLoader.h"
#include "Utility.h"

bool ImageLoader::addSpace(const cv::Mat& src, cv::Mat& dst, int width, int height, const cv::Vec3b& color)
{
	if (width < src.cols || height < src.rows) return false;

	try
	{
		cv::Mat result = cv::Mat(height, width, CV_8UC3, cv::Scalar(color[0], color[1], color[2]));

		for (int row = 0; row < src.rows; ++row)
		{
			for (int col = 0; col < src.cols; ++col)
			{
				result.at<cv::Vec3b>(row + ((height - src.rows) / 2), col + ((width - src.cols) / 2)) = src.at<cv::Vec3b>(row, col);
			}
		}

		dst = result;
	}
	catch (cv::Exception e)
	{
		return false;
	}

	return true;
}

cv::Mat ImageLoader::loadImage(const std::string& path)
{
	cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

	cv::Mat imageAlpha = cv::imread(path, cv::IMREAD_UNCHANGED);

	if (image.empty() || imageAlpha.empty())
	{
		return cv::Mat();
	}

	if (imageAlpha.channels() != 4)
	{
		return image;
	}

	for (int row = 0; row < image.rows; ++row)
	{
		for (int col = 0; col < image.cols; ++col)
		{
			const cv::Vec4b& color = imageAlpha.at<cv::Vec4b>(row, col);

			for (int i = 0; i < 3; i++)
			{
				image.at<cv::Vec3b>(row, col)[i] = color[i] + static_cast<unsigned char>(((255 - color[3]) / 255.0) * (255 - color[i]));
			}
		}
	}

	return image;
}

Tile ImageLoader::loadTile(const std::string & path, int iconWidth, int iconHeight)
{
	cv::Mat image = loadImage(path);
	if (image.empty())
	{
		return Tile();
	}

	cv::Scalar s = cv::mean(image);
	cv::Vec3b mean = cv::Vec3b(s[0], s[1], s[2]);

	int squareSize;
	if (image.cols > image.rows)
	{
		squareSize = image.cols;
	}
	else
	{
		squareSize = image.rows;
	}

	cv::Mat icon;
	if (!addSpace(image, icon, squareSize, squareSize, cv::Vec3b(255, 255, 255)))
	{
		return Tile();
	}
	if (!Utility::resizeMat(icon, icon, iconWidth, iconHeight))
	{
		return Tile();
	}

	return Tile(path, image, icon, mean);
}
