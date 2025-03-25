#include "solve.h"
#include "minpriorityqueue.h"
#include <iostream>
using namespace std;

class Vertex 
{	
public:
	Vertex(int r, int c)
	{
		row = r;
		col = c;
		marked = false;
		crumbs = nullptr;
		cur_weight = 9999;
	}

	// Corresponding row and column location in maze
	int row;
	int col;
	int cur_weight;
	bool marked;
	Vertex* crumbs;

	// List of neighboring vertices
	// The first parameter contains the
	// Vertex pointer 
	// <><><><><><><><><><><><><><><><><><><><><><><><><>
	// The second contains a weight that is either a 
	// value of 1 OR a value equal to its portal key
	vector<pair<Vertex*, int >> neighs;
};

// this map holds all the Vertex objects
// and assigns them a key based off of
// int values for row, col, and port
unordered_map<int, Vertex*> vertexMap;
unordered_map<int, vector<Vertex*>>portKeys;

/*
	THESE ARE THE HEAPS AND MAPS FOR OUR
	  PRIORITY QUEUEs WHICH ADJUST BASED OFF
	THE PRIORITY OF THE NEIGHBORING VERTEXS
	  AFTER WE RELAX EACH WEIGHTED EDGE.

	RELAXING AN EDGE WILL CAUSE THE QUEUE VALUE
	  TO DECREASE AND THEREFORE AFFECT ITS POSITION
	IN THE MIN-PRIORITY QUEUE.

	vector< pair<T, int> > H; // The heap.
	unordered_map<T, int> I;  // Maps values to their indices in H.
*/
void setNeighbors(int key, int w8t);
void relaxEdges(Vertex* Vert, int w8t,MinPriorityQueue <int> &MinPQ);
string makeTrail(vector<Vertex*>trail, string maze, int row_len);

string makeTrail();

string solve(string maze)
{
	// we want to make this PQueue local to this function call
	MinPriorityQueue <int> MinPQ;
	// first we make the vertexs
	Vertex* start = nullptr;
	Vertex* end = nullptr;
	vertexMap.clear();
	portKeys.clear();
	int rr = 0;
	int max_r = 0;
	int cc = 0;
	int max_c = 0;
	int port_num = 0;
	int key;
	// we first make every space represent a vertex
	for (int i = 0; i < maze.size(); i++)
	{

		if (maze[i] == ' ')
		{
			key = (rr * 100) + cc;
			vertexMap[key] = new Vertex(rr, cc);
			cc += 1;
		}
		else if (maze[i] == '\n')
		{
			max_r = rr;
			rr += 1;
			cc = 0;
		}
		else if(maze[i]=='#')
		{
			cc += 1;
			max_c = cc;
		}
		else
		{
			// if the maze contain a 1-9 we convert maze[i] to
			// an integer and use that to create the unique key
			// for our vertexMap
			port_num = int(maze[i])-48;
			key = (rr * 100) + cc ;
			vertexMap[key] = new Vertex(rr, cc);
			// we push this vertex into a sort of histogram using hashmaps
			// we will iterate through each key and assign them weighted edges after
			// we assign all the normal vertex points
			// unordered_map<int, vector<Vertex*>>portKeys;
			if (portKeys.find(port_num) == portKeys.end())
			{
				portKeys[port_num] = vector<Vertex*>();
			}
			portKeys[port_num].push_back(new Vertex(rr, cc));
			cc += 1;
		}
	}
	// Second we assign the neighbors
	// and create weighted edges due to portals
	// First normal vertex creation
	// We add each vertex to the priority queue as well
	for (auto x : vertexMap)
	{
		key = (x.second->col + (x.second->row) * 100);
		setNeighbors(key,1);
		MinPQ.push(x.first,x.second->cur_weight);
	}
	// Now for portals we use they key value as the weight
	// and iterate through each key's Vertex* vector
	int portal_weight;
	for (auto x : portKeys)
	{
		// for reference
		// unordered_map < int, vector <Vertex*> >portKeys;
		// vector<pair<Vertex*, int >> neighs;
		// we assume there are exactly 2 vertices with matching portal numbers
		portal_weight = x.first;
		int key1 = x.second[0]->col + (x.second[0]->row) * 100;
		int key2 = x.second[1]->col + (x.second[1]->row) * 100;

		// we set the two Vertices as neighbors of each other
		// with a weighted edge of value = portal key
		vertexMap[key1]->neighs.push_back(make_pair(vertexMap[key2], portal_weight));
		vertexMap[key2]->neighs.push_back(make_pair(vertexMap[key1], portal_weight));
	}

	// Third we find the start and end to the maze
	int row = 1;
	int col = 0;
	key = 0;
	// These loops checks for vertex* with keys
	// That match the characters of our maze border
	for (col; col < max_c; col++)
	{
		// this checks the top row
		key = col;
		if (vertexMap.find(key) != vertexMap.end())
		{
			if (start == nullptr)
				start = vertexMap[key];
			else if (end == nullptr)
			{
				end = vertexMap[key];
				break;
			}
		}
		// this checks the bottom row
		key = max_r * 100 + col;
		if (vertexMap.find(key) != vertexMap.end())
		{
			if (start == nullptr)
				start = vertexMap[key];
			else if (end == nullptr)
			{
				end = vertexMap[key];
				break;
			}
		}
	}
	for (row; row < max_r; row++)
	{
		// this checks the leftmost collumn
		key = row * 100;
		if (vertexMap.find(key) != vertexMap.end())
		{
			if (start == nullptr)
				start = vertexMap[key];
			else if (end == nullptr)
			{
				end = vertexMap[key];
				break;
			}
		}

		// this checks the rightmost collumn
		key = row * 100 + (max_c - 1);
		if (vertexMap.find(key) != vertexMap.end())
		{
			if (start == nullptr)
				start = vertexMap[key];
			else if (end == nullptr)
			{
				end = vertexMap[key];
				break;
			}
		}
	}

	// Using out MinPQ we can start do Dijkstra's Algorithm
	int start_key = (start->row)*100 + start->col;
	MinPQ.decrease_key(start_key, 0);
	start->cur_weight = 0;

	// We set our start Vertex* as our first MinPQ item
	// Then we relax the edges and change the weights of its neighbors
	// As we relax the edges breadcrumb pointers are updated
	while (MinPQ.size() != 0)
	{
		int curr_vert_key = MinPQ.front();
		MinPQ.pop();
		Vertex* Vert = vertexMap[curr_vert_key];
		relaxEdges(Vert, Vert->cur_weight,MinPQ);
	}
	// Fifth we fill a vector with all of the index location
	// In our string maze.
	vector <Vertex*> trail;
	Vertex* curr = end;
	string crumb_check = "";
	key = 0;

	while (curr != nullptr)
	{
		key = (curr->row * 100) + curr->col;
		crumb_check += to_string(key) + " ,";
		trail.push_back(curr);
		curr = curr->crumbs;
	}
	// Lastly, we replace all the string values (either blank spaces or portal numbers)
	// with "o" using the index values of all the vertexs in our new vector
	string complete_maze = makeTrail(trail, maze, max_c + 1);
	return complete_maze;
}

void setNeighbors(int key, int w8t)
{
	// we check the 4 possible locations of a neighbor vertex
	// based off of the key parameter we can adjust the row and collumn value
	// to find the 4 possible keys
	int top = key - 100;
	int bot = key + 100;
	int left = key - 1;
	int right = key + 1;
	int edge_weight = w8t;
	Vertex* aptr = vertexMap[key];
	Vertex* bptr;

	// if the any of the neighbor keys exist in our map
	// we can assign them to neighbors
	if (vertexMap.find(left) != vertexMap.end())
	{
		bptr = vertexMap[left];
		aptr->neighs.push_back(make_pair(bptr, edge_weight));
	}
	if (vertexMap.find(bot) != vertexMap.end())
	{
		bptr = vertexMap[bot];
		aptr->neighs.push_back(make_pair(bptr, edge_weight));
	}
	if (vertexMap.find(right) != vertexMap.end())
	{
		bptr = vertexMap[right];
		aptr->neighs.push_back(make_pair(bptr, edge_weight));
	}
	if (vertexMap.find(top) != vertexMap.end())
	{
		bptr = vertexMap[top];
		aptr->neighs.push_back(make_pair(bptr,edge_weight));
	}
	
}
void relaxEdges(Vertex* Vert,int w8t,MinPriorityQueue<int> &MinPQ)
{
	int incoming_weight = w8t;
	for (auto y : Vert->neighs)
	{
		int neighbor_key = (y.first->col + ((y.first->row) * 100));
		if (y.first != Vert->crumbs)
		{
			// this case checks for non zero portal neighbors
			// we allow weight increases of 0 due to portals potentially
			// being equal to 0
			if (y.first->cur_weight == incoming_weight && y.first->marked == true)
				continue;
			else if(y.first->cur_weight >= incoming_weight)//
			{
				Vert->marked = true;
				// we add the weight of the current vertex with
				// the weight of the weighted edge connecting the two
				int new_weight = incoming_weight + y.second;

				// this new weight value of the neighbors and the
				// priority of the vertex in the MinPQ are updated to
				// the new_weight value
				y.first->cur_weight = new_weight;
				MinPQ.decrease_key(neighbor_key, new_weight);

				// a new crumb pointer is assigned to the
				// new shortest weight/route to this vertex
				y.first->crumbs = Vert;
				y.first->marked = true;
			}
		}
	}
}
string makeTrail(vector<Vertex*>trail, string maze, int row_len)
{
	string completed_maze = maze;
	int sol = 0;
	for (int i = 0;i < trail.size();i++)
	{
		sol = trail[i]->col + (trail[i]->row) * row_len;
		completed_maze.replace(sol, 1, "o");
	}
	return completed_maze;
}