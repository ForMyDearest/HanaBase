#include <hana/graph/graphviz.hpp>

int main() {
	using namespace hana::graphviz;

	auto registry = Registry::create();

	auto graph = registry->create_graph("G");

	auto v_a0 = registry->create_node("a0");
	auto v_a1 = registry->create_node("a1");
	auto v_a2 = registry->create_node("a2");
	auto v_a3 = registry->create_node("a3");

	auto v_b0 = registry->create_node("b0");
	auto v_b1 = registry->create_node("b1");
	auto v_b2 = registry->create_node("b2");
	auto v_b3 = registry->create_node("b3");

	auto v_start = registry->create_node("start");
	auto v_end = registry->create_node("end");

	auto e_a0_a1 = registry->create_edge(v_a0, v_a1);
	auto e_a1_a2 = registry->create_edge(v_a1, v_a2);
	auto e_a2_a3 = registry->create_edge(v_a2, v_a3);
	auto e_a3_a0 = registry->create_edge(v_a3, v_a0);

	auto e_b0_b1 = registry->create_edge(v_b0, v_b1);
	auto e_b1_b2 = registry->create_edge(v_b1, v_b2);
	auto e_b2_b3 = registry->create_edge(v_b2, v_b3);

	auto e_a1_b3 = registry->create_edge(v_a1, v_b3);
	auto e_b2_a3 = registry->create_edge(v_b2, v_a3);

	auto e_start_a0 = registry->create_edge(v_start, v_a0);
	auto e_start_b0 = registry->create_edge(v_start, v_b0);

	auto e_a3_end = registry->create_edge(v_a3, v_end);
	auto e_b3_end = registry->create_edge(v_b3, v_end);

	auto c0 = registry->create_subgraph("cluster0");
	c0
		->add_node_attribute(NodeAttr::style_, "filled")
		->add_node_attribute(NodeAttr::color_, "white")
		->add_subgraph_attribute(SubgraphAttr::style_, "filled")
		->add_subgraph_attribute(SubgraphAttr::color_, "lightgrey")
		->add_subgraph_attribute(SubgraphAttr::label_, "process #1")

		->attach_edge(e_a0_a1)
		->attach_edge(e_a1_a2)
		->attach_edge(e_a2_a3);

	auto c1 = registry->create_subgraph("cluster1");
	c1
		->add_node_attribute(NodeAttr::style_, "filled")
		->add_subgraph_attribute(SubgraphAttr::color_, "blue")
		->add_subgraph_attribute(SubgraphAttr::label_, "process #2")

		->attach_edge(e_b0_b1)
		->attach_edge(e_b1_b2)
		->attach_edge(e_b2_b3);

	v_start->add_attribute(NodeAttr::shape_, "Mdiamond");
	v_end->add_attribute(NodeAttr::shape_, "Msquare");

	graph
		->attach_node(v_start)
		->attach_node(v_end)

		->attach_edge(e_start_a0)
		->attach_edge(e_start_b0)
		->attach_edge(e_a1_b3)
		->attach_edge(e_b2_a3)
		->attach_edge(e_a3_a0)
		->attach_edge(e_a3_end)
		->attach_edge(e_b3_end)

		->attach_subgraph(c0)
		->attach_subgraph(c1);

	graph->dump("subgraph.dot", true);

	return 0;
}
