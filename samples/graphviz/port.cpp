#include <hana/graph/graphviz.hpp>

int main() {
	using namespace hana::graphviz;

	auto registry = Registry::create();

	auto graph = registry->create_graph("structs");

	auto v_struct1 = registry->create_node("struct1");
	auto v_struct2 = registry->create_node("struct2");
	auto v_struct3 = registry->create_node("struct3");

	auto e_struct1_f1_struct2_f0 = registry->create_edge(v_struct1, v_struct2);
	auto e_struct1_f2_struct3_here = registry->create_edge(v_struct1, v_struct3);

	graph->add_node_attribute(NodeAttr::shape_, "record");
	v_struct1->add_attribute(NodeAttr::label_, "<f0> left|<f1> mid&#92; dle|<f2> right");
	v_struct2->add_attribute(NodeAttr::label_, "<f0> one|<f1> two");
	v_struct3->add_attribute(NodeAttr::label_, "hello&#92;nworld |{ b |{c|<here> d|e}| f}| g | h");

	auto p_1 = registry->create_port("f1");
	auto p_0 = registry->create_port("f0");
	auto p_2 = registry->create_port("f2");
	auto p_here = registry->create_port("here");

	e_struct1_f1_struct2_f0->set_ports(p_1, p_0);
	e_struct1_f2_struct3_here->set_ports(p_2, p_here);

	graph
		->attach_node(v_struct1)
		->attach_node(v_struct2)
		->attach_node(v_struct3)

		->attach_edge(e_struct1_f1_struct2_f0)
		->attach_edge(e_struct1_f2_struct3_here);

	graph->dump("port.dot", true);

	return 0;
}
