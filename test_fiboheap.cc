/**
 * Copyright (c) 2014, Emmanuel Benazera beniz@droidnik.fr, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#include "fiboheap.hpp"
#include "fiboqueue.hpp"

#include <stdlib.h>
#include <cassert>
#include <queue>

using namespace std;

void fill_heaps(fibonacci_heap::fibonacci_heap<int>& fh, priority_queue<int, vector<int>, greater<int>>& pqueue, const unsigned int& n) {
	cout << "fill heaps begin" << endl;
	for(unsigned int i = 0; i < n; ++i) {
		auto r = rand();
		fh.push(r);
		pqueue.push(r);
		// pqueue.push((fh.push(rand()))->key));
	}
	assert(fh.size() == n);
	assert(pqueue.size() == n);
	fh.top();
	cout << "fill heaps end" << endl;
}

bool match_heaps(fibonacci_heap::fibonacci_heap<int>& fh, priority_queue<int, vector<int>, greater<int>>& pqueue) {
	cout << "match heaps begin" << endl;
	while(!pqueue.empty()) {
		auto i1 = pqueue.top(), i2 = fh.top();
		cerr << "i1: " << i1 << " -- i2: " << i2 << endl;
		assert(i1 == i2);
		pqueue.pop();
		fh.pop();
	}
	assert(fh.empty());
	cout << "match heaps end" << endl;
	return true;
}

void fill_queues(fibonacci_heap::fibonacci_queue::fibonacci_queue<int>& fh, priority_queue<int, vector<int>, greater<int>>& pqueue, const unsigned int& n) {
	for(unsigned int i = 0; i < n; ++i) {
		int r = rand();
		fh.push(r);
		pqueue.push(r);
		// pqueue.push((fh.push(rand()))->key));
	}
	assert(fh.size() == n);
	assert(pqueue.size() == n);
}

bool match_queues(fibonacci_heap::fibonacci_queue::fibonacci_queue<int>& fh, priority_queue<int, vector<int>, greater<int>>& pqueue) {
	while(!pqueue.empty()) {
		int i1 = pqueue.top(), i2 = fh.top();
		cerr << "i1: " << i1 << " -- i2: " << i2 << endl;
		assert(i1 == i2);
		pqueue.pop();
		fh.pop();
	}
	assert(fh.empty());
	return true;
}

int main(int argc, char* argv[]) {
	fibonacci_heap::fibonacci_heap<int> fh;
	unsigned int n = 10;
	priority_queue<int, vector<int>, greater<int>> pqueue;

	// srand(time(0));

	fill_heaps(fh, pqueue, n);
	fh.top();
	assert(match_heaps(fh, pqueue));

	fill_heaps(fh, pqueue, n);
	// cerr << "top pqueue: " << pqueue.top() << " -- top fh: " << fh.top() << endl;
	int r = pqueue.top() - 1;
	// cerr << "old val: " << pqueue.top() << " -- new val: " << r << endl;
	pqueue.pop();
	pqueue.push(r);
	make_heap(const_cast<int*>(&pqueue.top()), const_cast<int*>(&pqueue.top()) + pqueue.size(), greater<int>());
	fh.decrease_key(fh.topNode(), r);
	assert(match_heaps(fh, pqueue));

	fibonacci_heap::fibonacci_queue::fibonacci_queue<int> fq;
	fill_queues(fq, pqueue, n);
	match_queues(fq, pqueue);

	fill_queues(fq, pqueue, n);
	r = rand();
	fq.push(r);
	fibonacci_heap::c_node* x = fq.findNode(r);
	assert(x != NULL);
	int nr = r - rand() / 2;
	fq.decrease_key(x, nr);
	pqueue.push(nr);
	match_queues(fq, pqueue);
}
