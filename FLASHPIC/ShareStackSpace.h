#pragma once
#include <windows.h>

template<typename T>
class ShareStackSpace
{
	
public:
	ShareStackSpace(int capcity = 20);
	~ShareStackSpace();
	void ClearStack();
	bool isEmpty();
	bool isFull();
	int StackLength();
	T GetTop();
	void Push(T e);
	void Pop();
	T* elemArray;
	int top;
	int tail;
	int capcity;
private:
	HANDLE m_hSetEventRW;
};

