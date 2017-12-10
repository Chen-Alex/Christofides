#include "random.h"

using namespace std;


void uniform_random(int n, string outfile) {
	ofstream file;
	file.open(outfile);
	file.precision(10);
	uniform_real_distribution<double> unif(0, 1000000000);
	default_random_engine rdm;
	for (int i = 0; i < n; i++) {
		double x = unif(rdm);
		double y = unif(rdm);
		file << i << " " << x << " " << y << endl;
	}
	file.close();
}


void close_to_origin(int n, string outfile) {
	ofstream file;
	file.open(outfile);
	normal_distribution<double> norm(0, 10000000);
	default_random_engine rdm;
	file.precision(10);
	for (int i = 0; i < n; i++) {
		double x = norm(rdm);
		double y = norm(rdm);
		file << i << " " << x << " " << y << endl;
	}
	file.close();
}


void close_to_hub(int n, string outfile) {
	ofstream file;
	file.open(outfile);
	file.precision(10);
	uniform_real_distribution<double> unif(-10000000, 10000000);
	normal_distribution<double> norm(0, 800000);
	default_random_engine rdm;
	vector<pair<double, double>> centers;
	for (int i = 0; i < 10; i++) {
		centers.push_back(make_pair(unif(rdm), unif(rdm)));
	}
	for (int i = 0; i < n; i++) {
		int j = rand() % 10;
		double x = norm(rdm) + centers[j].first;
		double y = norm(rdm) + centers[j].second;
		file << i << " " << x << " " << y << endl;
	}
	file.close();
}
