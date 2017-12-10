#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include "graph.h"
#include "edge.h"
#include "compareedge.h"
#include <set>
#include <math.h>
#include <sstream>

using namespace std;

// Generate a uniformly random test case for Christofides' algorithm
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

// Generate a test case where the points are centered near (0, 0)
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

// Generate a test case where the points are located near one of 10 "centers," chosen randomly
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

// Generate a test case
/**
* Reads in a file in specified format and returns the resulting graph
* **/
Graph ReadFile(string filename) {
	vector<Edge> edges;
	string line;

	ifstream infile;
	infile.open(filename);

	if (infile.is_open()) {
		while (getline(infile, line)) {
			int a, b, c;
			stringstream ss(line);
			ss >> a >> b >> c;
			edges.push_back(Edge(a, b, c));
		}
		infile.close();
	}
	else {
		cout << "Unable to read file";
	}
	cout << "--------" << endl;
	return Graph(edges);
}

/**
* Calculates the Euclidian distance between two points
* */
double eucDist(pair<double, double> a, pair<double, double> b) {
	if (a == b) {
		return 0.0;
	}
	double dist = sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
	return dist;
}
/**
* Read file metric.
* Given a list of 2d points
* Construct a graph from this
* */
Graph ReadFileMetric(string filename) {
	vector<pair<double, double>> points;
	vector<Edge> edges;
	set<pair<int, int>> visited;
	string line;

	ifstream infile;
	infile.open(filename);

	if (infile.is_open()) {
		while (getline(infile, line)) {
			int a;
			long double b, c;
			stringstream ss(line);
			ss >> a >> b >> c;
			points.push_back(make_pair(b, c));
		}
		infile.close();
	}
	else {
		cout << "Unable to read file";
	}


	cout << "------" << endl;
	for (int i = 0; i < points.size(); i++) {
		pair<double, double> a = points[i];
		for (int j = 0; j < points.size(); j++) {
			pair<double, double> b = points[j];
			double dist = eucDist(a, b);
			if ((dist == 0.0) || (visited.count(make_pair(i, j)) || visited.count(make_pair(j, i)))) {
				continue;
			}
			else {
				visited.insert(make_pair(i, j));
				visited.insert(make_pair(j, i));
				edges.push_back(Edge(i, j, dist));
			}
		}
	}
	return Graph(edges);

}

int main() {
	close_to_hub(100, "out.txt");
	Graph g(ReadFileMetric("out.txt"));
	return 0;
}