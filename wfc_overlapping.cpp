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

	void Init(int n, int m) override {
		Result.create(m, n);
		Model::Init(n, m);
	}

	void Show() override {
		if (field.size() == 0) {
			return;
		}

		for (int i = 0; i < static_cast<int>(field.size()); ++i) {
			for (int j = 0; j < static_cast<int>(field[0].size()); ++j) {
				int r = 0, g = 0, b = 0, contribs = 0;

				for (int x = 0; x < N; ++x) {
					if (j - x < 0) continue;

					for (int y = 0; y < N; ++y) {
						if (i - y < 0) continue;

						for (int t = 0; t < count; ++t) {
							if (!field[i - y][j - x][t]) continue;

							sf::Color pixel = Source.getPixel(tiles[t].first + x, tiles[t].second + y);
							r += pixel.r;
							g += pixel.g;
							b += pixel.b;
							++contribs;
						}
					}
				}

				Result.setPixel(j, i, sf::Color(r / contribs, g / contribs, b / contribs));
			}
		}
	}
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		clog << "Invalid arguments\n";
		return 1;
	}

	sf::Image image;
	if (!image.loadFromFile(string(argv[1]))) {
		return 1;
	}

	int N, n, m;
	cin >> N >> n >> m;

	OverlappingModel model(image, N);
	model.Init(n, m);

	int scale = 10;
	sf::Texture texture;
	texture.create(m, n);
	sf::Sprite sprite(texture);
	sprite.setScale(scale, scale);

	bool generated = false, autoStep = false, changed;

	sf::RenderWindow window(sf::VideoMode(m * scale, n * scale), "wfc");
	window.setFramerateLimit(60);
	while (window.isOpen()) {
		changed = false;
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();

			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G && !generated && !autoStep) {
				generated = !model.Step();
				changed = true;

			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
				generated = false;
				model.Clear();
				changed = true;

			} else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
				if (generated) {
					generated = false;
					model.Clear();
				}
				autoStep = true;
				changed = true;
			}
		}

		if (autoStep) {
			autoStep = model.Step();
			generated = !autoStep;
			changed = true;
		}
 
		window.clear();
		if (changed) {
			model.Show();
			texture.update(model.Result);
		}
		window.draw(sprite);
		window.display();
	}
}