#include <stdio.h>
#include <stdlib.h>

#include "heap.h"

////////////////////////////////////////////////////////////////////////////////
// 힙의 내용 출력 (for debugging)
void heapPrint( HEAP *heap)
{
	int i;
	tNode **p = heap->heapArr;
	int last = heap->last;
	
	for( i = 0; i <= last; i++)
	{
		printf("[%d]%c(%6d)\n", i, p[i]->data, p[i]->freq);
	}
	printf( "\n");
}

////////////////////////////////////////////////////////////////////////////////
// 힙 생성
// 배열을 위한 메모리 할당 (capacity)
// last = -1
HEAP *heapCreate( int capacity)
{
	HEAP *heap;
	
	heap = (HEAP *)malloc( sizeof(HEAP));
	if (!heap) return 0;

	heap->last = -1;
	heap->capacity = capacity;
	heap->heapArr = (tNode **)malloc( sizeof(tNode *) * capacity);
	if (heap->heapArr == 0)
	{
		fprintf( stderr, "Error : not enough memory!\n");
		free( heap);
		return 0;
	}
	return heap;
}

////////////////////////////////////////////////////////////////////////////////
// 최소힙 유지
static void _reheapUp( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	int parent;
	
	while(1)
	{
		if (index == 0) return; // root node
		
		parent = (index - 1) / 2;
		
		if (arr[index]->freq < arr[parent]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;
			
			index = parent;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 힙에 원소 삽입
// _reheapUp 함수 호출
int heapInsert( HEAP *heap, tNode *data)
{
	if (heap->last == heap->capacity - 1)
		return 0;
	
	(heap->last)++;
	(heap->heapArr)[heap->last] = data;
	
	_reheapUp( heap, heap->last);
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// 최소힙 유지
static void _reheapDown( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	tNode *leftData;
	tNode *rightData;
	int noright = 0;
	int largeindex; // index of left or right child with large key
	
	while(1)
	{
		if ((index * 2 + 1) > heap->last) return; // leaf node (there is no left subtree)
		
		leftData = arr[index * 2 + 1];
		if (index * 2 + 2 <= heap->last) rightData = arr[index * 2 + 2];
		else noright = 1;
		
		if (noright || leftData->freq < rightData->freq) largeindex = index * 2 + 1; // left child
		else largeindex = index * 2 + 2; // right child
		
		if (arr[index]->freq > arr[largeindex]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[largeindex];
			arr[largeindex] = temp;
			
			index = largeindex;
			
			noright = 0;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 최소값 제거
// _reheapDown 함수 호출
tNode *heapDelete( HEAP *heap)
{
	if (heap->last == -1) return 0; // empty heap
	
	tNode *data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	
	(heap->last)--;
	
	_reheapDown( heap, 0);
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////
// 힙 메모리 해제
void heapDestroy( HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}


