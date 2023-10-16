#include "pch.h"
#include "CppUnitTest.h"

#include "../VergeProject/VergeProject.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:
		double thresh = 0.00001;
		TEST_METHOD(SimpleFourByFour)
		{
			// Bost basic case that involes picking one edge to get from the {0,1} pair to the {2,3} pair
			std::vector<std::vector<double>> test{
			{ 0.0, 1.5, 2.7, 1.2 },
			{ 1.5, 0.0, 4.6, 1.1 },
			{ 2.7, 4.6, 0.0, 1.0 },
			{ 1.2, 1.1, 1.0, 0.0 }, };

			double min = minPath(test);
			auto opt = optimalMin(test);

			Assert::AreEqual(min, 1.1);
			Assert::AreEqual(opt.first, 1.1);

			std::vector<int> expected = { 1, 3 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {1, 3}");
		}

		TEST_METHOD(AllZeros)
		{
			std::vector<std::vector<double>> test{
				{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
				{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, 
				{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, 
				{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, 
				{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, 
				{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, };

			double min = minPath(test);
			auto opt = optimalMin(test);

			Assert::AreEqual(min, 0.0);
			Assert::AreEqual(opt.first, 0.0);

			std::vector<int> expected = { 0, 2, 4 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {0, 2, 4}");
		}

		TEST_METHOD(TwoByTwo)
		{
			// Should return 0 as we start in the {0,1} pair and don't go anywhere
			std::vector<std::vector<double>> test{
				{ 0.0, 1.5, },
				{ 1.5, 0.0, }, };

			double min = minPath(test);
			auto opt = optimalMin(test);

			Assert::AreEqual(min, 0.0);
			Assert::AreEqual(opt.first, 0.0);

			std::vector<int> expected = { 0 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {0}");
		}

		TEST_METHOD(ZeroNodes)
		{
			// Invalid. minPath returns -2.0 as an arbitrary error code
			// optimalMin will return the default min weight which is -1.0
			// These could of course be changed to have the same return value for consistent error return values.
			// Especially if these were both production level functions where logging error values matter for tracking bugs etc
			std::vector<std::vector<double>> test{ };

			double min = minPath(test);
			auto opt = optimalMin(test);

			Assert::AreEqual(min, -2.0);
			Assert::AreEqual(opt.first, -1.0);

			std::vector<int> expected = { };
			Assert::IsTrue(opt.second == expected, L"Expected result is { }");
		}

		TEST_METHOD(OddNumberOfNodes)
		{
			// Invalid. minPath returns -1.0 as an arbitrary error code
			// optimalMin returns -2.0 instead (mainly because the default min is -1.0 and so we should
			// be able to differentiate between invalid states based on return value).
			std::vector<std::vector<double>> test{
				{ 0.0 } };

			double min = minPath(test);
			auto opt = optimalMin(test);

			Assert::AreEqual(min, -1.0);
			Assert::AreEqual(opt.first, -2.0);

			std::vector<int> expected = { };
			Assert::IsTrue(opt.second == expected, L"Expected result is { }");
		}

		TEST_METHOD(SixBySix)
		{
			// 6x6 with optimal path (2, 0, 4) for a weight of 1.5
			// This test was used to debug the optimalMin alg not pursuing the (2, 0, 4) path because the bitmask
			// for that path is 21 which would have already been seen before in the (0, 2, 4) path. Due to the
			// visited logic we ended up ignoring 2,0,4 despite it having a better weight of 1.5 compared to 2.0
			// This is also another example where greedy fails because it follows a path starting from 0
			std::vector<std::vector<double>> test{
				{ 0.0, 2.0, 1.0, 2.0, 0.5, 2.0 },
				{ 2.0, 0.0, 2.0, 2.0, 2.0, 2.0 },
				{ 1.0, 2.0, 0.0, 2.0, 1.5, 2.0 },
				{ 2.0, 2.0, 2.0, 0.0, 2.0, 2.0 },
				{ 0.5, 2.0, 1.5, 2.0, 0.0, 2.0 },
				{ 2.0, 2.0, 2.0, 2.0, 2.0, 0.0 }, };

			double min = minPath(test);
			auto opt = optimalMin(test);

			Assert::AreEqual(min, 2.0);
			Assert::AreEqual(opt.first, 1.5);

			std::vector<int> expected = { 2, 0, 4 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {2, 0, 4}");
		}

		TEST_METHOD(TenByTen)
		{
			// Semi large 10x10 graph. Pairs are {0,1} {2,3} {4,5} {6,7} {8,9}
			// minPath won't find the optimal route but it will find a feasible one (1, 3, 9, 5, 6)
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

			double min = minPath(test);

			Assert::IsTrue(std::abs(min - 16.1) < thresh);
		}

		TEST_METHOD(OptimalTenByTen)
		{
			// Semi large 10x10 graph. Pairs are {0,1} {2,3} {4,5} {6,7} {8,9}
			// optimalMin should (I think) find the optimal route of (1, 3, 8, 4, 7)
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

			auto opt = optimalMin(test);

			Assert::IsTrue(std::abs(opt.first - 14.1) < thresh);

			std::vector<int> expected = { 1, 3, 8, 4, 7 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {1, 3, 8, 4, 7}");
		}

		TEST_METHOD(ModifiedTenByTen)
		{
			// Modified 10x10 from the last test: (1,5) and (5,1) are changed from 9.5 -> 1.2
			// This configuration would produce a spanning tree if the minPath algorithm did not keep track of the last
			// visited node and where our current node has come from (which would give a different total weight)
			// We expect to get the same total weight as in the last 10x10 minPath test since we still start from node 1 and
			// heads towards node 3 (since it has 0.9 weight) which means we should ignore he (1,5) edge with 1.2 weight
			std::vector<std::vector<double>> test{
				{ 0.0, 8.1, 9.2, 7.7, 9.3, 2.3, 5.1, 10.2, 6.1, 7.0},
				{ 8.1, 0.0, 12.0, 0.9, 12.0, 1.2, 10.1, 12.8, 2.0, 1.0 },
				{ 9.2, 12.0, 0.0, 11.2, 0.7, 11.1, 8.1, 1.1, 10.5, 11.5 },
				{ 7.7, 0.9, 11.2, 0.0, 11.2, 9.2, 9.5, 12.0, 1.6, 1.1 },
				{ 9.3, 12.0, 0.7, 11.2, 0.0, 11.2, 8.5, 1.0, 10.6, 11.6 },
				{ 2.3, 1.2, 11.1, 9.2, 11.2, 0.0, 5.6, 12.1, 7.7, 8.5 },
				{ 5.1, 10.1, 8.1, 9.5, 8.5, 5.6, 0.0, 9.1, 8.3, 9.3 },
				{ 10.2, 12.8, 1.1, 12.0, 1.0, 12.1, 9.1, 0.0, 11.4, 12.4 },
				{ 6.1, 2.0, 10.5, 1.6, 10.6, 7.7, 8.3, 11.4, 0.0, 1.1 },
				{ 7.0, 1.0, 11.5, 1.1, 11.6, 8.5, 9.3, 12.4, 1.1, 0.0 } };

			double min = minPath(test);

			Assert::IsTrue(std::abs(min - 16.1) < thresh);
		}

		TEST_METHOD(OptimalModifiedTenByTen)
		{
			// Same modified 10x10 that could produce a spanning tree using a "careless" version of minPath
			// Despite the large decrease in edge weight, minPath still found the same route as it greedily
			// took a path which ignored this new edge.
			// optimalMin is not so careless and is able to use this new edge to make a large improvement (14.1 to 8.8)
			// The path is (6, 5, 1, 3, 9) (using the 5-1 edge is a good sign that the alg is pretty solid)
			std::vector<std::vector<double>> test{
				{ 0.0, 8.1, 9.2, 7.7, 9.3, 2.3, 5.1, 10.2, 6.1, 7.0},
				{ 8.1, 0.0, 12.0, 0.9, 12.0, 1.2, 10.1, 12.8, 2.0, 1.0 },
				{ 9.2, 12.0, 0.0, 11.2, 0.7, 11.1, 8.1, 1.1, 10.5, 11.5 },
				{ 7.7, 0.9, 11.2, 0.0, 11.2, 9.2, 9.5, 12.0, 1.6, 1.1 },
				{ 9.3, 12.0, 0.7, 11.2, 0.0, 11.2, 8.5, 1.0, 10.6, 11.6 },
				{ 2.3, 1.2, 11.1, 9.2, 11.2, 0.0, 5.6, 12.1, 7.7, 8.5 },
				{ 5.1, 10.1, 8.1, 9.5, 8.5, 5.6, 0.0, 9.1, 8.3, 9.3 },
				{ 10.2, 12.8, 1.1, 12.0, 1.0, 12.1, 9.1, 0.0, 11.4, 12.4 },
				{ 6.1, 2.0, 10.5, 1.6, 10.6, 7.7, 8.3, 11.4, 0.0, 1.1 },
				{ 7.0, 1.0, 11.5, 1.1, 11.6, 8.5, 9.3, 12.4, 1.1, 0.0 } };

			auto opt = optimalMin(test);

			Assert::IsTrue(std::abs(opt.first - 8.8) < thresh);

			std::vector<int> expected = { 6, 5, 1, 3, 9 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {6, 5, 1, 3, 9}");
		}

		TEST_METHOD(TwentyByTwenty)
		{
			// The largest size the problem wants us to test
			std::vector<std::vector<double>> test{
				{ 0.0, 1.7, 0.5, 5.7, 4.2, 2.0, 0.5, 8.6, 9.2, 6.3, 3.5, 2.9, 7.7, 1.9, 4.3, 6.6, 7.1, 1.7, 9.9, 3.3, }, // 0
				{ 1.7, 0.0, 9.2, 8.1, 3.1, 4.3, 2.2, 9.1, 6.1, 7.1, 2.1, 1.9, 4.8, 8.1, 5.8, 6.1, 4.9, 5.3, 4.8, 2.7, }, // 1
				{ 0.5, 9.2, 0.0, 4.5, 1.7, 7.7, 3.8, 0.6, 4.6, 6.4, 2.2, 7.1, 0.2, 1.2, 0.3, 2.8, 7.7, 0.9, 1.9, 8.5, }, // 2
				{ 5.7, 8.1, 4.5, 0.0, 5.6, 1.9, 5.1, 2.9, 8.3, 5.6, 3.8, 7.2, 0.8, 4.8, 7.6, 7.1, 4.6, 8.4, 5.0, 9.1, }, // 3
				{ 4.2, 3.1, 1.7, 5.6, 0.0, 4.8, 1.8, 7.3, 4.0, 5.5, 5.1, 3.6, 3.8, 0.9, 2.4, 6.8, 6.6, 6.1, 3.5, 1.5, }, // 4
				{ 2.0, 4.3, 7.7, 1.9, 4.8, 0.0, 3.3, 1.0, 6.8, 8.6, 9.1, 3.2, 1.5, 8.2, 1.7, 7.0, 0.3, 7.6, 4.2, 4.4, }, // 5
				{ 0.5, 2.2, 3.8, 5.1, 1.8, 3.3, 0.0, 1.8, 3.3, 6.7, 7.4, 3.4, 6.1, 2.7, 3.0, 1.7, 8.2, 6.7, 1.1, 8.1, }, // 6
				{ 8.6, 9.1, 0.6, 2.9, 7.3, 1.0, 1.8, 0.0, 5.7, 5.9, 8.2, 2.9, 7.3, 1.0, 7.3, 2.1, 8.0, 4.6, 6.2, 2.4, }, // 7
				{ 9.2, 6.1, 4.6, 8.3, 4.0, 6.8, 3.3, 5.7, 0.0, 8.3, 3.6, 8.2, 7.4, 5.2, 6.0, 3.3, 8.8, 3.8, 1.4, 5.0, }, // 8
				{ 6.3, 7.1, 6.4, 5.6, 5.5, 8.6, 6.7, 5.9, 8.3, 0.0, 4.2, 3.7, 6.4, 8.1, 0.9, 9.9, 1.7, 8.0, 2.9, 7.7, }, // 9
				{ 3.5, 2.1, 2.2, 3.8, 5.1, 9.1, 7.4, 8.2, 3.6, 4.2, 0.0, 1.9, 2.1, 6.4, 7.1, 9.2, 6.9, 6.8, 4.6, 8.2, }, // 10
				{ 2.9, 1.9, 7.1, 7.2, 3.6, 3.2, 3.4, 2.9, 8.2, 3.7, 1.9, 0.0, 4.5, 3.2, 4.6, 9.1, 3.3, 8.1, 8.6, 3.0, }, // 11
				{ 7.7, 4.8, 0.2, 0.8, 3.8, 1.5, 6.1, 7.3, 7.4, 6.4, 2.1, 4.5, 0.0, 3.8, 7.9, 1.7, 0.6, 2.5, 8.0, 4.7, }, // 12
				{ 1.9, 8.1, 1.2, 4.8, 0.9, 8.2, 2.7, 1.0, 5.2, 8.1, 6.4, 3.2, 3.8, 0.0, 7.1, 1.8, 1.9, 6.6, 2.5, 9.5, }, // 13
				{ 4.3, 5.8, 0.3, 7.6, 2.4, 1.7, 3.0, 7.3, 6.0, 0.9, 7.1, 4.6, 7.9, 7.1, 0.0, 6.1, 3.7, 6.8, 7.1, 2.8, }, // 14
				{ 6.6, 6.1, 2.8, 7.1, 6.8, 7.0, 1.7, 2.1, 3.3, 9.9, 9.2, 9.1, 1.7, 1.8, 6.1, 0.0, 1.8, 3.6, 6.7, 1.2, }, // 15
				{ 7.1, 4.9, 7.7, 4.6, 6.6, 0.3, 8.2, 8.0, 8.8, 1.7, 6.9, 3.3, 0.6, 1.9, 3.7, 1.8, 0.0, 5.4, 3.9, 4.0, }, // 16
				{ 1.7, 5.3, 0.9, 8.4, 6.1, 7.6, 6.7, 4.6, 3.8, 8.0, 6.8, 8.1, 2.5, 6.6, 6.8, 3.6, 5.4, 0.0, 2.3, 6.8, }, // 17
				{ 9.9, 4.8, 1.9, 5.0, 3.5, 4.2, 1.1, 6.2, 1.4, 2.9, 4.6, 8.6, 8.0, 2.5, 7.1, 6.7, 3.9, 2.3, 0.0, 5.1, }, // 18
				{ 3.3, 2.7, 8.5, 9.1, 1.5, 4.4, 8.1, 2.4, 5.0, 7.7, 8.2, 3.0, 4.7, 9.5, 2.8, 1.2, 4.0, 6.8, 5.1, 0.0, }, };// 19

			double min = minPath(test);

			Assert::IsTrue(std::abs(min - 12.6) < thresh);
		}

		TEST_METHOD(OptimalTwentyByTwenty)
		{
			// optimalMin will find a better result over minPath, but it is a lot slower. Note that there are likely to be
			// some optimizations that could speed up the algorithm (maybe even some fairly large and clever ones)
			// In general the algorithm intends to store all possible valid paths (which is a lot for n = 20) and starts to 
			// consider their weight once the correct number of nodes have been visited, so it's going to be slower than minPath
			// The path is (5, 16, 12, 2, 14, 9, 18, 6, 0, 11)
			std::vector<std::vector<double>> test{
				{ 0.0, 1.7, 0.5, 5.7, 4.2, 2.0, 0.5, 8.6, 9.2, 6.3, 3.5, 2.9, 7.7, 1.9, 4.3, 6.6, 7.1, 1.7, 9.9, 3.3, }, // 0
				{ 1.7, 0.0, 9.2, 8.1, 3.1, 4.3, 2.2, 9.1, 6.1, 7.1, 2.1, 1.9, 4.8, 8.1, 5.8, 6.1, 4.9, 5.3, 4.8, 2.7, }, // 1
				{ 0.5, 9.2, 0.0, 4.5, 1.7, 7.7, 3.8, 0.6, 4.6, 6.4, 2.2, 7.1, 0.2, 1.2, 0.3, 2.8, 7.7, 0.9, 1.9, 8.5, }, // 2
				{ 5.7, 8.1, 4.5, 0.0, 5.6, 1.9, 5.1, 2.9, 8.3, 5.6, 3.8, 7.2, 0.8, 4.8, 7.6, 7.1, 4.6, 8.4, 5.0, 9.1, }, // 3
				{ 4.2, 3.1, 1.7, 5.6, 0.0, 4.8, 1.8, 7.3, 4.0, 5.5, 5.1, 3.6, 3.8, 0.9, 2.4, 6.8, 6.6, 6.1, 3.5, 1.5, }, // 4
				{ 2.0, 4.3, 7.7, 1.9, 4.8, 0.0, 3.3, 1.0, 6.8, 8.6, 9.1, 3.2, 1.5, 8.2, 1.7, 7.0, 0.3, 7.6, 4.2, 4.4, }, // 5
				{ 0.5, 2.2, 3.8, 5.1, 1.8, 3.3, 0.0, 1.8, 3.3, 6.7, 7.4, 3.4, 6.1, 2.7, 3.0, 1.7, 8.2, 6.7, 1.1, 8.1, }, // 6
				{ 8.6, 9.1, 0.6, 2.9, 7.3, 1.0, 1.8, 0.0, 5.7, 5.9, 8.2, 2.9, 7.3, 1.0, 7.3, 2.1, 8.0, 4.6, 6.2, 2.4, }, // 7
				{ 9.2, 6.1, 4.6, 8.3, 4.0, 6.8, 3.3, 5.7, 0.0, 8.3, 3.6, 8.2, 7.4, 5.2, 6.0, 3.3, 8.8, 3.8, 1.4, 5.0, }, // 8
				{ 6.3, 7.1, 6.4, 5.6, 5.5, 8.6, 6.7, 5.9, 8.3, 0.0, 4.2, 3.7, 6.4, 8.1, 0.9, 9.9, 1.7, 8.0, 2.9, 7.7, }, // 9
				{ 3.5, 2.1, 2.2, 3.8, 5.1, 9.1, 7.4, 8.2, 3.6, 4.2, 0.0, 1.9, 2.1, 6.4, 7.1, 9.2, 6.9, 6.8, 4.6, 8.2, }, // 10
				{ 2.9, 1.9, 7.1, 7.2, 3.6, 3.2, 3.4, 2.9, 8.2, 3.7, 1.9, 0.0, 4.5, 3.2, 4.6, 9.1, 3.3, 8.1, 8.6, 3.0, }, // 11
				{ 7.7, 4.8, 0.2, 0.8, 3.8, 1.5, 6.1, 7.3, 7.4, 6.4, 2.1, 4.5, 0.0, 3.8, 7.9, 1.7, 0.6, 2.5, 8.0, 4.7, }, // 12
				{ 1.9, 8.1, 1.2, 4.8, 0.9, 8.2, 2.7, 1.0, 5.2, 8.1, 6.4, 3.2, 3.8, 0.0, 7.1, 1.8, 1.9, 6.6, 2.5, 9.5, }, // 13
				{ 4.3, 5.8, 0.3, 7.6, 2.4, 1.7, 3.0, 7.3, 6.0, 0.9, 7.1, 4.6, 7.9, 7.1, 0.0, 6.1, 3.7, 6.8, 7.1, 2.8, }, // 14
				{ 6.6, 6.1, 2.8, 7.1, 6.8, 7.0, 1.7, 2.1, 3.3, 9.9, 9.2, 9.1, 1.7, 1.8, 6.1, 0.0, 1.8, 3.6, 6.7, 1.2, }, // 15
				{ 7.1, 4.9, 7.7, 4.6, 6.6, 0.3, 8.2, 8.0, 8.8, 1.7, 6.9, 3.3, 0.6, 1.9, 3.7, 1.8, 0.0, 5.4, 3.9, 4.0, }, // 16
				{ 1.7, 5.3, 0.9, 8.4, 6.1, 7.6, 6.7, 4.6, 3.8, 8.0, 6.8, 8.1, 2.5, 6.6, 6.8, 3.6, 5.4, 0.0, 2.3, 6.8, }, // 17
				{ 9.9, 4.8, 1.9, 5.0, 3.5, 4.2, 1.1, 6.2, 1.4, 2.9, 4.6, 8.6, 8.0, 2.5, 7.1, 6.7, 3.9, 2.3, 0.0, 5.1, }, // 18
				{ 3.3, 2.7, 8.5, 9.1, 1.5, 4.4, 8.1, 2.4, 5.0, 7.7, 8.2, 3.0, 4.7, 9.5, 2.8, 1.2, 4.0, 6.8, 5.1, 0.0, }, };// 19

			auto opt = optimalMin(test);

			Assert::IsTrue(std::abs(opt.first - 9.7) < thresh);

			std::vector<int> expected = { 5, 16, 12, 2, 14, 9, 18, 6, 0, 11 };
			Assert::IsTrue(opt.second == expected, L"Expected result is {5, 16, 12, 2, 14, 9, 18, 0, 11}");
		}
	};
}
