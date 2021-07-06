#include <SFML/Graphics.hpp>
#include <set>
#include <string>
#include <iomanip>

#include "model.hpp"

class OverlappingModel : public Model {
protected:
	sf::Image Source;
	vector<pair<int, int>> tiles;
	int N;

	bool Neighbour(int i, int j, int d) {
		int xmin = (dx[d] <= 0) ? 0 : dx[d], xmax = (dx[d] >= 0) ? N : N + dx[d];
		int ymin = (dy[d] <= 0) ? 0 : dy[d], ymax = (dy[d] >= 0) ? N : N + dy[d];

		for (int x = xmin; x < xmax; ++x) {
			for (int y = ymin; y < ymax; ++y) {
				if (Source.getPixel(tiles[i].first + x, tiles[i].second + y) != Source.getPixel(tiles[j].first + x - dx[d], tiles[j].second + y - dy[d])) {
					return false;
				}
			}
		}
		return true;
	}

	bool TilesCompare(const pair<int, int> &a, const pair<int, int> &b) {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				sf::Color c1 = Source.getPixel(a.first + i, a.second + j);
				sf::Color c2 = Source.getPixel(b.first + i, b.second + j);

				if (c1.r != c2.r) {
					return c1.r < c2.r;
				}

				if (c1.g != c2.g) {
					return c1.g < c2.g;
				}

				if (c1.b != c2.b) {
					return c1.b < c2.b;
				}

				if (c1.a != c2.a) {
					return c1.a < c2.a;
				}
			}
		}
		return false;
	}

public:
	sf::Image Result;

	OverlappingModel(sf::Image &image, int N) : Source(image), N(N) {
		auto comparator = [this](const pair<int, int> &a, const pair<int, int> &b) { return TilesCompare(a, b); };
		map<pair<int, int>, int, decltype(comparator)> search(comparator);

		auto [width, height] = image.getSize();
		count = 0;

		for (size_t i = 0; i <= height - N; ++i) {
			for (size_t j = 0; j <= width - N; ++j) {
				pair<int, int> tile(j, i);
				auto iter = search.find(tile);
				if (iter == search.end()) {
					search.insert(make_pair(tile, count));
					++count;
					tiles.push_back(tile);
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

	bool Generate(int n, int m) {
		Result.create(m, n);
		return Model::Generate(n, m);
	}

	void Show() override {
		if (field.size() == 0) {
			return;
		}

		for (size_t i = 0; i < field.size(); ++i) {
			for (size_t j = 0; j < field[0].size(); ++j) {
				int tile = field[i][j].Number();
				if (tile != -1) {
					Result.setPixel(j, i, Source.getPixel(tiles[tile].first, tiles[tile].second));
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
	bool generated = model.Generate(n, m);

	int scale = 10;
	sf::Texture texture;
	texture.create(m, n);
	sf::Sprite sprite(texture);
	sprite.setScale(scale, scale);

	model.Show();
	texture.update(model.Result);
	bool regenerated = false;

	sf::RenderWindow window(sf::VideoMode(m * scale, n * scale), "wfc");
	sf::Clock timer;
	int fps = 60;
	while (window.isOpen()) {
		regenerated = false;
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
				generated = model.Generate(n, m);
				regenerated = true;
			}
		}
 
		window.clear(sf::Color::Red);
		if (generated) {
			if (regenerated) {
				model.Show();
				texture.update(model.Result);
			}
			window.draw(sprite);
		}
		window.display();

		int frameDuration = timer.getElapsedTime().asMilliseconds();
		int sleepTime = int(1000.f / fps) - frameDuration;
		if (sleepTime > 0) {
			sf::sleep(sf::milliseconds(sleepTime));
		}
		timer.restart();
	}
}