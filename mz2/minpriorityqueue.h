
#ifndef MINPRIORITYQUEUE_H
#define MINPRIORITYQUEUE_H

#include <unordered_map>
#include <vector>
#include <utility> // Has pair and swap

using namespace std;

template <typename T>
class MinPriorityQueue
{
	// For the mandatory running times below:
	//
	// n is the number of elements in the MinPriorityQueue.
	//
	// Assume that the operations of unordered_map are O(1) time 
	// (they are average case, but not worst-case).

public:
	// Creates an empty MinPriorityQueue
	MinPriorityQueue()
	{
		H.clear();
		I.clear();
	}

	// Returns the number of elements in the MinPriorityQueue.
	//
	// Must run in O(1) time. Returns the size of Vec
	int size()
	{
		return H.size();
	}

	// Pushes a new value x with priority p
	// into the MinPriorityQueue.
	//
	// Must run in O(log(n)) time.		 
	void push(T x, int p)
	{
		H.push_back(make_pair(x, p));
		int new_index = size() - 1;
		I[x] = new_index;
		bubble_up(new_index);
	}

	// Returns the value at the front of the MinPriorityQueue.
	// Undefined behavior if the MinPriorityQueue is empty.
	// 
	// Must run in O(1) time.
	T front()
	{
		if (!H.empty())
			return H[0].first;
	}

	// Removes the value at the front of the MinPriorityQueue.
	// Undefined behavior if the MinPriorityQueue is empty.
	//
	// Must run in O(log(n)) time. 
	void pop()
	{
		if (H.empty())
			return;
		I.erase(H[0].first);
		swap(H[0], H[H.size() - 1]);
		H.pop_back();
		bubble_down(0);

	}

	// If x is in the MinPriorityQueue 
	// with current priority at least new_p,  
	// then changes the priority of x to new_p.
	// Undefined behavior otherwise.
	//
	// Must run in O(log(n)) time. 
	void decrease_key(T x, int new_p)
	{
		if (I.find(x) == I.end())
			return;

		int old_loc = I[x];
		if (H[old_loc].second > new_p)
		{
			H[old_loc].second = new_p;
			bubble_up(old_loc);
		}
	}

	// Helper Functions
	int parent(int x)
	{
		return (x - 1) / 2;
	}
	int left_child(int x)
	{
		return x * 2 + 1;
	}
	int right_child(int x)
	{
		return x * 2 + 2;
	}

	// recursively comapres priorities at parent and child index in H
	// then swap their priority values in I and then their positions in
	// vector H until it can bubble up no longer
	void bubble_up(int child)
	{
		if (child == 0)
			return;

		int p = parent(child);
		if (H[child].second < H[p].second)
		{
			I[H[child].first] = p;
			I[H[p].first] = child;
			swap(H[child], H[p]);
			bubble_up(p);
		}
		else
			return;
	}
	void bubble_down(int parent)
	{
		int left = left_child(parent);
		int right = right_child(parent);
		if (left >= H.size())
			return;
		if (right >= H.size())
		{
			if (H[parent].second > H[left].second)
			{
				I[H[left].first] = parent;
				I[H[parent].first] = left;
				swap(H[parent], H[left]);
				return;
			}
			else
			{
				I[H[parent].first] = parent;
				return;
			}
		}
		int smaller_child = H[left].second < H[right].second ? left : right;
		if (H[parent].second > H[smaller_child].second)
		{
			I[H[smaller_child].first] = parent;
			I[H[parent].first] = smaller_child;
			swap(H[parent], H[smaller_child]);
			bubble_down(smaller_child);
		}
		else
			I[H[parent].first] = parent;
		return;

	}


private:

	vector< pair<T, int> > H; // The heap.

	unordered_map<T, int> I;  // Maps values to their indices in H.
};

#endif 

