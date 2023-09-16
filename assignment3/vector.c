#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	assert(elemSize > 0);
	v->elemSize = elemSize;
	v->logLength = 0;
	v->allocLength = initialAllocation;
	v->elems = malloc(initialAllocation * elemSize);
	assert(v->elems != NULL);

	v->freeFn = freeFn;
}

void VectorDispose(vector *v)
{
	if(v->freeFn) {
		for(int i = 0; i < v->logLength; i++) {
			// casting to (char*) in order to allow pointer arithmetics
			v->freeFn((char*)v->elem + v->elemSize * i);
		}
	}
	free(v->elems);
}

int VectorLength(const vector *v)
{
	return v->logLength;
}

void *VectorNth(const vector *v, int position)
{
	assert(position < 0 && position > v->logLength - 1);
	return (char*)v->elems + v->elemSize * position;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{}

void VectorInsert(vector *v, const void *elemAddr, int position)
{}

void VectorAppend(vector *v, const void *elemAddr)
{}

void VectorDelete(vector *v, int position)
{}

void VectorSort(vector *v, VectorCompareFunction compare)
{}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ return -1; }
