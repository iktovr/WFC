#pragma once

#include <vector>
#include <iostream>
#include <cmath>

#include "random.hpp"

using namespace std;

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
			if (mask[i] && probs[i] > 0) {
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