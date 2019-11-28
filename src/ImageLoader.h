#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>

#include "TileSet.h"

class ImageLoader
{
private:

	/**
	*   \brief Enlarges the source image to a given size by adding borders of given color around it, the original image will be in the center of the destination image 
	*   \param src source image
	*   \param dst destination image where the result is stored 
	*   \param width desired width, must be greater or equal to the source image width
	*   \param height desired height, must be greater or equal to the source image height
	*   \param color color which will be used for the borders
	*   \return false if error occured or if the desired size is smaller than the source image size in either dimension, true otherwise
	*/
	static bool addSpace(const cv::Mat& src, cv::Mat& dst, int width, int height, const cv::Vec3b& color = cv::Vec3b(255, 255, 255));

public:

	static cv::Mat loadImage(const std::string& path);

	static Tile loadTile(const std::string& path, int iconWidth, int iconHeight);
};
