#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <utility>

#include "domain.hpp"
#include "random.hpp"

using namespace std;

const int dx[4] = {0, 1, 0, -1}; // вверх, вправо, вниз, влево
const int dy[4] = {-1, 0, 1, 0};

const int MAX_BACKTRACKING = 1e4;

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

	bool Generate(int n, int m) {
		int countError = 0;
		field.assign(n, vector<Domain> (m, Domain(count)));
		vector<vector<bool>> visited(n, vector<bool>(m, false));
		stack<Backup> backup;

		int y = Randrange(n);
		int x = Randrange(m);
		
		queue<pair<int, int>> q, visit;
		bool changed = true;
		while (changed) {
			changed = false;
			backup.push({field, y, x});
			backup.top().n = field[y][x].Choice(probs);
			q.push({y, x});
			visit.push({y, x});
			double entropy = Domain::MaxEntropy(probs) + 1;
			
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

						if (propagated && ((field[i2][j2] & mask) != field[i2][j2])) { // иначе пустая ячейка обнуляет соседей
							changed = true;
							field[i2][j2] &= mask;
							q.push({i2, j2});
							visit.push({i2, j2});

							while (field[i2][j2].Count() == 0) {
								++countError;
								backtracked = true;
								while (!q.empty()) {
									q.pop();
								}

								if (countError == MAX_BACKTRACKING) {
									field.assign(n, vector<Domain>(m, Domain(count)));
									visited.assign(n, vector<bool>(m, false));
									x = Randrange(m);
									y = Randrange(n);
									while (!visit.empty()) {
										visit.pop();
									}
									countError = 0;
									while (!backup.empty()) {
										backup.pop();
									}
									break;
								}

								if (backup.empty()) {
									return false;
								}
								field = backup.top().oldField;
								j2 = x = backup.top().j;
								i2 = y = backup.top().i;
								field[y][x].Reset(backup.top().n);
								backup.pop();
							}
						}
					}

					if (backtracked) {
						break;
					}
				}
			}

			while(!visit.empty()) {
				auto [i, j] = visit.front();
				visited[i][j] = false;
				visit.pop();
			}

			if (!changed) {
				entropy = Domain::MaxEntropy(probs) + 1;
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
			}
		}
		return true;
	}

	virtual void Show() = 0;
};