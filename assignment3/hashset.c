#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0 && numBuckets > 0);
	assert(hashfn && comparefn);

	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->hashFn = hashfn;
	h->compareFn = comparefn;
	h->freeFn = freefn;
	h->count = 0;

	h->buckets = malloc(numBuckets*(sizeof(vector)));
	assert(h->buckets);
	for(int i = 0; i < numBuckets; i++) {
		VectorNew(&h->buckets[i], elemSize, freefn, 1);
	}
}

void HashSetDispose(hashset *h)
{
	for(int i = 0; i < h->numBuckets; i++)
		VectorDispose(&h->buckets[i]);
}

int HashSetCount(const hashset *h)
{
	return h->count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	for(int i = 0; i < h->numBuckets; i++)
		VectorMap(&h->buckets[i], mapfn, auxData);
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr);
	int hash = h->hashFn(elemAddr, h->numBuckets);
	assert(hash >= 0 && hash < h->numBuckets);
	vector* v = &h->buckets[hash];
	int elemIndex = VectorSearch(v, elemAddr, h->compareFn, 0, false);
	if (elemIndex < 0) {
		VectorAppend(v, elemAddr);
		h->count++;
	}
	else {
		VectorReplace(v, elemAddr, elemIndex);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	assert(elemAddr);
	int hash = h->hashFn(elemAddr, h->numBuckets);
	assert(hash >= 0 && hash < h->numBuckets);
	vector* v = &h->buckets[hash];
	int elemIndex = VectorSearch(v, elemAddr, h->compareFn, 0, false);
	return (elemIndex >= 0) ? VectorNth(v, elemIndex) : NULL;
}
