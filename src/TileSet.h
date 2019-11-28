#pragma once

#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>
#include <random>

class Tile
{
private:

	cv::Vec3b m_mean;       //!< mean color of the tile
	cv::Mat m_icon;         //!< icon of the tile to be displayed in GUI
	cv::Mat m_image;        //!< image of the tile
	std::string m_filePath; //!< path to the file with the tile image

public:

	Tile() = default;

	Tile(const std::string filePath, const cv::Mat& image, const cv::Mat& icon, const cv::Vec3b& mean);

	Tile(const Tile& other);

	Tile(Tile&& other) = default;

	Tile& operator=(const Tile& other);

	Tile& operator=(Tile&& other) = default;

	~Tile() = default;

	const std::string& getFilePath() const;

	const cv::Mat& getImage() const;

	const cv::Mat& getIcon() const;

	const cv::Vec3b& getMean() const;

	bool operator==(const Tile& rhs);

	bool operator!=(const Tile& rhs);
};

class TileSet
{
private:

	std::vector<Tile> m_tiles;
	static std::default_random_engine m_randomEngine;

public:

	bool addTile(const Tile& tile);

	void deleteTile(int index);

	void clear();

	const std::vector<Tile>& getTiles() const;

	const Tile& getClosestTile(const cv::Vec3b& color, int numNearest) const;

	bool isEmpty() const;

};
