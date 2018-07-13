/**
 * Fibonacci Heap
 * Copyright (c) 2014, Emmanuel Benazera beniz@droidnik.fr, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/*
 * @note sizeof(Node) = 136, sizeof(c_node) = 64
 */

#ifndef FIBOHEAP_HPP
#define FIBOHEAP_HPP

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <limits>
#include <iostream>
#include <utility>
#include <vector>

namespace fibonacci_heap {
	extern "C" {
		typedef struct c_node {
			void* key = nullptr, *payload = nullptr;
			bool mark = false;
			c_node* p = nullptr, *left = nullptr, *right = nullptr, *child = nullptr;
			int degree = -1;
		} c_node;
		c_node* create_c_node_on_heap(void* _key, void* _payload) {
			// création de l'élément
			c_node b;
			// initialisation des membres
			b.key = _key;
			b.payload = _payload;
			// allocation mémoire sur la heap
			c_node* c_node_p = (c_node*)malloc(sizeof(c_node));
			// copie des données
			memcpy(c_node_p, &b, sizeof *c_node_p);
			return c_node_p;
		}
	}
}

namespace fibonacci_heap {
	template<class T>
	class fibonacci_heap {
		/* attributes */
			private:
				size_t n;
				c_node* min;
		/* members */
			public:
				// constructors
					fibonacci_heap() : n(0), min(nullptr) {
						int a = -1;
						push(0, &a);
						extract_min();

					}
					fibonacci_heap(const fibonacci_heap& other) : n(other.n), min(other.min) {}
					fibonacci_heap(fibonacci_heap&& other) noexcept : n(other.n), min(other.min) { delete_Nodes(other.min); }
				// destructor
					~fibonacci_heap() noexcept { delete_nodes(min); }
				// operators
					fibonacci_heap& operator=(const fibonacci_heap& other) {
						n = other.n;
						min = other.min;
					}
					fibonacci_heap& operator=(fibonacci_heap&& other) noexcept {
						n = other.n;
						min = other.min;
						delete_nodes(other.min);
					}
				// getters
					bool empty() const noexcept { return n == 0; }
					size_t size() const noexcept { return n; }
				// accessors
					c_node* topNode() const { return minimum(); }
					T top() const { return static_cast<T>(*reinterpret_cast<T*>(&minimum()->key)); }
					[[time_complexity::Θ(1)]] c_node* minimum() const { return min; }
					c_node* extract_min() {
						c_node* z = min;

						if(c_node* x, *next; z != nullptr) {
							if(z->child != nullptr) {
								std::vector<c_node*> childList(z->degree);
								next = x = z->child;

								for_each(childList.begin(), childList.end(),
									[](auto& element) {
									element = element->right;
								}
								);

								for(auto i = 0; i < z->degree; ++i) {
									min->left->right = x = childList.at(i);
									x->left = min->left;
									min->left = x;
									x->right = min;
									x->p = nullptr;
								}
							}
							z->left->right = z->right;
							z->right->left = z->left;
							if(z == z->right)
								min = nullptr;
							else {
								min = z->right;
								consolidate();
							}
							--n;
						}
						return z;
					}
				// modifiers
					void pop() {
						if(empty())
							return;
						if(c_node* x = extract_min();  x)
							delete x;
					}
					[[time_complexity::Θ(1)]] c_node* insert(c_node* x) {
						x->degree = 0;
						x->child = x->p = nullptr;
						x->mark = false;
						if(min == nullptr)
							min = x->left = x->right = x;
						else {
							min->left->right = x;
							x->left = min->left;
							min->left = x;
							x->right = min;
							if(x->key < min->key)
								min = x;
						}
						++n;
						return x;
					}
					[[time_complexity::Θ(1)]] static fibonacci_heap* union_fibheap(fibonacci_heap* H1, fibonacci_heap* H2) {
						fibonacci_heap* H = new fibonacci_heap();
						H->min = H1->min;
						if(H->min != nullptr && H2->min != nullptr) {
							H2->min->left->right = H->min->right;
							H->min->right->left = H2->min->left;
							H->min->right = H2->min;
							H2->min->left = H->min;
						}
						if(H1->min == nullptr || (H2->min != nullptr && H2->min->key < H1->min->key))
							H->min = H2->min;
						H->n = H1->n + H2->n;
						return H;
					}
					[[noreturn]] void cut(c_node* x, c_node* y) {
						if(x->right == x)
							y->child = nullptr;
						else {
							x->right->left = x->left;
							x->left->right = x->right;
							if(y->child == x)
								y->child = x->right;
						}
						--y->degree;
						min->right->left = x;
						x->right = min->right;
						min->right = x;
						x->left = min;
						x->p = nullptr;
						x->mark = false;
					}
					[[noreturn]] void cascading_cut(c_node* y) {
						if(c_node* z = y->p; z != nullptr) {
							if(!y->mark)
								y->mark = true;
							else {
								cut(y, z);
								cascading_cut(z);
							}
						}
					}
					[[noreturn]] void remove_c_node(c_node* x) {
						decrease_key(x, std::numeric_limits<T>::min());
						delete extract_min();
					}
					[[noreturn]] void fib_heap_link(c_node* y, c_node* x) {
						y->left->right = y->right;
						y->right->left = y->left;
						if(x->child != nullptr) {
							x->child->left->right = y;
							y->left = x->child->left;
							x->child->left = y;
							y->right = x->child;
						}
						else
							y->left = y->right = x->child = y;
						y->p = x;
						++x->degree;
						y->mark = false;
					}
					c_node* push(const T& k, void* pl = nullptr) {
						return insert(create_c_node_on_heap(reinterpret_cast<void*>(static_cast<T*>(const_cast<T*>(&k))), pl));
					}
					c_node* push(T&& k, void* pl = nullptr) { return insert(new c_node{ reinterpret_cast<void*>(static_cast<T*>(&k)), pl }); }
					[[using time_complexity:Θ(1), amortized]] void decrease_key(c_node* x, T k) {
						try {
							if(static_cast<T>(*reinterpret_cast<T*>(&x->key)) < k)
								throw std::out_of_range("new key is greater than current key");
						}
						catch(std::out_of_range& e) {
							e.what();
							return;
						}
						x->key = reinterpret_cast<void*>(static_cast<T*>(&k));
						if(c_node* y = x->p; y != nullptr && static_cast<T>(*reinterpret_cast<T*>(&x->key)) < static_cast<T>(*reinterpret_cast<T*>(&y->key))) {
							cut(x, y);
							cascading_cut(y);
						}
						if(x->key < min->key)
							min = x;
					}
			protected:
				[[using time_complexity:O(log n), amortized]] void delete_nodes(c_node* x) {
					if(!x)
						return;
					c_node* cur = x;
					while(true) {
						// cerr << "cur: " << cur << endl << "x: " << x << endl;
						if(cur->left && cur->left != x) {
							// cerr << "\tcur left: " << cur->left << endl;
							c_node* tmp = cur;
							cur = cur->left;
							if(tmp->child)
								delete_nodes(tmp->child);
							delete tmp;
						}
						else {
							if(cur->child)
								delete_nodes(cur->child);
							delete cur;
							break;
						}
					}
				}
				void consolidate() {
					if(0 < n)
						return;

					c_node* w = nullptr, *next = nullptr, *x = nullptr, *y = nullptr;
					unsigned int rootSize = 0;
					auto max_degree = static_cast<int>(floor(log(static_cast<double>(n)) / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2)));

					std::vector<c_node*> A(max_degree);
					fill(A.begin(), A.end(), nullptr);
					next = w = min;
					do {
						++rootSize;
						next = next->right;
					} while(next != w);

					std::vector<c_node*> rootList(rootSize);
					for_each(rootList.begin(), rootList.end(),
						[](auto& element) {
							element = element->right;
						}
					);

					for(const auto& element : rootList) {
						x = w = element;
						auto d = x->degree;
						while(A.at(d) != nullptr) {
							y = A.at(d);
							if(y->key < x->key)
								std::swap(x, y);
							fib_heap_link(y, x);
							A.at(d) = nullptr;
							++d;
						}
						A.at(d) = x;
					}

					min = nullptr;
					for(const auto& element : A) {
						if(element != nullptr) {
							if(min == nullptr)
								min = element->left = element->right = element;
							else {
								min->left->right = element;
								element->left = min->left;
								min->left = element;
								element->right = min;
								if(element->key < min->key)
									min = element;
							}
						}
					}
				}
	};
}

#endif