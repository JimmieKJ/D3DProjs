#include "ShareStackSpace.h"  
#include <iostream>  

using namespace std;


template<typename T>
ShareStackSpace<T>::ShareStackSpace(int capcity)
{
	if (capcity < 0)
	{
		//MessageBox(NULL, TEXT("the capcity is illegal"), TEXT(""), MB_OK);
		return;
	}
	this->capcity = capcity;
	top = 0;
	tail = 0;
	elemArray = new T[capcity];
	m_hSetEventRW = CreateEvent(NULL, FALSE, FALSE, NULL);
}

template<typename T>
ShareStackSpace<T>::~ShareStackSpace()
{
	delete[] elemArray;
}

template<typename T>
void ShareStackSpace<T>::ClearStack()
{
	top = 0;
}

template<typename T>
bool ShareStackSpace<T>::isEmpty()
{
	return (top == 0);
}

template<typename T>
bool ShareStackSpace<T>::isFull()
{
	return (top == capcity);
}

template<typename T>
int ShareStackSpace<T>::StackLength()
{
	return top;
}

template<typename T>
T ShareStackSpace<T>::GetTop()
{
	if (isEmpty())
	{
		//MessageBox(NULL, TEXT("the stack is empty"), TEXT(""), MB_OK);
		return 0;
	}
	return elemArray[top - 1];
}

template<typename T>
void ShareStackSpace<T>::Push(T e)
{
	if (isFull())
	{
		//cout << "The stack is full" << endl;
		return;
	}

	elemArray[top] = e;
	top++;
	SetEvent(m_hSetEventRW);
}

template<typename T>
void ShareStackSpace<T>::Pop()
{
	if (isEmpty())
	{
		//MessageBox(NULL, TEXT("the stack is empty"), TEXT(""), MB_OK);
		return;
	}

	top--;
	SetEvent(m_hSetEventRW);
}