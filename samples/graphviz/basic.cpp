#include <hana/graph/graphviz.hpp>

int main() {
	using namespace hana::graphviz;

	auto registry = Registry::create();

	auto graph = registry->create_graph("hello_world");

	auto v_a = registry->create_node("a");
	auto v_b = registry->create_node("b");
	auto v_c = registry->create_node("c");
	auto v_d = registry->create_node("d");

	auto e_ab = registry->create_edge(v_a, v_b);
	auto e_ac = registry->create_edge(v_a, v_c);
	auto e_bd = registry->create_edge(v_b, v_d);
	auto e_cd = registry->create_edge(v_c, v_d);

	graph
		->attach_edge(e_ab)
		->attach_edge(e_ac)
		->attach_edge(e_bd)
		->attach_edge(e_cd);

	graph->dump("basic.dot", true);

	return 0;
}
