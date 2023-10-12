// VergeProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

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

    std::vector<bool> visited(numNodes, false);

    // Build a map to keep track of node pairs
    std::unordered_map<int, int> mapNodePairs;
    for (int i = 0; i < numNodes; i += 2)
    {
        mapNodePairs[i] = i + 1;
        mapNodePairs[i + 1] = i;
    }

    typedef std::pair<double, std::pair<int, int>> weightNodes;
    typedef std::vector <weightNodes> vecWeightNodes;
    // Organize data by {weight, {toNode, fromNode}}
    // By default a pq will put the largest weight first (pair.first). Use std::greater to make
    // it by the lowest weight first (See cppreference.com page on priority queue)
    std::priority_queue<weightNodes, vecWeightNodes, std::greater<weightNodes>> pq2;

    // We can arbitrarily start from the first node pair (0 & 1). Figure out which is best
    // Assuming the weights will never be negative in this loop
    std::pair<double, std::pair<int, int>> startingNode{-1.0, { 0, 0 }};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            if (i == j) continue;
            if (mapNodePairs[i] == j) continue;
            double weight = distanceMatrix[i][j];

            if (startingNode.first < 0 || weight < startingNode.first)
            {
                startingNode.first = weight;
                startingNode.second.first = j;
                startingNode.second.second = i;
            }
        }
    }
    visited[0] = true;
    visited[1] = true;


    pq2.push(startingNode);

    // We get the current node we are at (top of list). If we've already been there then skip it.
    // Also if the node leading to our current node (fromNode) is not the last node we visited skip it
    // (we could get a spanning tree otherwise)
    // Then we add the weight to our path total and then add all other edge weights leading to
    // new nodes to our queue.
    int lastNode = startingNode.second.second;
    double pathWeight = 0;
    while (!pq2.empty())
    {
        auto curNodePair = pq2.top();
        pq2.pop();

        int curNode = curNodePair.second.first;
        int fromNode = curNodePair.second.second;
        double weight = curNodePair.first;

        if (visited[curNode] || visited[mapNodePairs[curNode]]) continue;
        if (fromNode != lastNode) continue;

        pathWeight += weight;
        visited[curNode] = true;
        visited[mapNodePairs[curNode]] = true;
        lastNode = curNode;

        for (int node = 0; node < numNodes; node++)
        {
            if (!visited[node] || !visited[mapNodePairs[node]])
            {
                pq2.push({ distanceMatrix[curNode][node], {node, curNode} });
            }
        }
    }

    return pathWeight;
}

// I will also try to do what I think should give an optimal solution using a bit mask.
// Idea is if we had 5 nodes, we could use 5 bits to track if each one has been visited. We can pair this with a weight,
// and each unique weight + bit mask route tells us a unique path around the graph. Once we have visited all nodes,
// we have the weight for that route and we should be able to find the smallest amongst all the routes.

// In this example we have an even number of nodes and nodes are in pairs. I'm not 100% sure yet but I think that means
// we would want to use a 5 bit bit mask if we had 10 nodes. Then when all bits are 1 we have visited each of the pairs.
// I think the unique weights should differentiate them.
// We need to avoid revisiting the same node as well

// I used two classes/structs for this nethod. Depending on the context of where/how small classes like this
// might be used, you could either make a simple struct where everything is public or a more standard class
// that properly utilizes encapsulation with private data and accessors. I've just shown both ways.

struct NodePath
{
    NodePath(int iCurNode, int iBitMask, double weight, int numVisited, const std::vector<int> & vPath = {}) :
        m_iCurNode{ iCurNode }, m_iBitMask{ iBitMask }, m_dWeight{ weight }, m_numVisited{ numVisited }, m_vPath{vPath}
    {
        m_vPath.push_back(iCurNode);
    }

    int m_iCurNode;
    int m_iBitMask;
    double m_dWeight;
    int m_numVisited;
    std::vector<int> m_vPath;
};

class VisitedPathWeights
{
public:
    VisitedPathWeights(int numNodes) : m_dPathWeight{ 0.0 }
    {
        for (int i = 0; i < numNodes; ++i)
        {
            m_vVis.push_back(false);
        }
    }

    double getPathWeight() { return m_dPathWeight; }
    bool isNodeVisited(int node) { return m_vVis[node]; }

    void setPathWeight(double dWeight) { m_dPathWeight = dWeight; }
    void setNodeVisited(int iNode, bool bIsVisit) { m_vVis[iNode] = bIsVisit; }

private:
    std::vector<bool> m_vVis;
    double m_dPathWeight;
};

double optimalMin(const std::vector<std::vector<double>>& distanceMatrix)
{
    int numNodes = static_cast<int>(distanceMatrix.size());

    if (numNodes % 2 != 0) return -2.0;

    // Vec to store all possible bitmask combinations. Store each one as a "VisitedPathWeight" so that
    // for each potential bitmask path, we can track the smallest weight used to reach it (since the
    // same bitmask can be hit by following nodes in different orders)
    int numTotalPaths = (1 << numNodes) - 1;
    std::vector<VisitedPathWeights> visited(numTotalPaths + 1, numNodes);

    // As previously, a map to track our node pairs
    std::unordered_map<int, int> mapNodePairs;
    for (int i = 0; i < numNodes; i += 2)
    {
        mapNodePairs[i] = i + 1;
        mapNodePairs[i + 1] = i;
    }

    // Since we have to check all possible paths, I don't believe using a priority queue gives any benefit
    // A standard FIFO queue allows for faster insertion.
    // Store a NodePath which allows us to access the current node at the end of our path, its bitmask,
    // the weight of said path, how many nodes we've visited, and full path itself in vector format
    // (We could access some of this info from the stored vector but the stored vector was added later on
    // and also it's simplier to access it from a stored variable
    std::queue<NodePath> queue;

    // Add all nodes to our queue as each could start the best path
    for (int i = 0; i < numNodes; i++)
    {
        int initialMask = 1 << i;
        queue.push({ i, initialMask, 0.0, 1 });
        visited[initialMask].setNodeVisited(i, true);
        visited[initialMask].setPathWeight(0.0);
    }

    // Assuming no negatives by using a negative weight as my default for min weight tracking
    double dMinWeight = -1;
    while (!queue.empty())
    {
        auto curNodePath = queue.front();
        queue.pop();

        int iCurNode = curNodePath.m_iCurNode;
        int iCurMask = curNodePath.m_iBitMask;
        double dWeight = curNodePath.m_dWeight;
        int iCurNumVisited = curNodePath.m_numVisited;

        if (iCurNumVisited == numNodes / 2)
        {
            if (dMinWeight < 0.0 || dWeight < dMinWeight)
            {
                dMinWeight = dWeight;
            }
        }

        for (int node = 0; node < numNodes; node++)
        {
            // The logic below took a lot of trial and error to work out:
            // - If the node we are looping over is already in the path, we ignore it. I'm using std::find for this over a stored path
            //   but there might be a way to do this using the bitmask and visited array. (I couldn't figure out how but it would save
            //   a few seconds (the path array is size numNodes/2 at most so its not too bad)
            // - If on our current path (checked using bitmask), we see that nodes pair has been visited, we ignore it. This
            //   for instance stops a path of 0 -> 2 -> 1 from happening (0 -> 2 has bitmask 5 and the 0th node will be visited already)
            // - Compute the new mask and new weight that this mask (path) will have
            // - If the new mask (path) has already exisited/been visited where the next element on the path is node, ignore it
            //   unless (OR) we have visited this path before but this "new" way of reaching this path (i.e. nodes in a different
            //   order) has a lower cost.
            // - All above satisied: we update the new path visited vecor, set its best weight, update num nodes visited and
            //   add it to the queue
            if (std::find(curNodePath.m_vPath.begin(), curNodePath.m_vPath.end(), node) != std::end(curNodePath.m_vPath)) continue;
            if (visited[iCurMask].isNodeVisited(mapNodePairs[node])) continue;

            int iNewMask = iCurMask | (1 << node);
            double dNewWeight = dWeight + distanceMatrix[iCurNode][node];
            if (!visited[iNewMask].isNodeVisited(node) || dNewWeight < visited[iNewMask].getPathWeight())
            {
                visited[iNewMask].setNodeVisited(node, true);
                visited[iNewMask].setPathWeight(dNewWeight);
                int newNumVisited = iCurNumVisited + 1;
                queue.push({ node, iNewMask, dNewWeight, newNumVisited, curNodePath.m_vPath });
            }
        }
    }

    return dMinWeight;
}

int main()
{
    std::vector<std::vector<double>> test0{
        { 0.0, 1.5, 2.7, 1.2 },
        { 1.5, 0.0, 4.6, 1.1 },
        { 2.7, 4.6, 0.0, 1.0 },
        { 1.2, 1.1, 1.0, 0.0 }, };

    std::vector<std::vector<double>> test{
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

    std::cout << minPath(test) << std::endl;
    std::cout << optimalMin(test) << std::endl;
}