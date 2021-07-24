#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <queue>
#include <queue>
#include <utility>

#include "domain.hpp"
#include "random.hpp"

using namespace std;

const int dx[4] = {0, 1, 0, -1}; // вверх, вправо, вниз, влево
const int dy[4] = {-1, 0, 1, 0};

const int MAX_BACKTRACKING = 500;
const int MAX_BACKUP_SIZE = 100;

class Model {
protected:
	int count;
	vector<double> probs;
	vector<vector<Domain>> rules;
	vector<vector<Domain>> field;
	
	struct Backup {
		vector<vector<Domain>> oldField;
		int i;
		int j;
		int n;

		Backup(vector<vector<Domain>> &field, int i, int j) : oldField(field), i(i), j(j), n(-1) {}
	};

	vector<vector<bool>> visited;
	deque<Backup> backup;
	int x;
	int y;
	int n;
	int m;
	int countError;

	bool Observe() {
		// Поиск элемента с мин. энтропией если такой еще не найден или не подходит
		if (x == -1 && y == -1) {
			bool found = false;
			double entropy = Domain::MaxEntropy(probs) + 1;
			for (int i = 0; i < n; ++i) {
				for (int j = 0; j < m; ++j) {
					double newEntropy = field[i][j].Entropy(probs);
					if (field[i][j].Count() > 1 && newEntropy < entropy) {
						entropy = newEntropy;
						y = i;
						x = j;
						found = true;
					}
				}
			}

			if (!found) {
				return false;
			}
		}

		// Коллапс
		backup.push_back({field, y, x});
		backup.back().n = field[y][x].Choice(probs);
		if (backup.size() > MAX_BACKUP_SIZE) {
			backup.pop_front();
		}

		return true;
	}

	bool Propagate() {
		queue<pair<int, int>> q, visit;
		q.push({y, x});
		visit.push({y, x});
		double entropy = Domain::MaxEntropy(probs) + 1;
		x = y = -1;

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

			bool backtracked = false;
			for (int d = 0; d < 4; ++d) {
				int i2 = i + dy[d], j2 = j + dx[d];
				if (i2 < n && i2 >= 0 && j2 < m && j2 >= 0 && !visited[i2][j2]) {
					// Применение изменений
					Domain mask(count, false);
					for (int k = 0; k < count; ++k) {
						if (!field[i][j].mask[k]) {
							continue;
						}
						mask |= rules[k][d];
					}

					if ((field[i2][j2] & mask) != field[i2][j2]) {
						field[i2][j2] &= mask;
						q.push({i2, j2});
						visit.push({i2, j2});

						// Откат изменений
						while (field[i2][j2].Count() == 0) {
							++countError;

							if (countError > MAX_BACKTRACKING) {
								return false;
							}

							backtracked = true;
							while (!q.empty()) {
								q.pop();
							}

							if (backup.empty()) {
								return false;
							}

							field = backup.back().oldField;
							j2 = x = backup.back().j;
							i2 = y = backup.back().i;
							field[y][x].Reset(backup.back().n);
							backup.pop_back();
						}
					}
				}

				if (backtracked) {
					break;
				}
			}
		}

		while(!visit.empty()) {
			visited[visit.front().first][visit.front().second] = false;
			visit.pop();
		}

		return true;
	}

	Model() {}
	
public:
	Model(vector<vector<int>> &sample, int count) : count(count) {
		probs.assign(count, 0);
		rules.assign(count, vector<Domain>(4, Domain(count, false)));
		int n = sample.size(), m = sample[0].size();
		int allprobs = n * m;
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				probs[sample[i][j]] += 1;
				for (int d = 0; d < 4; ++d) {
					int i2 = i + dy[d], j2 = j + dx[d];
					if (i2 < n && i2 >= 0 && j2 < m && j2 >= 0) {					
						rules[sample[i][j]][d].Set(sample[i2][j2]);
					}
				}
			}
		}
		
		for (int i = 0; i < count; ++i) {
			probs[i] = probs[i] / allprobs;
		}
	}

	virtual void Init(int _n, int _m) {
		n = _n;
		m = _m;
		y = Randrange(n);
		x = Randrange(m);

		countError = 0;
		field.assign(n, vector<Domain> (m, Domain(count)));
		visited.assign(n, vector<bool>(m, false));
	}

	void Clear() {
		y = Randrange(n);
		x = Randrange(m);

		countError = 0;
		field.assign(n, vector<Domain> (m, Domain(count)));
		visited.assign(n, vector<bool>(m, false));
		backup.clear();
	}

	bool Step() {
		return Observe() && Propagate();
	}

	void Generate(int n, int m) {
		Init(n, m);
		while (Step());
	}

	virtual void Show() = 0;
};