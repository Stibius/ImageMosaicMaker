#pragma once

#include <opencv2/imgproc/imgproc.hpp>

class InputImage
{
private:

	cv::Mat m_image = cv::Mat(0, 0, CV_8UC3);      //!< input image
	std::vector<cv::Mat> m_stencils;               //!< states of stencil from oldest to newest
	std::vector<cv::Mat>::iterator m_stencil;      //!< current state of stencil
	std::vector<cv::Mat> m_stencilImages;          //!< states of the input image combined with stencil from oldest to newest
	std::vector<cv::Mat>::iterator m_stencilImage; //!< current state of the input image combined with stencil 

	cv::Vec3b m_stencilColor = cv::Vec3b(0, 0, 0);

public:

	static constexpr float STENCIL_ALPHA = 0.5f; //!< alpha value of the displayed stencil

	static constexpr int NO_STENCIL = 0; //!< value in stencil in non-stenciled parts

	static constexpr int STENCIL = 1;    //!< value in stencil in stenciled parts

	InputImage();

	explicit InputImage(const cv::Mat& image);

	InputImage(const InputImage& other);

	InputImage(InputImage&& other) = default;

	InputImage& operator=(const InputImage& other);

	InputImage& operator=(InputImage&& other) = default;

	~InputImage() = default;

	const cv::Mat& getImage() const;

	const cv::Mat& getStencilImage() const;

	const cv::Mat& getStencil() const;

	const cv::Vec3b& getStencilColor() const;

	bool isEmpty() const;

	bool isFirstState() const;

	bool isLastState() const;

	void setImage(const cv::Mat& image);

	void addStencil(const cv::Point& point, int brushRadius, bool newState = true);

	void addStencil(const cv::Point& startPoint, const cv::Point& endPoint, int brushRadius, bool newState = true);

	void removeStencil(const cv::Point& point, int brushRadius, bool newState = true);

	void removeStencil(const cv::Point& startPoint, const cv::Point& endPoint, int brushRadius, bool newState = true);

	void setStencilColor(const cv::Vec3b& color);

	bool undo();

	bool redo();
};
