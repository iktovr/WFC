#include <SFML/Graphics.hpp>
#include <set>
#include <string>
#include <iomanip>

#include "model.hpp"

int Scale = 10;

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

class OverlappingModel : public Model {
protected:
	vector<sf::Texture> tiles;
	int N;

	bool Neighbour(int i, int j, int d) {
		sf::Image image1 = tiles[i].copyToImage(), image2 = tiles[j].copyToImage();
		int xmin = (dx[d] <= 0) ? 0 : dx[d], xmax = (dx[d] >= 0) ? N : N + dx[d];
		int ymin = (dy[d] <= 0) ? 0 : dy[d], ymax = (dy[d] >= 0) ? N : N + dy[d];

		for (int x = xmin; x < xmax; ++x) {
			for (int y = ymin; y < ymax; ++y) {
				if (image1.getPixel(x, y) != image2.getPixel(x - dx[d], y - dy[d])) {
					return false;
				}
			}
		}
		return true;
	}

public:
	OverlappingModel(sf::Image &image, int N) : N(N) {
		map<sf::Image, int, ImageComparator> search;

		auto [width, height] = image.getSize();
		int k = 0;

		for (size_t i = 0; i < height - N; ++i) {
			for (size_t j = 0; j < width - N; ++j) {

				sf::Texture text;
				text.loadFromImage(image, sf::IntRect(j, i, N, N));
				sf::Image newImage = text.copyToImage();

				auto iter = search.find(newImage);
				if (iter == search.end()) {
					search.insert(make_pair(newImage, k));
					k += 1;
					tiles.push_back(text);
					probs.push_back(1);
				} else {
					++probs[iter->second];
				}
			}
		}

		int sum = (height - N) * (width - N);
		for (double &prob: probs) {
			prob /= sum;
		}

		count = tiles.size();
		rules.assign(count, vector(4, Domain(count, false)));

		for (int i = 0; i < count; ++i) {
			for (int d = 0; d < 4; ++d) {
				for (int j = 0; j < count; ++j) {
					if (Neighbour(i, j, d)) {
						rules[i][d].Set(j);
					}
				}
			}
		}
		cout << tiles.size() << '\n';

	}

	void Show() override {
		for (size_t i = 0; i < field.size(); ++i) {
			for (size_t j = 0; j < field[i].size(); ++j) {
				if (field[i][j].Count() > 0) {
					cout << setw(2) << field[i][j].Number() << " ";
				}
			}
			cout << '\n';
		}
	}

	void Show(sf::RenderWindow &window) {
		for (size_t i = 0; i < field.size(); ++i) {
			for (size_t j = 0; j < field[i].size(); ++j) {
				if (field[i][j].Count() > 0) {
					sf::Sprite sprite(tiles[field[i][j].Number()]);
					sprite.setScale(Scale, Scale);
					sprite.setPosition(j * Scale, i * Scale);
					window.draw(sprite);
				}
			}
		}
	}

};

int main(int, char *argv[]) {
	srand(time(nullptr));
	sf::Image image;
	if (!image.loadFromFile(string(argv[1]))) {
		cout << "Error" << endl;
		return 1;
	}

	int N, n, m;
	cin >> N >> n >> m;

	OverlappingModel model(image, N);
	model.Generate(n, m);

	sf::RenderWindow window(sf::VideoMode(m * Scale, n * Scale), "wfc");
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
				model.Generate(n, m);
			}
		}
 
		window.clear(sf::Color::Red);

		model.Show(window);

		window.display();
	}
}