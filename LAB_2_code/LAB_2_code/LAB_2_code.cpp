#include<vector>
#include<iostream>
#include<string>
#include<deque>

// alternatively to always typing std::
// you can also write
using namespace std;

// vertex class
struct vertex {
	vertex(const std::string& lbl); // constructor with label
	vertex(const vertex& v);		// copy constructor
	~vertex(void);					// destructor - nothing to do
	bool visited;					// flag if vertex has been visited
	std::string label;				// variable to store the label
};

// constuctor, initialized label and visited
vertex::vertex(const std::string& lbl) :
	visited(false),
	label(lbl)
{}

// copy constructor
vertex::vertex(const vertex& v) :
	visited(v.visited),
	label(v.label)
{}

// destructor
vertex::~vertex(void) {
}

// to output a vertex in a stream
// the way this works is that in std::cout << v1 << v2 << std::endl;
// std::cout << v1  evaluates to a modified std::cout.
// Then, you continue std::cout << v2 << std::endl; etc.
std::ostream& operator<<(std::ostream& stream, const vertex& v) {
	stream << "(" << v.label << "," << v.visited << ")";
	return stream;
}

// graph class
class graph {
public:
	graph(void);							// constructor
	graph(const graph& other);				// copy constructor
	~graph(void);							// destructor
	// here, we assume an undirected graph and
	// check if both vertices exist. Returns false
	// if either from or to don't exist
	bool addEdge(int from, int to);			// add an edge (from,to)
	// add a vertex and return its ID
	int addVertex(const std::string& label);
	// depth-first traversal, returns sequence of vertices
	std::vector<int> dfs(int start);
	// breadth-first traversal, returns sequence of vertices
	std::vector<int> bfs(int start);
	int get_weight(int i, int j) const;
	size_t nVertices(void) const;
	size_t nEdges(void) const;
	std::vector<std::pair<int, int> > get_edges(void) const;
	std::vector<vertex> get_vertices(void) const;
protected:
	// one vector per vertex, storing adjacent vertices
	// (inner vector). We store all those in another
	// vector (outer vector)
	std::vector< std::vector<int> > adj;
	// vector storing all vertices
	std::vector<vertex> vertex_list;
	// id_counter - may become necessary when deleting vertices
	int id_counter;
	// check if a vertex exists
	bool vertex_exist(int v) const;
};

// largest possible integer
// binary 01111111.11111111.111111111.111111111
const int INFTY = 0x7FFFFFFF;
int graph::get_weight(int i, int j) const {
	if (i >= adj.size()) return INFTY;
	if (j >= adj[i].size()) return INFTY;
	return adj[i][j];
}

size_t graph::nVertices(void) const {
	return vertex_list.size();
}

size_t graph::nEdges(void) const {
	size_t count = 0;
	// "outer vector" : row indices
	for (size_t n = 0; n < adj.size(); n++) {
		// "inner vector" : column indices
		// this is an undirected graph.
		// the adjacency matrix is symmetric.
		// it is sufficient to only look at half the matrix.
		for (size_t k = n + 1; k < adj[n].size(); k++) {
			if (get_weight(n, k) != INFTY) count++;
		}
	}
	return count;
}

std::vector<std::pair<int, int> > graph::get_edges(void) const {
	std::vector<std::pair<int, int> > result;
	size_t count = 0;
	// "outer vector" : row indices
	for (size_t n = 0; n < adj.size(); n++) {
		// "inner vector" : column indices
		// this is an undirected graph.
		// the adjacency matrix is symmetric.
		// it is sufficient to only look at half the matrix.
		for (size_t k = n + 1; k < adj[n].size(); k++) {
			if (get_weight(n, k) != INFTY) {
				result.push_back(std::make_pair(int(n), int(k)));
			}
		}
	}
	return result;
}

// constructor initializes id_counter
graph::graph(void) :
	id_counter(0)
{}

// copy constructor
graph::graph(const graph & other) :
	id_counter(other.id_counter)
{
	// since we're using STL object with assignment
	// support, we can just copy things like here:
	adj = other.adj;
}

// destructor
graph::~graph(void) {
	adj.clear();
}

bool graph::vertex_exist(int v) const {
	// adj contains one row for every vertex.
	// therefore, its size must be larger than v
	// if it contains v.
	return (adj.size() > v);
}

bool graph::addEdge(int from, int to) {
	if (!vertex_exist(from) || !vertex_exist(to)) return false;
	// we assume an undirected graph, the adjacency is symmetric

	// append -1s at the end of adj[from] (inner vector)
	// such that we have an 
	for (size_t n = adj[from].size(); n <= to; n++)
		adj[from].push_back(INFTY);
	// now we know that adj[from] has enough "slots"
	adj[from][to] = 1;

	// repeat for adj[to]: This we need to do only
	// for an undirected graph
	for (size_t n = adj[to].size(); n <= from; n++)
		adj[to].push_back(INFTY);
	adj[to][from] = 1;
	return true;
}

int graph::addVertex(const std::string & label) {
	// get new ID
	int id = id_counter++;
	// add to vertex list
	vertex_list.push_back(vertex(label));
	adj.push_back(std::vector<int>());
	return id;
}

std::vector<int> graph::dfs(int start) {
	std::deque<int> todo;
	std::vector<int> result;
	for (size_t i = 0; i < vertex_list.size(); i++)
		vertex_list[i].visited = false;
	todo.push_back(start);
	vertex_list[start].visited = true;
	while (!todo.empty()) {
		size_t from = todo.back();
		todo.pop_back();
		result.push_back(int(from));
		// adj[0] -> std::vector<int> : (3,4,5)
		for (size_t to = from + 1; to < adj[from].size(); to++) {
			if (adj[from][to] != INFTY) {
				if (!vertex_list[to].visited) {
					todo.push_back(to);
					vertex_list[to].visited = true;
				}
			}
		}
	}
	return result;
}

std::vector<int> graph::bfs(int start) {
	std::deque<int> todo;
	std::vector<int> result;
	for (size_t i = 0; i < vertex_list.size(); i++)
		vertex_list[i].visited = false;
	todo.push_back(start);
	vertex_list[start].visited = true;
	while (!todo.empty()) {
		size_t from = todo.front();
		todo.pop_front();
		result.push_back(int(from));
		// adj[0] -> std::vector<int> : (3,4,5)
		for (size_t to = from + 1; to < adj[from].size(); to++) {
			if (adj[from][to] != INFTY) {
				if (!vertex_list[to].visited) {
					todo.push_back(to);
					vertex_list[to].visited = true;
				}
			}
		}
	}
	return result;
}

std::vector<vertex> graph::get_vertices(void) const {
	return vertex_list;
}

std::ostream& operator<<(std::ostream & stream, const graph & G) {
	std::cout << "The graph has " << G.nVertices() << " vertices." << std::endl;
	auto edges = G.get_edges();
	std::cout << "The graph has " << edges.size() << " edges." << std::endl;
	std::cout << "Edgelist:" << std::endl;
	for (const auto& e : edges) {
		std::cout << "  (" << e.first << "," << e.second << ")" << std::endl;
	}
	std::cout << "Vertexlist:" << std::endl;
	auto vertices = G.get_vertices();
	for (size_t n = 0; n < vertices.size(); n++) {
		int id = int(n);
		std::cout << "  " << vertices[n] << " id=" << id << std::endl;
	}
	return stream;
}


int main(int argc, char** argv) {
	// initialization can be done like this:

	//std::vector<std::string> vtx({ "A","B","C","D","E" });
	std::vector<std::string> vtx({ "A","B","C","D","E","F","G","H","I" });
	graph G;
	std::vector<int> vertex_ids;
	for (const auto& elem : vtx) {
		int id = G.addVertex(elem);
		vertex_ids.push_back(id);
	}
	struct edge {
		edge(int i, int j) : from(i), to(j) {}
		int from, to;
	};
	/*
	std::vector<edge> edges({
		edge(0,1), edge(0,3),
		edge(1,2), edge(1,3), edge(1,4),
		edge(2,4), edge(2,3),
		edge(3,4)
		}); */
	std::vector<edge> edges({
		edge(0,1), edge(0,2), edge(0,3), edge(0,4),
		edge(1,5),
		edge(3,6),
		edge(5,7),
		edge(6,8),
		});
	// TODO 1: Change adjacency to adjacency list
	//         Mind the for loops
	// TODO 2: Reset all visited flags to false (iterate the vertex_list)
	// TODO 3: copy-paste dfs into bfs, 
	// change to correct traversal order.
	// current: adj
	//[0] -> [INFTY,  1  ,INFTY,1]
	//[1] -> [  1  ,INFTY,  1,  1,  1]
	//[2] ...
	//[3]
	//[4]
	// target adj
	//[0] -> [1,2,3]
	//[1] -> [0,2,3,4]
	// ...
	for (const auto& e : edges) G.addEdge(e.from, e.to);
	std::cout << G << std::endl;

	auto resultDFS = G.dfs(0);
	std::cout << "DFS" << std::endl;
	auto vertices = G.get_vertices();
	for (const auto& idDFS : resultDFS) {
		std::cout << vertices[idDFS] << std::endl;
	}

	auto resultBFS = G.bfs(0);
	std::cout << "BFS" << std::endl;
	for (const auto& idBFS : resultBFS) {
		std::cout << vertices[idBFS] << std::endl;
	}

	return 0;
}