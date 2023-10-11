// VergeProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

// This below way will not always give the best solution, but I believe it should ways get a
// feasible one. To get the best solution maybe we have to use a bitmask. I might look into that
// after I get the first way working

// Another observation is this matrix is symmetric along the diagonal. That could allow for some optimizations
// but I will not focus on that for getting a working solution.

// One way is to use a BFS and greedily choose the best (smallest weight) edge from a starting node
// The starting node can be any node on the graph since we know the graph is complete (and
// will always be able to get to every node once without backtracking). But since we are considering
// node in pairs, we can pick any pair and then pick the best edge from edges in that pair. The
// node which has that edge will be our start.
// We keep track of what has been visisted using a vector of bools. Visited one entry in a pair
// will visited both.
// We can use a priority queue to keep the smallest weight edge at the front of our queue. This
// should give constant access time and then we can take that edge if it leads somewhere that hasn't
// been visited yet.
float minPath(const std::vector<std::vector<float>>& distanceMatrix)
{
    int numNodes = distanceMatrix.size();

    // We could maybe assume this would never happen since the problem specifies that the graph
    // will always be complete with an even number of nodes, but it is quick and easy to check for
    // odd. We could also verify completeness by checking the size of each column to ensure we have
    // a square matrix but I'll leave it out
    if (numNodes % 2 != 0) return -1;

    return 1;
}

int main()
{
    std::vector<std::vector<float>> test(4, std::vector<float>(4, 0));

    minPath(test);
}