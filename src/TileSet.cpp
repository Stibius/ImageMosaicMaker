#include "TileSet.h"
#include "ImageLoader.h"
#include "Utility.h"

#include <algorithm>
#include <chrono>

std::default_random_engine TileSet::m_randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

Tile::Tile(const std::string filePath, const cv::Mat& image, const cv::Mat& icon, const cv::Vec3b& mean)
{
	m_filePath = filePath;
	m_image = image.clone();
	m_icon = icon.clone();
	m_mean = mean;
}

Tile::Tile(const Tile& other)
	: m_mean(other.m_mean)
	, m_image(other.m_image.clone())
	, m_icon(other.m_icon.clone())
	, m_filePath(other.m_filePath)
{
}

Tile& Tile::operator=(const Tile& other)
{
	if (this == &other)
	{
		return *this;
	}

	m_mean = other.m_mean;
	m_image = other.m_image.clone();
	m_icon = other.m_icon.clone();
	m_filePath = other.m_filePath;

	return *this;
}

const std::string& Tile::getFilePath() const
{
	return m_filePath;
}

const cv::Mat& Tile::getImage() const
{
	return m_image;
}

const cv::Mat& Tile::getIcon() const
{
	return m_icon;
}

const cv::Vec3b& Tile::getMean() const
{
	return m_mean;
}

bool Tile::operator==(const Tile& rhs)
{
	return m_filePath == rhs.m_filePath;
}

bool Tile::operator!=(const Tile& rhs)
{
	return m_filePath != rhs.m_filePath;
}

bool TileSet::addTile(const Tile& tile)
{
	if (std::find(m_tiles.begin(), m_tiles.end(), tile) == m_tiles.end())
	{
		m_tiles.push_back(tile);

		return true;
	}
	else
	{
		return false;
	}
}

void TileSet::deleteTile(int index) 
{ 
	m_tiles.erase(m_tiles.begin() + index); 
}

void TileSet::clear() 
{ 
	m_tiles.clear(); 
}

const std::vector<Tile>& TileSet::getTiles() const 
{ 
	return m_tiles; 
}

const Tile& TileSet::getClosestTile(const cv::Vec3b& color, int numNearest) const
{
	std::vector<std::pair<const Tile*, int>> candidates;

	for (const Tile& tile : m_tiles)
	{
		int distance = Utility::distance(tile.getMean(), color);

		auto it = std::find_if(candidates.begin(), candidates.end(), [distance](const std::pair<const Tile*, int>& candidate)
		{
			return distance < candidate.second;
		});

		if (it != candidates.end())
		{
			it->first = &tile;
			it->second = distance;
		}
		else if (candidates.size() < numNearest)
		{
			candidates.insert(it, std::make_pair(&tile, distance));
		}
	}

	std::uniform_int_distribution<int> distribution(0, candidates.size() - 1);
	int index = distribution(m_randomEngine);

	return *candidates[index].first;
}

bool TileSet::isEmpty() const 
{ 
	return m_tiles.empty(); 
}
