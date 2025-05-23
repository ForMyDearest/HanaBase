#include <hana/graph/graphviz.hpp>

int main() {
	using namespace hana::graphviz;

	auto registry = Registry::create();

	auto graph = registry->create_graph("attr");

	auto v_a = registry->create_node("a");
	auto v_b = registry->create_node("b");
	auto v_c = registry->create_node("c");
	auto v_d = registry->create_node("d");
	auto v_e = registry->create_node("e");

	auto e_ab = registry->create_edge(v_a, v_b);
	auto e_bc = registry->create_edge(v_b, v_c);
	auto e_bd = registry->create_edge(v_b, v_d);

	v_a
		->add_attribute(NodeAttr::shape_, "polygon")
		->add_attribute(NodeAttr::sides_, "5")
		->add_attribute(NodeAttr::peripheries_, "3")
		->add_attribute(NodeAttr::color_, "lightblue")
		->add_attribute(NodeAttr::style_, "filled");

	v_c
		->add_attribute(NodeAttr::shape_, "polygon")
		->add_attribute(NodeAttr::sides_, "4")
		->add_attribute(NodeAttr::skew_, ".4")
		->add_attribute(NodeAttr::label_, "hello world");

	v_d->add_attribute(NodeAttr::shape_, "invtriangle");

	v_e
		->add_attribute(NodeAttr::shape_, "polygon")
		->add_attribute(NodeAttr::sides_, "4")
		->add_attribute(NodeAttr::distortion_, ".7");

	graph
		->attach_node(v_a)
		->attach_node(v_b)
		->attach_node(v_c)
		->attach_node(v_d)
		->attach_node(v_e)

		->attach_edge(e_ab)
		->attach_edge(e_bc)
		->attach_edge(e_bd);

	graph->dump("attr.dot", true);

	return 0;
}
