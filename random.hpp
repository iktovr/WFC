#pragma once

#include <random>
#include <vector>
#include <algorithm>

using namespace std;

int Randrange(int b) {
	static random_device r;
	static mt19937 gen(r());
	return gen() % b;
}

int Randrange(int a, int b) {
	return Randrange(b) + a;
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