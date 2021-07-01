#include <iostream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <utility>
#include <cmath>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <unordered_map>

using namespace std;

int Randrange(int b) {
	return rand() % b;
}

int Randrange(int a, int b) {
	return rand() % b + a;
}

int Randchoice(const vector<int> &nums, const vector<double> &probs) {
	vector<double> newProbs;
	
	double sum = 0;
	for (int i: nums) {
		sum += probs[i];
		newProbs.push_back(probs[i]);
	}
	newProbs[0] /= sum;
	for (size_t i = 1; i < newProbs.size(); ++i) {
		newProbs[i] = newProbs[i] / sum + newProbs[i - 1];
	}
	
	double choice = (double)rand() / RAND_MAX;
	size_t i = lower_bound(newProbs.begin(), newProbs.end(), choice) - newProbs.begin();
	return nums[i];
}

struct Domain {
    vector<bool> mask;

	Domain() {}
	Domain(int count, bool value = true) : mask(count, value) {}
	Domain(const vector<bool> &&other) : mask(other) {}
	Domain(const Domain &other) : mask(other.mask) {}

	size_t Size() const {
		return mask.size();
	}

	bool operator[](const size_t index) const {
		return mask[index];
	}

	int Count() const {
		int n = 0;
		for (const bool& b: mask) {
			n += (int)b;
		}
		return n;
	}

	void Set() {
		for (size_t i = 0; i < Size(); i++) {
			mask[i] = true;
		}
	}
	
	void Set(size_t i) {
		if (i < Size()) {
			mask[i] = true;
		}
	}
	
	void Reset() {
		for (size_t i = 0; i < Size(); i++) {
			mask[i] = false;
		}
	}
	
	void Reset(size_t i) {
		if (i < Size()) {
			mask[i] = false;
		}
	}
	
	int Number() const {
		for (size_t i = 0; i < Size(); i++) {
			if (mask[i]) return i;
		}
		return -1;
	}

	int Choice(const vector<double> &probs) {
		vector<int> nums(Count());
		int i = 0;
		for (size_t j = 0; j < Size(); j++) {
			if (mask[j]) {
				Reset(j);
				nums[i] = j;
				++i;
			}
		}
		i = Randchoice(nums, probs);
		Set(i);
		return i;
	}

	void operator&=(const Domain &other) {
		if (Size() != other.Size()) {
			return;
		}
		for (size_t i = 0; i < Size(); i++) {
			mask[i] = mask[i] && other[i];
		}
	}

	void operator|=(const Domain &other) {
		if (Size() != other.Size()) {
			return;
		}
		for (size_t i = 0; i < Size(); i++) {
			mask[i] = mask[i] || other[i];
		}
	}

	friend Domain operator&(const Domain &, const Domain &);

	// void operator=(const vector<int> &idx) {
	// 	Reset();
	// 	for (int i: idx) {
	// 		Set(i);
	// 	}
	// }

	bool operator==(const Domain &other) {
		return mask == other.mask;
	}

	bool operator!=(const Domain &other) {
		return !(mask == other.mask);
	}

	double Entropy(const vector<double> &probs) {
		double entropy = 0;
		for (size_t i = 0; i < mask.size(); ++i) {
			if (mask[i]) {
				entropy -= probs[i] * log(probs[i]);
			}
		}
		return entropy;
	}

	void Print(vector<char> &tiles) {
		for (size_t i = 0; i < Size(); ++i) {
			if (mask[i]) {
				cout << tiles[i];
			} else {
				cout << "_";
			}
		}
	}
};

Domain operator&(const Domain &lhs, const Domain &rhs) {
	Domain res(lhs);
	res &= rhs;
	return res;
}

const int dx[4] = {0, 1, 0, -1}; // вверх, вправо, вниз, влево
const int dy[4] = {-1, 0, 1, 0};

void ParseRules(vector<vector<int>> &sample, int count, vector<double> &probs, vector<vector<Domain>> &rules) {
	probs.assign(count, 0);
	rules.assign(count, vector<Domain>(4, Domain(count, false)));
	int allprobs = sample.size() * sample[0].size();
	for (size_t i = 0; i < sample.size(); ++i) {
		for (size_t j = 0; j < sample[i].size(); ++j) {
			probs[sample[i][j]] += 1;
			for (int d = 0; d < 4; ++d) {
				int i2 = i + dy[d], j2 = j + dx[d];
				if (i2 < sample.size() && i2 >= 0 && j2 < sample[0].size() && j2 >= 0) {					
					rules[sample[i][j]][d].Set(sample[i2][j2]);
				}
			}
		}
	}
	
	for (size_t i = 0; i < count; ++i) {
		probs[i] = probs[i] / allprobs;
	}
}

void Parse(string filename, vector<char> &tiles, int &count, vector<double> &probs, vector<vector<Domain>> &rules) {
	ifstream file(filename);
	int n, m, i = 0, j = 0;
	file >> n >> m;
	unordered_map<char, int> tilesId;
	vector<vector<int>> sample(n, vector<int>(m));
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
	count = tiles.size();
	ParseRules(sample, count, probs, rules);
}

/*
TODO:
x энтропия
x случайные числа
x правила (vector<array<4, Domain>>)
x основной цикл
- enum для индексов направлений, чтобы не путаться (?)
- симметрия
x парсинг данных из ввода
- ввод
- растянуть main по функциям (?)
- обработка ошибок
*/

int main() {
	srand(time(nullptr));

	int count;
	vector<char> tiles;
	vector<double> probs;
	vector<vector<Domain>> rules;
	Parse("SCL.config", tiles, count, probs, rules);
	
	int n, m;
    cin >> n >> m;
    vector<vector<Domain>> field(n, vector<Domain> (m, Domain(count)));
	vector<vector<bool>> visited(n, vector<bool>(m, false));

    int y = Randrange(n);
    int x = Randrange(m);
	
	queue<pair<int, int>> q, visit;
	bool changed = true;
	while (changed) {
		changed = false;
		field[y][x].Choice(probs);
		q.push({y, x});
		visit.push({y, x});
		double entropy = Domain(count).Entropy(probs);
		
		while (!q.empty()) {
			auto [i, j] = q.front();
			q.pop();
			if (visited[i][j]) {
				continue;
			}
			visited[i][j] = true;
			
			double newEntropy = field[i][j].Entropy(probs);
			if (newEntropy < entropy && field[i][j].Count() > 1) {
				entropy = newEntropy;
				x = j;
				y = i;
			}

			for (int d = 0; d < 4; ++d) {
				int i2 = i + dy[d], j2 = j + dx[d];
				if (i2 < n && i2 >= 0 && j2 < m && j2 >= 0 && !visited[i2][j2]) {
					bool propagated = false;
					// применение изменений
					Domain mask(count, false);
					for (int k = 0; k < count; ++k) {
						if (!field[i][j].mask[k]) {
							continue;
						}
						propagated = true;
						mask |= rules[k][d];
					}

					if (propagated) { // иначе пустая ячейка обнуляет соседей
						if ((field[i2][j2] & mask) != field[i2][j2]) {
							changed = true;
							field[i2][j2] &= mask;
							q.push({i2, j2});
							visit.push({i2, j2});
						}
					}
				}
			}

			// for (vector<Domain> &row: field) {
			// 	for (Domain &domain: row) {
			// 		domain.Print(tiles);
			// 		cout << " ";
			// 	}
			// 	cout << '\n';
			// }
			// sleep(1);
		}

		while(!visit.empty()) {
			auto [i, j] = visit.front();
			visited[i][j] = false;
			visit.pop();
		}

		entropy = Domain(count).Entropy(probs);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				double newEntropy = field[i][j].Entropy(probs);
				if (field[i][j].Count() > 1 && newEntropy < entropy) {
					entropy = newEntropy;
					y = i;
					x = j;
					changed = true;
				}
			}
		}

		// cout << "---\n";
	}

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