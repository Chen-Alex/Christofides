#ifndef _RANDOM
#define _RANDOM


#include "graph.h"

// Generate a uniformly random test case for Christofides' algorithm
void uniform_random(int n, string outfile);

// Generate a test case where the points are centered near (0, 0)
void close_to_origin(int n, string outfile);

// Generate a test case where the points are located near one of 10 "centers," chosen randomly
void close_to_hub(int n, string outfile);

#endif