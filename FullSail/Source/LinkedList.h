/***************************************************************
|	File:		LinkedList.h
|	Author:		Jason N. Bloomfield
|	Created:	March 9, 2016
|	Modified:	March 10, 2016
|	Purpose:	LinkedList<> class used to hold stuff
***************************************************************/
#pragma once

#include <cassert>


template<typename Stuff>
class LinkedList
{
	struct Node
	{
		Stuff	data;
		Node*	prev;
		Node*	next;

		Node(Stuff const& _d) : data(_d), prev(nullptr), next(nullptr) { }
		Node(Stuff const& _d, Node* _p, Node* _n) : data(_d), prev(_p), next(_n) { }
	};


public:
	LinkedList();// : first(nullptr), last(nullptr), count(0) { }
	~LinkedList();

	bool			Push_Front	(Stuff const & _stuff);
	bool			Push_Back	(Stuff const & _stuff);
	bool			Pop			(Stuff const & _stuff);
	bool			Pop_Front	(void);
	bool			Pop_Back	(void);

	unsigned int	Size	(void)					const	{ return count; }
	void			Clear	(void);
	bool			IsEmpty (void)					const;
	int				Find	(Stuff const & _stuff);

	Stuff			Get_Front	(void)				const	{ return first->data; }
	Stuff			Get_Back	(void)				const	{ return last->data; }

	Stuff&			operator[]	(int index);
	Stuff const&	operator[]	(int index) const;

private:

	Node*			first;
	Node*			last;
	unsigned int	count;
};

template<typename Stuff>
LinkedList<Stuff>::LinkedList()
{
	first	= nullptr;
	last	= nullptr;
	count	= 0;
}

template<typename Stuff>
bool LinkedList<Stuff>::IsEmpty() const
{
	return (first == nullptr) ? true : false;
}

template<typename Stuff>
LinkedList<Stuff>::~LinkedList()
{
	Clear();
}

template<typename Stuff>
bool LinkedList<Stuff>::Push_Front(Stuff const & _stuff)
{
	// Make a new node
	Node* newNode = new Node(_stuff, nullptr, first);

	// Fail?
	if (newNode == nullptr)
		return false;

	// not empty?
	first != nullptr
		? first->prev = newNode
		: last = newNode;

	// Update first
	first = newNode;

	// count++
	count++;

	// return
	return true;
}

template<typename Stuff>
bool LinkedList<Stuff>::Push_Back(Stuff const & _stuff)
{
	// Make a new node
	Node* newNode = new Node(_stuff, last, nullptr);

	// Fail?
	if (newNode == nullptr)
		return false;

	// not empty?
	last != nullptr
		? last->next = newNode
		: first = newNode;

	// Update last
	last = newNode;

	// count++
	count++;

	// return
	return true;
}

template<typename Stuff>
bool LinkedList<Stuff>::Pop(Stuff const & _stuff)
{
	// not found
	int index = Find(_stuff);
	if (index == -1)
		return false;

	// found: go to index
	Node* iter = first;
	for (int i = 0; i < index; i++, iter = iter->next);


	// count == 1
	if (first == last)
		first = last = nullptr;

	// iter == first
	else if (iter == first)
	{
		first = first->next;
		first->prev = nullptr;
	}

	// iter == last
	else if (iter == last)
	{
		last = last->prev;
		last->next = nullptr;
	}

	// iter > first && iter < last
	else
	{
		iter->prev->next = iter->next;
		iter->next->prev = iter->prev;
	}


	// delete stuff
	//delete iter;

	// count--
	count--;

	// return
	return true;
}

template<typename Stuff>
bool LinkedList<Stuff>::Pop_Front(void)
{
	return Pop(first->data);
}

template<typename Stuff>
bool LinkedList<Stuff>::Pop_Back(void)
{
	return Pop(last->data);
}

template<typename Stuff>
void LinkedList<Stuff>::Clear(void)
{
	for (;;)
	{
		// empty?
		if (first == nullptr)
			break;

		// Keep popping!
		Pop_Back();
	}
}

template<typename Stuff>
int LinkedList<Stuff>::Find(Stuff const & _stuff)
{
	// empty?
	if (first == nullptr)
		return -1;

	// look for stuff
	Node *iter = first;
	for (unsigned int i = 0; i < count; i++)
	{
		// not found: at end of list
		if (iter == nullptr)
			break;

		// not found: go to next
		if (iter->data != _stuff)
		{
			iter = iter->next;
			continue;
		}

		// found
		return i;
	}

	// not found
	return -1;
}

template <typename Stuff>
Stuff& LinkedList<Stuff>::operator[](int index)
{
	// assert: index check
	if (index < 0 || index >= (int)count)
		assert("LinkedList<>::operator[] - index out of range!");

	// go to index
	Node *iter = first;
	for (int i = 0; i < index; i++, iter = iter->next);
	return iter->data;
}

template <typename Stuff>
Stuff const& LinkedList<Stuff>::operator[](int index) const
{
	// assert: index check
	if (index < 0 || index >= (int)count)
		assert("LinkedList<>::operator[] - index out of range!");

	// go to index
	Node *iter = first;
	for (int i = 0; i < index; i++, iter = iter->next);
	return iter->data;
}
