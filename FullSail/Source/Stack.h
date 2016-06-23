/***************************************************************
|	File:		Stack.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 10, 2016
|	Purpose:	Stack<> class used to hold stuff
***************************************************************/
#pragma once


template<typename Stuff>
class Stack
{
	struct Node
	{
		Stuff	data;
		Node*	next;

		Node(Stuff const& _d) : data(_d), next(nullptr) { }
		Node(Stuff const& _d, Node* _n) : data(_d), next(_n) { }
	};


public:
	Stack(void) : top(nullptr), count(0) { }
	~Stack(void);

	void			Push	(Stuff const & _stuff);
	void			Pop		(void);
	unsigned int	Size	(void)					const	{ return count; }
	void			Clear	(void);
	bool			IsEmpty	(void)					const	{ return (top == nullptr) ? true : false; }
	bool			Find	(Stuff const & _stuff);

	Stuff			GetTop	(void)					const	{ return top->data; }


private:

	Node*			top;
	unsigned int	count;
};


template<typename Stuff>
Stack<Stuff>::~Stack(void)
{
	Clear();
}

template<typename Stuff>
void Stack<Stuff>::Push(Stuff const & _stuff)
{
	// Make a new node
	Node* newNode = new Node(_stuff);

	// Fail?
	if (newNode == nullptr)
	{
		return;
	}

	// Fill in new node
	//newNode->data = _stuff;

	// Link node up into stack
	newNode->next = top;

	// Update top
	top = newNode;

	// count++
	count++;
}

template<typename Stuff>
void Stack<Stuff>::Pop(/*Stuff& _stuff*/)
{
	// Empty?
	if (top == nullptr)
		return;

	// Copy top data
	//Stuff _stuff = top->data;

	// Temp top
	Node* temp = top;

	// Move top to top->next
	top = top->next;

	// delete old top
	delete temp;

	// count--
	count--;
}

template<typename Stuff>
void Stack<Stuff>::Clear(void)
{
	for (;;)
	{
		// empty?
		if (top == nullptr)
			break;

		// Keep popping!
		Pop();
	}
}

template<typename Stuff>
bool Stack<Stuff>::Find(Stuff const & _stuff)
{
	// empty?
	if (top == nullptr)
		return false;


	// look for stuff
	Node* iter = top;
	for (;;)
	{
		// end?
		if (iter == nullptr)
			break;

		// found
		if (iter->data == _stuff)
			return true;

		// not found: go to next
		iter = iter->next;
	}

	// not found
	return false;
}

