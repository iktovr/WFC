#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <cstring>

#include "model.hpp"

namespace fs = std::filesystem;

class TilesModel : public Model {
protected:
	vector<sf::Texture> tiles;
	int tilesSize;

public:
	TilesModel(vector<vector<int>> &sample, vector<sf::Texture> &tiles, int tilesSize) : 
		Model(sample, tiles.size()), tiles(tiles), tilesSize(tilesSize) {}

	void Show() override {}

	void Show(sf::RenderWindow &window) {
		for (size_t i = 0; i < field.size(); ++i) {
			for (size_t j = 0; j < field[i].size(); ++j) {
				if (field[i][j].Count() > 0) {
					sf::Sprite sprite(tiles[field[i][j].Number()]);
					sprite.setPosition(j * tilesSize, i * tilesSize);
					window.draw(sprite);
				}
			}
		}
	}
};

struct ImageComparator {
	bool operator()(const sf::Image & l_image, const sf::Image & r_image) const { 
		auto [width1, height1] = l_image.getSize();
		auto [width2, height2] = r_image.getSize();

		if (width1 != width2) {
			return width1 < width2;
		}

		if (height1 != height2) {
			return height1 < height2;
		}

		for (size_t i = 0; i < width1; i += 1) {
			for (size_t j = 0; j < height1; j += 1) {
				sf::Color c1;
				sf::Color c2;

				c1 = l_image.getPixel(i, j);
				c2 = r_image.getPixel(i, j);

				if (c1.a != c2.a) {
					return c1.a < c2.a;
				}

				if (c1.b != c2.b) {
					return c1.b < c2.b;
				}
				if (c1.g != c2.g) {
					return c1.g < c2.g;
				}

				if (c1.r != c2.r) {
					return c1.r < c2.r;
				}
			}
		}
		return false;
	}
};

vector<vector<int>> ParseImage(sf::Image image, int tilesSize, vector<sf::Texture> &tiles) {
	map<sf::Image, int, ImageComparator> search;

	auto [width, height] = image.getSize();

	vector<vector<int>> sample(width / tilesSize, vector<int>(height / tilesSize));

	int k = 0;

	for (size_t i = 0, offset_y = 0; i < sample.size(); ++i, offset_y += tilesSize) {
		for (size_t j = 0, offset_x = 0; j < sample[i].size(); ++j, offset_x += tilesSize) {

			sf::Texture text;
			text.loadFromImage(image, sf::IntRect(offset_x, offset_y, tilesSize, tilesSize));
			sf::Image newImage = text.copyToImage();

			auto iter = search.find(newImage);

			if (iter == search.end()) {
				search.insert(make_pair(newImage, k));
				sample[i][j] = k;
				k += 1;
				tiles.push_back(text);
			} else {
				sample[i][j] = iter->second;
			}
		}
	}
	return sample;
}


int main(int argc, char *argv[]) {
	srand(time(nullptr));

	if (argc < 2) {
		clog << "Invalid arguments\n";
		return 1;
	}

	vector<vector<int>> sample;
	vector<sf::Texture> tiles;
	int tilesSize, n, m;

	if (strcmp(argv[1], "rules") == 0) {
		fs::path dir = fs::path(argv[2]).parent_path();
		ifstream file(argv[2]);
		int count;
		file >> count >> tilesSize;
		tiles.assign(count, sf::Texture());
		string filename;
		for (int i = 0; i < count; ++i) {
			file >> filename;
			tiles[i].loadFromFile((dir / filename).string(), sf::IntRect(0, 0, tilesSize, tilesSize));
		}

		int n, m;
		file >> n >> m;
		sample.assign(n, vector<int>(m));
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				file >> sample[i][j];
			}
		}
	} else if (strcmp(argv[1], "image") == 0) {
		cin >> tilesSize;
		sf::Image image;
		if (!image.loadFromFile(argv[2])) {
			return 1;
		}
		sample = ParseImage(image, tilesSize, tiles);
	} else {
		clog << "Invalid argument\n";
		return 1;
	}

	TilesModel model(sample, tiles, tilesSize);
	cin >> n >> m;
	model.Generate(n, m);

	sf::RenderWindow window(sf::VideoMode(tilesSize * m, tilesSize * n), "Demo");
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
				model.Generate(n, m);
			}
		}
		window.clear();
		model.Show(window);
		window.display();
	}
}
