/** \file
 * \brief Declaration and implementation of list-based stacks
 *        (StackPure<E> and Stack<E>).
 *
 * \author Carsten Gutwenger
 *
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 *
 * \par
 * Copyright (C)<br>
 * See README.txt in the root directory of the OGDF installation for details.
 *
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation;
 * see the file LICENSE.txt included in the packaging of this file
 * for details.
 *
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * \par
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/

#pragma once

#include <ogdf/basic/basic.h>
#include <ogdf/basic/memory.h>


namespace ogdf {


//! List-based stacks.
/**
 * @ingroup containers
 *
 * In contrast to Stack<E>, instances of \a StackPure<E> do not store the
 * number of elements contained in the stack.
 *
 * @tparam E is the element type.
 */
template<class E> class StackPure
{
	struct Element {
		Element(Element *pNext, const E &x) : m_next(pNext), m_x(x) { }

		template<class ... Args>
		Element(Element *pNext, Args && ... args) : m_next(pNext), m_x(std::forward<Args>(args)...) { }

		Element *m_next;
		E        m_x;
		OGDF_NEW_DELETE
	};

	Element *m_head;

public:
	//! Constructs an empty stack.
	StackPure() { m_head = nullptr; }

	//! Constructs a stack and pushes the elements in \a initList on it (last element will be on top).
	StackPure(std::initializer_list<E> initList) : m_head(nullptr) {
		for (const E &x : initList)
			push(x);
	}

	//! Constructs a stack that is a copy of \a S.
	StackPure(const StackPure<E> &S)  {
		m_head = nullptr;
		copy(S);
	}

	//! Constructs a stack containing the elements of \a S (move semantics).
	/**
	 * Stack \a S is empty afterwards.
	 */
	StackPure(StackPure<E> &&S) : m_head(S.m_head) {
		S.m_head = nullptr;
	}

	// destruction
	~StackPure() {
		clear();
	}

	//! Returns true iff the stack is empty.
	bool empty() const { return m_head == nullptr; }

	//! Returns a reference to the top element.
	const E &top() const {
		return m_head->m_x;
	}

	//! Returns a reference to the top element.
	E &top() {
		return m_head->m_x;
	}

	//! Assignment operator.
	StackPure<E> &operator=(const StackPure<E> &S) {
		clear();
		copy(S);
		return *this;
	}

	//! Assignment operator (move semantics).
	/**
	 * Stack \a S is empty afterwards.
	 */
	StackPure<E> &operator=(StackPure<E> &&S) {
		clear();
		m_head = S.m_head;
		S.m_head = nullptr;
		return *this;
	}

	//! Adds element \a x as top-most element to the stack.
	void push(const E &x) {
		m_head = OGDF_NEW Element(m_head,x);
	}

	//! Adds a new element at as top-most element to the stack.
	/**
	* The element is constructed in-place with exactly the same arguments \a args as supplied to the function.
	*/
	template<class ... Args>
	void emplace(Args && ... args) {
		m_head = OGDF_NEW Element(m_head, std::forward<Args>(args)...);
	}

	//! Removes the top-most element from the stack and returns it.
	E pop() {
		OGDF_ASSERT(m_head != nullptr)
		Element *pX = m_head;
		m_head = m_head->m_next;
		E x = pX->m_x;
		delete pX;
		return x;
	}

	//! Makes the stack empty.
	void clear() {
		while(m_head) {
			Element *pX = m_head;
			m_head = m_head->m_next;
			delete pX;
		}
	}

	void print(ostream &os, char delim = ' ') const
	{
		Element *pX = m_head;
		if (pX != nullptr) {
			os << pX->m_x;
			for(pX = pX->m_next; pX != nullptr; pX = pX->m_next)
				os << delim << pX->m_x;
		}
	}

private:
	void copy(const StackPure<E> &S) {
		Element **p = &m_head;

		for(Element *q = S.m_head; q != nullptr; q = q->m_next) {
			*p = OGDF_NEW Element(nullptr, q->m_x);
			p = &(*p)->m_next;
		}
	}

	OGDF_NEW_DELETE
}; // class StackPure


//! The parameterized class \a Stack<E> implements list-based stacks
/**
 * @ingroup containers
 *
 * In contrast to StackPure<E>, instances of \a Stack<E> store the
 * number of elements contained in the stack.
 *
 * @tparam E is the element type.
 */
template<class E> class Stack : private StackPure<E>
{
	size_t m_count; //! The number of elements in the list.

public:
	typedef size_t size_type;

	//! Constructs an empty stack.
	Stack() { m_count = 0; }

	//! Constructs a stack and pushes the elements in \a initList on it (last element will be on top).
	Stack(std::initializer_list<E> initList) : StackPure<E>(initList) {
		m_count = initList.size();
	}

	//! Constructs a stack that is a copy of \a S.
	Stack(const Stack<E> &S) : StackPure<E>(S) { m_count = S.m_count; }

	//! Constructs a stack containing the elements of \a S (move semantics).
	/**
	 * Stack \a S is empty afterwards.
	 */
	Stack(Stack<E> &&S) : StackPure<E>(std::move(S)), m_count(S.m_count) {
		S.m_count = 0;
	}

	// destruction
	~Stack() { }

	//! Returns true iff the stack is empty.
	bool empty() const { return StackPure<E>::empty(); }

	//! Returns the number of elements contained in the stack.
	size_t size() const { return m_count; }

	//! Returns a reference to the top element.
	const E &top() const {
		return StackPure<E>::top();
	}

	//! Returns a reference to the top element.
	E &top() {
		return StackPure<E>::top();
	}

	//! Assignment operator.
	Stack<E> &operator=(const Stack<E> &S) {
		StackPure<E>::operator=(S);
		m_count = S.m_count;
		return *this;
	}

	//! Assignment operator (move semantics).
	/**
	 * Stack \a S is empty afterwards.
	 */
	Stack<E> &operator=(Stack<E> &&S) {
		StackPure<E>::operator=(std::move(S));
		m_count = S.m_count;
		S.m_count = 0;
		return *this;
	}

	//! Adds element \a x as top-most element to the stack.
	void push(const E &x) {
		++m_count;
		StackPure<E>::push(x);
	}

	//! Adds a new element at as top-most element to the stack.
	/**
	* The element is constructed in-place with exactly the same arguments \a args as supplied to the function.
	*/
	template<class ... Args>
	void emplace(Args && ... args) {
		++m_count;
		StackPure<E>::emplace(std::forward<Args>(args)...);
	}

	//! Removes the top-most element from the stack and returns it.
	E pop() {
		--m_count;
		return StackPure<E>::pop();
	}

	//! Makes the stack empty.
	void clear() {
		StackPure<E>::clear();
		m_count = 0;
	}

	void print(ostream &os, char delim = ' ') const {
		StackPure<E>::print(os,delim);
	}

	OGDF_NEW_DELETE
}; // class Stack



template<class E>
ostream &operator<<(ostream &os, const StackPure<E> &S)
{
	S.print(os);
	return os;
}


template<class E>
ostream &operator<<(ostream &os, const Stack<E> &S)
{
	S.print(os);
	return os;
}

} // end namespace ogdf
