/**
 * Fibonacci Queue
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

/**
 * This is basically a Fibonacci heap with an added fast store for retrieving
 * vertices, and decrease their key as needed. Useful for search algorithms (e.g.
 * Dijstra, heuristic, ...).
 */

#ifndef FIBOQUEUE_HPP
#define FIBOQUEUE_HPP

#include "fiboheap.hpp"

#include <unordered_map>
#include <algorithm>

namespace fibonacci_heap {
	namespace fibonacci_queue {
		template<class T>
		class fibonacci_queue : public fibonacci_heap<T> {
			/* atributes */
				private:
					std::unordered_multimap<T, c_node*> fstore;
			/* members */
				public:
					// constructors
						fibonacci_queue() {}
						/*
						Foo(const Foo& other) {}
						Foo(Foo&& other) noexcept {}
						*/
					// destructor
						~fibonacci_queue() noexcept {}
					// operators
						/*
						Foo& operator=(const Foo& other) {}
						Foo& operator=(Foo&& other) noexcept {}
						*/
					// accessors
						auto find(T k) { return fstore.find(k); }
						c_node* findNode(T k) { return find(k)->second; }
					// modifiers
						[[noreturn]] void decrease_key(c_node* x, int k) {
							fstore.erase(find(static_cast<T>(*reinterpret_cast<T*>(&x->key))));
							fstore.emplace(k, x);
							fibonacci_heap<T>::decrease_key(x, k);
						}
						void pop() {
							if(fibonacci_heap<T>::empty())
								return;
							c_node* x = fibonacci_heap<T>::extract_min();
							auto range = fstore.equal_range(static_cast<T>(*reinterpret_cast<T*>(&x->key)));
							auto mit = find_if(range.first, range.second,
								[x](const auto& element) {
									return element.second == x;
								}
							);
							if(mit != range.second)
								fstore.erase(mit);
							else
								std::cerr << "[Error]: key " << x->key << " cannot be found in FiboQueue fast store" << std::endl;
							delete x;
						}
						c_node* push(const T& k, void* pl) {
							auto x = fibonacci_heap<T>::push(k, pl);
							fstore.emplace(k, x);
							return x;
						}
						c_node* push(const T& k) { return push(k, nullptr); }
						c_node* push(T&& k, void* pl) {
							auto x = fibonacci_heap<T>::push(k, pl);
							fstore.emplace(k, x);
							k = nullptr;
							return x;
						}
						c_node* push(T&& k) { return push(k, nullptr); }
		};

	}
}

#endif