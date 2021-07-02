#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <queue>
#include <utility>

#include "domain.hpp"
#include "random.hpp"

using namespace std;

const int dx[4] = {0, 1, 0, -1}; // вверх, вправо, вниз, влево
const int dy[4] = {-1, 0, 1, 0};

class Model {
protected:
	int count;
	vector<double> probs;
	vector<vector<Domain>> rules;
	vector<vector<Domain>> field;
	
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

	void Generate(int n, int m) {
		field.assign(n, vector<Domain> (m, Domain(count)));
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
			double entropy = Domain(count).Entropy(probs) + 1;
			
			while (!q.empty()) {
				auto [i, j] = q.front();
				// cout << i << " " << j << '\n';
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

			if (!changed) {			
				entropy = Domain(count).Entropy(probs) + 1;
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

			// cout << "---\n";
		}
	}

	virtual void Show() = 0;
};