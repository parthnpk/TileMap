// TileMap.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include "noiseutils.h"
#include <noise/noise.h>
#include <SFML/Graphics.hpp>

using namespace noise;

class TileMap : public sf::Drawable, public sf::Transformable {
public:

	bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height){
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j){
				// get the current tile number
				int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}

		return true;
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};

void generateRandomSeed(utils::NoiseMap &heightMap, TileMap &tilemap) {

	int seed = rand(); //get a random number to set it as the seed no
	//create a Perlin module
	module::Perlin myModule;
	myModule.SetSeed(seed);

	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(myModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(64, 64);
	heightMapBuilder.SetBounds(0, 2, 0, 2);
	heightMapBuilder.Build();

	int level[4096];
	int arrayIndex = 0;

	for (size_t i = 0; i < 64; i++)
	{
		for (size_t j = 0; j < 64; j++)
		{
			if (/*(heightMap.GetValue(i, j)>= -1) &&*/ (heightMap.GetValue(i, j) <= -0.3))
			{
				level[arrayIndex] = 7; //Water 1
			}
			else if ((heightMap.GetValue(i, j)> -0.3) && (heightMap.GetValue(i, j) <= -0.2))
			{
				level[arrayIndex] = 8; //Water 2
			}
			else if ((heightMap.GetValue(i, j)> -0.2) && (heightMap.GetValue(i, j) <=-0.1))
			{
				level[arrayIndex] = 9; //Desert 1
			}
			else if ((heightMap.GetValue(i, j)> -0.1) && (heightMap.GetValue(i, j) <= -0.05))
			{
				level[arrayIndex] = 10; //Desert 2
			}
			else if ((heightMap.GetValue(i, j)> -0.05) && (heightMap.GetValue(i, j) <= 0))
			{
				level[arrayIndex] = 12; //Desert 3
			}
			else if ((heightMap.GetValue(i, j)> 0) && (heightMap.GetValue(i, j) <= 0.2))
			{
				level[arrayIndex] = 0; //Forest 1
			}
			else if ((heightMap.GetValue(i, j)> 0.2) && (heightMap.GetValue(i, j) <= 0.25))
			{
				level[arrayIndex] = 1; //Forest 2
			}
			else if ((heightMap.GetValue(i, j)> 0.25) && (heightMap.GetValue(i, j) <= 0.30))
			{
				level[arrayIndex] = 2; //Forest 3
			}
			else if ((heightMap.GetValue(i, j)> 0.30) && (heightMap.GetValue(i, j) <= 0.50))
			{
				level[arrayIndex] = 13; //Deep Forest 1
			}
			else if ((heightMap.GetValue(i, j)> 0.50) && (heightMap.GetValue(i, j) <= 0.55))
			{
				level[arrayIndex] = 14; //Deep Forest 2
			}
			else if ((heightMap.GetValue(i, j)> 0.55) && (heightMap.GetValue(i, j) <= 0.60))
			{
				level[arrayIndex] = 15; //Deep Forest 3
			}
			else if ((heightMap.GetValue(i, j)> 0.6) && (heightMap.GetValue(i, j) <= 0.80))
			{
				level[arrayIndex] = 4; //Snow 1
			}
			else if ((heightMap.GetValue(i, j)> 0.80) && (heightMap.GetValue(i, j) <= 0.85))
			{
				level[arrayIndex] = 5; //Snow 2
			}
			else if ((heightMap.GetValue(i, j)> 0.85) /*&& (heightMap.GetValue(i, j) <= 1)*/)
			{
				level[arrayIndex] = 6; //Snow 3
			}

			std::cout.precision(1);
			std::cout << level[arrayIndex] << "\t";
			arrayIndex++;
		}
		std::cout << std::endl;
	}

	if (!tilemap.load("TileSet.png", sf::Vector2u(16, 16), level, 64, 64))
		std::cout << "There was some problem loading the tileset!" << std::endl;;
}

int main() {

	sf::RenderWindow window(sf::VideoMode(512, 512), "Tile Map");

	srand(time(NULL));

	TileMap myMap;

	utils::NoiseMap heightMap;
	generateRandomSeed(heightMap, myMap);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::E) {
					generateRandomSeed(heightMap, myMap);
				}
			}
		}

		window.clear();
		window.draw(myMap);
		window.display();
	}
	return 0;
}