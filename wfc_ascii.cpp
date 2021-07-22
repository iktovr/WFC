#include "domain.hpp"
#include "model.hpp"

using namespace std;

class ASCIIModel : public Model {
protected:
	vector<char> tiles;

public:
	ASCIIModel(vector<vector<int>> &sample, vector<char> &tiles) : Model(sample, tiles.size()), tiles(tiles) {}

	void Show() override {
		for (vector<Domain> &row: field) {
			for (Domain &domain: row) {
				if (domain.Number() == -1) {
					cout << "_";
				} else if (domain.Count() > 1) {
					cout << "*";
				} else {
					cout << tiles[domain.Number()];
				}
			}
			cout << '\n';
		}
		cout << '\n';
	}
};

int main(int , char* argv[]) {
	ifstream file(argv[1]);
	int n, m, i = 0, j = 0;
	file >> n >> m;
	unordered_map<char, int> tilesId;
	vector<vector<int>> sample(n, vector<int>(m));
	vector<char> tiles;
	char c;
	while (file >> c) {
		auto iter = tilesId.find(c);
		if (iter == tilesId.end()) {
			tiles.push_back(c);
			tilesId[c] = tiles.size() - 1;
		}

		sample[i][j] = tilesId[c];
		++j;
		if (j == m) {
			j = 0;
			++i;
		}
	}

	ASCIIModel model(sample, tiles);
	cin >> n >> m;
	model.Generate(n, m);
	model.Show();
}