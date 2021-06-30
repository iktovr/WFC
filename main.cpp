#include <iostream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <utility>
#include <cmath>
#include <algorithm>

using namespace std;

int Randrange(int b) {
	return rand() % b;
}

int Randrange(int a, int b) {
	return rand() % b + a;
}

int Randchoice(vector<int> &nums, vector<double> &probs) {
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

	Domain(int count) : mask(count, true) {}

	size_t Size() const {
		return mask.size();
	}

	bool operator[](const size_t index) {
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

	int Choice(vector<double> &probs) {
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
		Set(nums[i]);
		return nums[i];
	}

	void operator&=(Domain &other) {
		for (size_t i = 0; i < Size(); i++) {
			mask[i] = mask[i] && other[i];
		}
	}

	void operator|=(Domain &other) {
		for (size_t i = 0; i < Size(); i++) {
			mask[i] = mask[i] || other[i];
		}
	}

	void operator=(vector<int> &idx) {
		Reset();
		for (int i: idx) {
			Set(i);
		}
	}

	bool operator==(Domain &other) {
		return mask == other.mask;
	}

	double Entropy(vector<double> &probs) {
		double entropy = 0;
		for (size_t i = 0; i < mask.size(); ++i) {
			if (mask[i]) {
				entropy -= probs[i] * log(probs[i]);
			}
		}
		return entropy;
	}
};

class Rule {
    
};

/*
TODO:
x энтропия
x случайные числа
- правила
x основной цикл
(обработка ошибок)
*/

int main() {
    int n, m, count;
    cin >> n >> m >> count;
	vector<double> probs(count, 1.0 / count);
    vector<vector<Domain>> field(n, vector<Domain> (m, Domain(count)));
    int y = Randrange(n);
    int x = Randrange(m);
	
	int dx[4] = {0, 1, 0, -1}; // вверх, вправо, вниз, влево
	int dy[4] = {1, 0, -1, 0}; 
	queue<pair<int, int>> q;
	bool change = true;
	while (change) {
		change = false;
		field[y][x].Choice(probs);
		q.push({y, x});
		double entropy = Domain(count).Entropy(probs);
		
		while (!q.empty()) {
			auto [i, j] = q.front();
			q.pop();
			
			double newEntropy= field[i][j].Entropy(probs);
			if (newEntropy < entropy) {
				entropy = newEntropy;
				x = j;
				y = i;
			}

			for (int d = 0; d < 4; ++d) {
				if (i + dy[d] < n && i + dy[d] > 0 && j + dx[d] < m && j + dx[d] > 0) {
					change = true;
					// применение изменений
					q.push({i + dy[d], j + dx[d]});
				}
			}
		}
	}
	
}