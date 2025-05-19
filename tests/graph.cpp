#include <doctest/doctest.h>

#include <hana/graph.hpp>

class TestRDGNode : public hana::GraphNode {
public:
	explicit TestRDGNode(const char* n): name(n) {}

	std::string name;
};

class TestRDGEdge : public hana::GraphEdge {
public:
	explicit TestRDGEdge(const char* n): name(n) {}

	std::string name;
};

TEST_CASE("graph") {
	using namespace hana;

	Graph graph;
	TestRDGNode node0("node0");
	TestRDGNode node1("node1");
	TestRDGNode node2("node2");
	TestRDGNode node3("node3");

	TestRDGEdge edge0("edge0");
	TestRDGEdge edge1("edge1");
	TestRDGEdge edge2("edge2");

	graph.add_edge(&node0, &node1);
	graph.add_edge(&node1, &node2, &edge0);
	graph.add_edge(&node0, &node2, &edge1);
	graph.add_edge(&node2, &node3, &edge2);
	graph.add_edge(&node2, &node1);

	std::set<std::string> res;

	SUBCASE("basic") {
		res.emplace("node1");
		res.emplace("node2");
		graph.foreach_neighbors(&node0, [&](GraphNode* node) {
			auto iter = res.find(reinterpret_cast<TestRDGNode*>(node)->name);
			CHECK(iter != res.end());
			res.erase(std::move(iter));
		});
		res.emplace("edge2");
		int num = 0;
		graph.foreach_outgoing_edges(&node2, [&](GraphEdge* edge) {
			num++;
			if (edge) {
				auto iter = res.find(reinterpret_cast<TestRDGEdge*>(edge)->name);
				CHECK(iter != res.end());
				res.erase(std::move(iter));
			}
		});
		CHECK_EQ(num, 2);
	}

	SUBCASE("remove edge") {
		graph.remove_edge(&edge2);
		graph.foreach_incoming_edges(&node3, [](GraphEdge* edge) {
			CHECK(false);
		});
		graph.remove_edge(&node0, &node2);
		res.emplace("node1");
		graph.foreach_inv_neighbors(&node2, [&](GraphNode* node) {
			auto iter = res.find(reinterpret_cast<TestRDGNode*>(node)->name);
			CHECK(iter != res.end());
			res.erase(std::move(iter));
		});
	}

	SUBCASE("remove vertex") {
		graph.remove_vertex(&node2);
		res.emplace("node0");
		graph.foreach_inv_neighbors(&node1, [&](GraphNode* node) {
			auto iter = res.find(reinterpret_cast<TestRDGNode*>(node)->name);
			CHECK(iter != res.end());
			res.erase(std::move(iter));
		});
	}

	graph.clear();
	graph.add_edge(&node0, &node1);
	graph.add_edge(&node1, &node2, &edge0);
	graph.add_edge(&node0, &node2, &edge1);
	graph.add_edge(&node2, &node3, &edge2);
	graph.add_edge(&node2, &node1);

	SUBCASE("basic2") {
		res.emplace("node1");
		res.emplace("node2");
		graph.foreach_neighbors(&node0, [&](GraphNode* node) {
			auto iter = res.find(reinterpret_cast<TestRDGNode*>(node)->name);
			CHECK(iter != res.end());
			res.erase(std::move(iter));
		});
		res.emplace("edge2");
		int num = 0;
		graph.foreach_outgoing_edges(&node2, [&](GraphEdge* edge) {
			num++;
			if (edge) {
				auto iter = res.find(reinterpret_cast<TestRDGEdge*>(edge)->name);
				CHECK(iter != res.end());
				res.erase(std::move(iter));
			}
		});
		CHECK_EQ(num, 2);
	}
}
