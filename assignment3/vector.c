#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void VectorGrow(vector *v)
{
	v->allocLength *= 2;
	if (v->allocLength == 0) v->allocLength++;
	v->elems = realloc(v->elems, v->allocLength * v->elemSize);
}

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
			v->freeFn(VectorNth(v, i));
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
	assert(!(position < 0) && !(position > v->logLength - 1));
	// casting to (char*) in order to allow pointer arithmetics
	return (char*)v->elems + v->elemSize * position;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	void* toReplace = VectorNth(v, position);
	if(v->freeFn) v->freeFn(toReplace);
	memcpy(toReplace, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(!(position < 0) && !(position > v->logLength));
	if(v->logLength == v->allocLength) VectorGrow(v);

	void* source = (char*)v->elems + v->elemSize * position;
	if(position < v->logLength) {
		void* dest = (char*)v->elems + v->elemSize * (position+1);
		memmove(dest, source, v->elemSize * (v->logLength - position));
	}
	memcpy(source, elemAddr, v->elemSize);
	v->logLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
	VectorInsert(v, elemAddr, v->logLength);
}

void VectorDelete(vector *v, int position)
{
	assert(!(position < 0) && !(position > v->logLength-1));
	if(v->freeFn) v->freeFn(VectorNth(v, position));

	void* dest = VectorNth(v, position);
	void* source = (char*)dest + v->elemSize;
	memmove(dest, source, v->elemSize * (v->logLength - position));
	v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	assert(compare != NULL);
	qsort(v->elems, v->logLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	for(int i = 0; i < v->logLength; i++) {
		mapFn(VectorNth(v, i), auxData);
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
	assert(!(startIndex < 0) && !(startIndex > v->logLength));
	assert(key != NULL && searchFn != NULL);

	if (isSorted) {
		void* result = bsearch(key, VectorNth(v, startIndex), v->logLength-startIndex, v->elemSize, searchFn);
		if (result) return ((char*)result - (char*)v->elems) / v->elemSize;
	}
	else {
		for(int i = startIndex; i < v->logLength-startIndex; i++)
			if(searchFn(VectorNth(v, i), key) == 0) return i;
	}
	return kNotFound;
}
