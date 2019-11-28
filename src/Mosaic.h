#pragma once

#include <opencv2/imgproc/imgproc.hpp>

class Mosaic 
{
	friend class MosaicGenerator;

private:

	cv::Mat m_mosaic = cv::Mat(0, 0, CV_8UC3);

public:

	Mosaic() = default;

	Mosaic(const Mosaic& other);

	Mosaic(Mosaic&& other) = default;

	Mosaic& operator=(const Mosaic& other);

	Mosaic& operator=(Mosaic&& other) = default;

	~Mosaic() = default;

	const cv::Mat& getMosaicImage() const;

	bool isEmpty() const;
};


