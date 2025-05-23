#pragma once

#include "hana/platform/macros.h"
#include "hana/container/string_view.hpp"

#include <memory>

namespace hana::graphviz
{
	enum class Compass {
		N,  // n
		NE, // ne
		E,  // e
		SE, // se
		S,  // s
		SW, // sw
		W,  // w
		NW, // nw
		C,  // c
		None,
	};

	// link : https://www.graphviz.org/docs/nodes/
	enum class NodeAttr {
		// Indicates the preferred area for a node or empty cluster. For patchwork only.
		area_,
		// Classnames to attach to the node, edge, graph, or cluster's SVG element. For svg only.
		class_,
		// Basic drawing color for graphics, not text.
		color_,
		// A color scheme namespace: the context for interpreting color names.
		colorscheme_,
		// Comments are inserted into output.
		comment_,
		// Distortion factor for shape=polygon.
		distortion_,
		// Color used to fill the background of a node or cluster.
		fillcolor_,
		// Whether to use the specified width and height attributes to choose node size (rather than sizing to fit the node contents).
		fixedsize_,
		// Color used for text.
		fontcolor_,
		// Font used for text.
		fontname_,
		// Font size, in points, used for text.
		fontsize_,
		// If a gradient fill is being used, this determines the angle of the fill.
		gradientangle_,
		// Name for a group of nodes, for bundling edges avoiding crossings. For dot only.
		group_,
		// Height of node, in inches.
		height_,
		// Synonym for URL. For map, postscript, svg only.
		href_,
		// Identifier for graph objects. For map, postscript, svg only.
		id_,
		// Gives the name of a file containing an image to be displayed inside a node.
		image_,
		// Controls how an image is positioned within its containing node.
		imagepos_,
		// Controls how an image fills its containing node.
		imagescale_,
		// Text label attached to objects.
		label_,
		// Vertical placement of labels for nodes, root graphs and clusters.
		labelloc_,
		// Specifies layers in which the node, edge or cluster is present.
		layer_,
		// For graphs, this sets x and y margins of canvas, in inches.
		margin_,
		// Whether to justify multiline text vs the previous text line (rather than the side of the container).
		nojustify_,
		// Constrains the left-to-right ordering of node edges. For dot only.
		ordering_,
		// node shape rotation angle, or graph orientation.
		orientation_,
		// Specifies the width of the pen, in points, used to draw lines and curves.
		penwidth_,
		// Set number of peripheries used in polygonal shapes and cluster boundaries.
		peripheries_,
		// Keeps the node at the node's given input position. For neato, fdp only.
		pin_,
		// Position of node, or spline control points. For neato, fdp only.
		pos_,
		// Rectangles for fields of records, in points. For write only.
		rects_,
		// If true, force polygon to be regular.
		regular_,
		// Specifies nodes to be used as the center of the layout. For twopi, circo only.
		root_,
		// Gives the number of points used for a circle/ellipse node.
		samplepoints_,
		// Sets the shape of a node.
		shape_,
		// A file containing user-supplied node content.
		shapefile_,
		// Print guide boxes for debugging. For dot only.
		showboxes_,
		// Number of sides when shape=polygon.
		sides_,
		// Skew factor for shape=polygon.
		skew_,
		// Sort order of graph components for ordering packmode packing.
		sortv_,
		// Set style information for components of the graph.
		style_,
		// If the object has a URL, this attribute determines which window of the browser is used for the URL. For map, svg only.
		target_,
		// Tooltip (mouse hover text) attached to the node, edge, cluster, or graph. For cmap, svg only.
		tooltip_,
		// Hyperlinks incorporated into device-dependent output. For map, postscript, svg only.
		URL_,
		// Sets the coordinates of the vertices of the node's polygon, in inches. For write only.
		vertices_,
		// Width of node, in inches.
		width_,
		// External label for a node or edge.
		xlabel_,
		// Position of an exterior label, in points. For write only.
		xlp_,
		// Z-coordinate value for 3D layouts and displays.
		z_,
		__count__
	};

	// link : https://www.graphviz.org/docs/edges/
	enum class EdgeAttr {
		// Style of arrowhead on the head node of an edge.
		arrowhead_,
		// Multiplicative scale factor for arrowheads.
		arrowsize_,
		// Style of arrowhead on the tail node of an edge.
		arrowtail_,
		// Classnames to attach to the node, edge, graph, or cluster's SVG element. For svg only.
		class_,
		// Basic drawing color for graphics, not text.
		color_,
		// A color scheme namespace: the context for interpreting color names.
		colorscheme_,
		// Comments are inserted into output.
		comment_,
		// If false, the edge is not used in ranking the nodes. For dot only.
		constraint_,
		// Whether to connect the edge label to the edge with a line.
		decorate_,
		// Edge type for drawing arrowheads.
		dir_,
		// Synonym for edgeURL. For map, svg only.
		edgehref_,
		// Browser window to use for the edgeURL link. For map, svg only.
		edgetarget_,
		// Tooltip annotation attached to the non-label part of an edge. For cmap, svg only.
		edgetooltip_,
		// The link for the non-label parts of an edge. For map, svg only.
		edgeURL_,
		// Color used to fill the background of a node or cluster.
		fillcolor_,
		// Color used for text.
		fontcolor_,
		// Font used for text.
		fontname_,
		// Font size, in points, used for text.
		fontsize_,
		// Center position of an edge's head label. For write only.
		head_lp_,
		// If true, the head of an edge is clipped to the boundary of the head node.
		headclip_,
		// Synonym for headURL. For map, svg only.
		headhref_,
		// Text label to be placed near head of edge.
		headlabel_,
		// Indicates where on the head node to attach the head of the edge.
		headport_,
		// Browser window to use for the headURL link. For map, svg only.
		headtarget_,
		// Tooltip annotation attached to the head of an edge. For cmap, svg only.
		headtooltip_,
		// If defined, headURL is output as part of the head label of the edge. For map, svg only.
		headURL_,
		// Synonym for URL. For map, postscript, svg only.
		href_,
		// Identifier for graph objects. For map, postscript, svg only.
		id_,
		// Text label attached to objects.
		label_,
		// The angle (in degrees) in polar coordinates of the head & tail edge labels.
		labelangle_,
		// Scaling factor for the distance of headlabel / taillabel from the head / tail nodes.
		labeldistance_,
		// If true, allows edge labels to be less constrained in position.
		labelfloat_,
		// Color used for headlabel and taillabel.
		labelfontcolor_,
		// Font for headlabel and taillabel.
		labelfontname_,
		// Font size of headlabel and taillabel.
		labelfontsize_,
		// Synonym for labelURL. For map, svg only.
		labelhref_,
		// Browser window to open labelURL links in. For map, svg only.
		labeltarget_,
		// Tooltip annotation attached to label of an edge. For cmap, svg only.
		labeltooltip_,
		// If defined, labelURL is the link used for the label of an edge. For map, svg only.
		labelURL_,
		// Specifies layers in which the node, edge or cluster is present.
		layer_,
		// Preferred edge length, in inches. For neato, fdp only.
		len_,
		// Logical head of an edge. For dot only.
		lhead_,
		// Label center position. For write only.
		lp_,
		// Logical tail of an edge. For dot only.
		ltail_,
		// Minimum edge length (rank difference between head and tail). For dot only.
		minlen_,
		// Whether to justify multiline text vs the previous text line (rather than the side of the container).
		nojustify_,
		// Specifies the width of the pen, in points, used to draw lines and curves.
		penwidth_,
		// Position of node, or spline control points. For neato, fdp only.
		pos_,
		// Edges with the same head and the same samehead value are aimed at the same point on the head. For dot only.
		samehead_,
		// Edges with the same tail and the same sametail value are aimed at the same point on the tail. For dot only.
		sametail_,
		// Print guide boxes for debugging. For dot only.
		showboxes_,
		// Set style information for components of the graph.
		style_,
		// Position of an edge's tail label, in points. For write only.
		tail_lp_,
		// If true, the tail of an edge is clipped to the boundary of the tail node.
		tailclip_,
		// Synonym for tailURL. For map, svg only.
		tailhref_,
		// Text label to be placed near tail of edge.
		taillabel_,
		// Indicates where on the tail node to attach the tail of the edge.
		tailport_,
		// Browser window to use for the tailURL link. For map, svg only.
		tailtarget_,
		// Tooltip annotation attached to the tail of an edge. For cmap, svg only.
		tailtooltip_,
		// If defined, tailURL is output as part of the tail label of the edge. For map, svg only.
		tailURL_,
		// If the object has a URL, this attribute determines which window of the browser is used for the URL. For map, svg only.
		target_,
		// Tooltip (mouse hover text) attached to the node, edge, cluster, or graph. For cmap, svg only.
		tooltip_,
		// Hyperlinks incorporated into device-dependent output. For map, postscript, svg only.
		URL_,
		// Weight of edge.
		weight_,
		// External label for a node or edge.
		xlabel_,
		// Position of an exterior label, in points. For write only.
		xlp_,
		__count__
	};

	// link : https://www.graphviz.org/docs/graph/
	enum class GraphAttr {
		// A string in the xdot format specifying an arbitrary background.
		_background_,
		// Bounding box of drawing in points. For write only.
		bb_,
		// Whether to draw leaf nodes uniformly in a circle around the root node in sfdp. For sfdp only.
		beautify_,
		// Canvas background color.
		bgcolor_,
		// Whether to center the drawing in the output canvas.
		center_,
		// Character encoding used when interpreting string input as a text label.
		charset_,
		// Classnames to attach to the node, edge, graph, or cluster's SVG element. For svg only.
		class_,
		// Mode used for handling clusters. For dot only.
		clusterrank_,
		// A color scheme namespace: the context for interpreting color names.
		colorscheme_,
		// Comments are inserted into output.
		comment_,
		// If true, allow edges between clusters. For dot only.
		compound_,
		// If true, use edge concentrators.
		concentrate_,
		// Factor damping force motions. For neato only.
		Damping_,
		// The distance between nodes in separate connected components. For neato only.
		defaultdist_,
		// Set the number of dimensions used for the layout. For neato, fdp, sfdp only.
		dim_,
		// Set the number of dimensions used for rendering. For neato, fdp, sfdp only.
		dimen_,
		// Whether to constrain most edges to point downwards. For neato only.
		diredgeconstraints_,
		// Specifies the expected number of pixels per inch on a display device. For bitmap output, svg only.
		dpi_,
		// Terminating condition. For neato only.
		epsilon_,
		// Margin used around polygons for purposes of spline edge routing. For neato, fdp, sfdp, osage, circo, twopi only.
		esep_,
		// Color used for text.
		fontcolor_,
		// Font used for text.
		fontname_,
		// Allows user control of how basic fontnames are represented in SVG output. For svg only.
		fontnames_,
		// Directory list used by libgd to search for bitmap fonts.
		fontpath_,
		// Font size, in points, used for text.
		fontsize_,
		// Whether to force placement of all xlabels, even if overlapping.
		forcelabels_,
		// If a gradient fill is being used, this determines the angle of the fill.
		gradientangle_,
		// Synonym for URL. For map, postscript, svg only.
		href_,
		// Identifier for graph objects. For map, postscript, svg only.
		id_,
		// A list of directories in which to look for image files.
		imagepath_,
		// Scales the input positions to convert between length units. For neato, fdp only.
		inputscale_,
		// Spring constant used in virtual physical model. For fdp, sfdp only.
		K_,
		// Text label attached to objects.
		label_,
		// Whether to treat a node whose name has the form |edgelabel|* as a special node representing an edge label. For sfdp only.
		label_scheme_,
		// Justification for graph & cluster labels.
		labeljust_,
		// Vertical placement of labels for nodes, root graphs and clusters.
		labelloc_,
		// If true, the graph is rendered in landscape mode.
		landscape_,
		// The separator characters used to split attributes of type layerRange into a list of ranges.
		layerlistsep_,
		// A linearly ordered list of layer names attached to the graph.
		layers_,
		// Selects a list of layers to be emitted.
		layerselect_,
		// The separator characters for splitting the layers attribute into a list of layer names.
		layersep_,
		// Which layout engine to use.
		layout_,
		// Number of levels allowed in the multilevel scheme. For sfdp only.
		levels_,
		// strictness of neato level constraints. For neato only.
		levelsgap_,
		// Height of graph or cluster label, in inches. For write only.
		lheight_,
		// How long strings should get before overflowing to next line, for text output.
		linelength_,
		// Label center position. For write only.
		lp_,
		// Width of graph or cluster label, in inches. For write only.
		lwidth_,
		// For graphs, this sets x and y margins of canvas, in inches.
		margin_,
		// Sets the number of iterations used. For neato, fdp only.
		maxiter_,
		// Scale factor for mincross (mc) edge crossing minimiser parameters. For dot only.
		mclimit_,
		// Specifies the minimum separation between all nodes. For circo only.
		mindist_,
		// Technique for optimizing the layout. For neato only.
		mode_,
		// Specifies how the distance matrix is computed for the input graph. For neato only.
		model_,
		// Whether to use a single global ranking, ignoring clusters. For dot only.
		newrank_,
		// In dot, nodesep specifies the minimum space between two adjacent nodes in the same rank, in inches.
		nodesep_,
		// Whether to justify multiline text vs the previous text line (rather than the side of the container).
		nojustify_,
		// normalizes coordinates of final layout. For neato, fdp, sfdp, twopi, circo only.
		normalize_,
		// Whether to avoid translating layout to the origin point. For neato only.
		notranslate_,
		// Sets number of iterations in network simplex applications. For dot only.
		nslimit_,
		// Sets number of iterations in network simplex applications. For dot only.
		nslimit1_,
		// Whether to draw circo graphs around one circle. For circo only.
		oneblock_,
		// Constrains the left-to-right ordering of node edges. For dot only.
		ordering_,
		// node shape rotation angle, or graph orientation.
		orientation_,
		// Specify order in which nodes and edges are drawn.
		outputorder_,
		// Determines if and how node overlaps should be removed. For fdp, neato, sfdp, circo, twopi only.
		overlap_,
		// Scale layout by factor, to reduce node overlap. For prism, neato, sfdp, fdp, circo, twopi only.
		overlap_scaling_,
		// Whether the overlap removal algorithm should perform a compression pass to reduce the size of the layout. For prism only.
		overlap_shrink_,
		// Whether each connected component of the graph should be laid out separately, and then the graphs packed together.
		pack_,
		// How connected components should be packed.
		packmode_,
		// Inches to extend the drawing area around the minimal area needed to draw the graph.
		pad_,
		// Width and height of output pages, in inches.
		page_,
		// The order in which pages are emitted.
		pagedir_,
		// Quadtree scheme to use. For sfdp only.
		quadtree_,
		// If quantum > 0.0, node label dimensions will be rounded to integral multiples of the quantum.
		quantum_,
		// Sets direction of graph layout. For dot only.
		rankdir_,
		// Specifies separation between ranks. For dot, twopi only.
		ranksep_,
		// Sets the aspect ratio (drawing height/drawing width) for the drawing.
		ratio_,
		// If there are multiple clusters, whether to run edge crossing minimization a second time. For dot only.
		remincross_,
		// The power of the repulsive force used in an extended Fruchterman-Reingold. For sfdp only.
		repulsiveforce_,
		// Synonym for dpi. For bitmap output, svg only.
		resolution_,
		// Specifies nodes to be used as the center of the layout. For twopi, circo only.
		root_,
		// If rotate=90, sets drawing orientation to landscape.
		rotate_,
		// Rotates the final layout counter-clockwise by the specified number of degrees. For sfdp only.
		rotation_,
		// Scales layout by the given factor after the initial layout. For neato, twopi only.
		scale_,
		// During network simplex, the maximum number of edges with negative cut values to search when looking for an edge with minimum cut value. For dot only.
		searchsize_,
		// Margin to leave around nodes when removing node overlap. For fdp, neato, sfdp, osage, circo, twopi only.
		sep_,
		// Print guide boxes for debugging. For dot only.
		showboxes_,
		// Maximum width and height of drawing, in inches.
		size_,
		// Specifies a post-processing step used to smooth out an uneven distribution of nodes. For sfdp only.
		smoothing_,
		// Sort order of graph components for ordering packmode packing.
		sortv_,
		// Controls how, and if, edges are represented.
		splines_,
		// Parameter used to determine the initial layout of nodes. For neato, fdp, sfdp only.
		start_,
		// Set style information for components of the graph.
		style_,
		// A URL or pathname specifying an XML style sheet, used in SVG output. For svg only.
		stylesheet_,
		// If the object has a URL, this attribute determines which window of the browser is used for the URL. For map, svg only.
		target_,
		// Which rank to move floating (loose) nodes to. For dot only.
		TBbalance_,
		// Tooltip (mouse hover text) attached to the node, edge, cluster, or graph. For cmap, svg only.
		tooltip_,
		// Whether internal bitmap rendering relies on a truecolor color model or uses. For bitmap output only.
		truecolor_,
		// Hyperlinks incorporated into device-dependent output. For map, postscript, svg only.
		URL_,
		// Clipping window on final drawing.
		viewport_,
		// Tuning margin of Voronoi technique. For neato, fdp, sfdp, twopi, circo only.
		voro_margin_,
		// Determines the version of xdot used in output. For xdot only.
		xdotversion_,
		__count__
	};

	// link : https://www.graphviz.org/docs/clusters/
	enum class SubgraphAttr {
		// Indicates the preferred area for a node or empty cluster. For patchwork only.
		area_,
		// Bounding box of drawing in points. For write only.
		bb_,
		// Canvas background color.
		bgcolor_,
		// Classnames to attach to the node, edge, graph, or cluster's SVG element. For svg only.
		class_,
		// Whether the subgraph is a cluster.
		cluster_,
		// Basic drawing color for graphics, not text.
		color_,
		// A color scheme namespace: the context for interpreting color names.
		colorscheme_,
		// Color used to fill the background of a node or cluster.
		fillcolor_,
		// Color used for text.
		fontcolor_,
		// Font used for text.
		fontname_,
		// Font size, in points, used for text.
		fontsize_,
		// If a gradient fill is being used, this determines the angle of the fill.
		gradientangle_,
		// Synonym for URL. For map, postscript, svg only.
		href_,
		// Identifier for graph objects. For map, postscript, svg only.
		id_,
		// Spring constant used in virtual physical model. For fdp, sfdp only.
		K_,
		// Text label attached to objects.
		label_,
		// Justification for graph & cluster labels.
		labeljust_,
		// Vertical placement of labels for nodes, root graphs and clusters.
		labelloc_,
		// Specifies layers in which the node, edge or cluster is present.
		layer_,
		// Height of graph or cluster label, in inches. For write only.
		lheight_,
		// Label center position. For write only.
		lp_,
		// Width of graph or cluster label, in inches. For write only.
		lwidth_,
		// For graphs, this sets x and y margins of canvas, in inches.
		margin_,
		// Whether to justify multiline text vs the previous text line (rather than the side of the container).
		nojustify_,
		// normalizes coordinates of final layout. For neato, fdp, sfdp, twopi, circo only.
		normalize_,
		// Color used to draw the bounding box around a cluster.
		pencolor_,
		// Specifies the width of the pen, in points, used to draw lines and curves.
		penwidth_,
		// Set number of peripheries used in polygonal shapes and cluster boundaries.
		peripheries_,
		// Sort order of graph components for ordering packmode packing.
		sortv_,
		// Set style information for components of the graph.
		style_,
		// If the object has a URL, this attribute determines which window of the browser is used for the URL. For map, svg only.
		target_,
		// Tooltip (mouse hover text) attached to the node, edge, cluster, or graph. For cmap, svg only.
		tooltip_,
		// Hyperlinks incorporated into device-dependent output. For map, postscript, svg only.
		URL_,
		__count__
	};

	struct HANA_BASE_API Node {
		virtual ~Node() noexcept = default;

		virtual Node* add_attribute(NodeAttr key, HStringView value) noexcept = 0;
		virtual Node* remove_attribute(NodeAttr key) noexcept = 0;
		virtual Node* clear_attributes() noexcept = 0;
	};

	struct HANA_BASE_API Edge {
		virtual ~Edge() noexcept = default;

		virtual Edge* add_attribute(EdgeAttr key, HStringView value) noexcept = 0;
		virtual Edge* remove_attribute(EdgeAttr key) noexcept = 0;
		virtual Edge* clear_attributes() noexcept = 0;

		virtual Edge* set_ports(const struct Port* from, const Port* to) noexcept = 0;
		virtual Edge* reset_ports() noexcept = 0;
	};

	struct HANA_BASE_API Subgraph {
		virtual ~Subgraph() noexcept = default;

		virtual Subgraph* attach_node(const Node* node) noexcept = 0;
		virtual Subgraph* detach_node(const Node* node) noexcept = 0;
		virtual Subgraph* clear_nodes() noexcept = 0;

		virtual Subgraph* attach_edge(const Edge* edge) noexcept = 0;
		virtual Subgraph* detach_edge(const Edge* edge) noexcept = 0;
		virtual Subgraph* clear_edges() noexcept = 0;

		virtual Subgraph* attach_subgraph(const Subgraph* subgraph) noexcept = 0;
		virtual Subgraph* detach_subgraph(const Subgraph* subgraph) noexcept = 0;
		virtual Subgraph* clear_subgraphs() noexcept = 0;

		virtual Subgraph* add_subgraph_attribute(SubgraphAttr key, HStringView value) noexcept = 0;
		virtual Subgraph* remove_subgraph_attribute(SubgraphAttr key) noexcept = 0;
		virtual Subgraph* clear_subgraph_attributes() noexcept = 0;
		// apply to all nodes
		virtual Subgraph* add_node_attribute(NodeAttr key, HStringView value) noexcept = 0;
		virtual Subgraph* remove_node_attribute(NodeAttr key) noexcept = 0;
		virtual Subgraph* clear_node_attributes() noexcept = 0;
		// apply to all edges
		virtual Subgraph* add_edge_attribute(EdgeAttr key, HStringView value) noexcept = 0;
		virtual Subgraph* remove_edge_attribute(EdgeAttr key) noexcept = 0;
		virtual Subgraph* clear_edge_attributes() noexcept = 0;
	};

	struct HANA_BASE_API Graph {
		virtual ~Graph() noexcept = default;

		virtual Graph* attach_node(const Node* node) noexcept = 0;
		virtual Graph* detach_node(const Node* node) noexcept = 0;
		virtual Graph* clear_nodes() noexcept = 0;

		virtual Graph* attach_edge(const Edge* edge) noexcept = 0;
		virtual Graph* detach_edge(const Edge* edge) noexcept = 0;
		virtual Graph* clear_edges() noexcept = 0;

		virtual Graph* attach_subgraph(const Subgraph* subgraph) noexcept = 0;
		virtual Graph* detach_subgraph(const Subgraph* subgraph) noexcept = 0;
		virtual Graph* clear_subgraphs() noexcept = 0;

		virtual Graph* add_graph_attribute(GraphAttr key, HStringView value) noexcept = 0;
		virtual Graph* remove_graph_attribute(GraphAttr key) noexcept = 0;
		virtual Graph* clear_graph_attributes() noexcept = 0;
		// apply to all nodes
		virtual Graph* add_node_attribute(NodeAttr key, HStringView value) noexcept = 0;
		virtual Graph* remove_node_attribute(NodeAttr key) noexcept = 0;
		virtual Graph* clear_node_attributes() noexcept = 0;
		// apply to all edges
		virtual Graph* add_edge_attribute(EdgeAttr key, HStringView value) noexcept = 0;
		virtual Graph* remove_edge_attribute(EdgeAttr key) noexcept = 0;
		virtual Graph* clear_edge_attributes() noexcept = 0;

		virtual void dump(HStringView filename, bool is_digraph) const noexcept = 0;
	};

	struct HANA_BASE_API Registry {
		static std::unique_ptr<Registry> create() noexcept;

		virtual ~Registry() noexcept = default;

		virtual Node* create_node(HStringView id) noexcept = 0;
		virtual Edge* create_edge(const Node* from, const Node* to) noexcept = 0;
		virtual Port* create_port(HStringView id) noexcept = 0;
		virtual Port* create_port(HStringView id, Compass compass) noexcept = 0;
		virtual Subgraph* create_subgraph(HStringView id) noexcept = 0;
		virtual Graph* create_graph(HStringView id) noexcept = 0;
	};
}
