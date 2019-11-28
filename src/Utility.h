#pragma once

#include <opencv2/imgproc/imgproc.hpp>

#include <QPixmap>
#include <QColor>

class Utility
{
public:

	static cv::Mat getSubMat(const cv::Mat& mat, int x, int y, int width, int height);

	static bool contains(const cv::Mat& mat, uchar value);

	static int distance(const cv::Vec3b& color1, const cv::Vec3b& color2);

	static bool resizeMat(const cv::Mat& src, cv::Mat& dst, int width, int height);

	static QPixmap cvMatToQPixmap(const cv::Mat& mat);

	static QColor vecToQColor(const cv::Vec3b& vec);

	static cv::Vec3b QColorToVec(const QColor& color);
};
