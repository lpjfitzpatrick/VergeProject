// VergeProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <queue>
#include <map>

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
double minPath(const std::vector<std::vector<double>>& distanceMatrix)
{
    int numNodes = static_cast<int>(distanceMatrix.size());

    // We could maybe assume this would never happen since the problem specifies that the graph
    // will always be complete with an even number of nodes, but it is quick and easy to check for
    // odd. We could also verify completeness by checking the size of each column to ensure we have
    // a square matrix but I'll leave it out
    if (numNodes % 2 != 0) return -1.0;

    if (numNodes == 0) return -2.0;

    double pathWeight = 0;
    std::vector<bool> visited(numNodes, false);

    // Build a map to keep track of node pairs
    std::map<int, int> mapNodePairs;
    for (int i = 0; i < numNodes; i += 2)
    {
        mapNodePairs[i] = i + 1;
        mapNodePairs[i + 1] = i;
    }

    typedef std::vector<std::pair<double, int>> vecPair;
    // Organize data by {weight, toNode}
    // By default a pq will put the largest weight first (pair.first). Use std::greater to make
    // it by the lowest weight first (See cppreference.com page on priority queue)
    std::priority_queue<std::pair<double, int>, vecPair, std::greater<std::pair<double, int>>> pq;

    // We can arbitrarily start from the first node pair (0 & 1). Figure out which is best
    double minWeight = -1;
    std::pair<double, int> startingNode{0.0, 0};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            if (i == j) continue;
            if (mapNodePairs[i] == j) continue;
            double weight = distanceMatrix[i][j];
            if (minWeight < 0 || weight < minWeight)
            {
                minWeight = weight;
                startingNode.second = i;
            }
        }
    }

    // Note the node we start from won't necessarily be the start of the list as the alg considers
    // past edge weights even after finding the best node to get to from the starting node.
    pq.push(startingNode);

    // We get the current node we are at (top of list). If we've already been there then skip it.
    // Else we add the weight to our path total and then add all other edge weights leading to
    // new nodes to our queue.
    while (!pq.empty())
    {
        auto curNodePair = pq.top();
        pq.pop();

        int curNode = curNodePair.second;
        double weight = curNodePair.first;

        if (visited[curNode] || visited[mapNodePairs[curNode]]) continue;

        pathWeight += weight;
        visited[curNode] = true;
        visited[mapNodePairs[curNode]] = true;

        for (int node = 0; node < numNodes; node++)
        {
            if (!visited[node] || !visited[mapNodePairs[node]])
            {
                pq.push({ distanceMatrix[curNode][node], node });
            }
        }
    }

    return pathWeight;
}

int main()
{
    std::vector<std::vector<double>> test{
        { 0.0, 1.5, 2.7, 1.2 },
        { 1.5, 0.0, 4.6, 1.1 },
        { 2.7, 4.6, 0.0, 1.0 },
        { 1.2, 1.1, 1.0, 0.0 }, };

    std::vector<std::vector<double>> test2{
        { 0.0, 8.1, 9.2, 7.7, 9.3, 2.3, 5.1, 10.2, 6.1, 7.0},
        { 8.1, 0.0, 12.0, 0.9, 12.0, 9.5, 10.1, 12.8, 2.0, 1.0 },
        { 9.2, 12.0, 0.0, 11.2, 0.7, 11.1, 8.1, 1.1, 10.5, 11.5 },
        { 7.7, 0.9, 11.2, 0.0, 11.2, 9.2, 9.5, 12.0, 1.6, 1.1 },
        { 9.3, 12.0, 0.7, 11.2, 0.0, 11.2, 8.5, 1.0, 10.6, 11.6 },
        { 2.3, 9.5, 11.1, 9.2, 11.2, 0.0, 5.6, 12.1, 7.7, 8.5 },
        { 5.1, 10.1, 8.1, 9.5, 8.5, 5.6, 0.0, 9.1, 8.3, 9.3 },
        { 10.2, 12.8, 1.1, 12.0, 1.0, 12.1, 9.1, 0.0, 11.4, 12.4 },
        { 6.1, 2.0, 10.5, 1.6, 10.6, 7.7, 8.3, 11.4, 0.0, 1.1 },
        { 7.0, 1.0, 11.5, 1.1, 11.6, 8.5, 9.3, 12.4, 1.1, 0.0 } };

    std::cout << minPath(test2) << std::endl;
}