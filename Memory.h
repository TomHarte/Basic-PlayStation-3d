//
//  Memory.h
//  
//
//  Created by Thomas Harte on 24/05/2014.
//
//

/*

	This file will establish a memory subsystem with low-memory warnings, allowing
	users to allocate greedily and repent only if there's a problem.

	It doesn't function yet (and, therefore, isn't used) as I'm looking for a good
	automatic way to pick arguments to InitHeap.

*/

#ifndef _Memory_h
#define _Memory_h

#include <sys/types.h>

typedef enum
{
	LowMemoryWarningLevelReleaseSomething,
	LowMemoryWarningLevelReleaseEverything
} memLowMemoryWarningLevel;

typedef (* memLowMemoryCallback)(memLowMemoryWarningLevel level);

void memSetup();
void memAddLowMemoryCallback(memLowMemoryCallback callback);

void *memMalloc(size_t s);
void memFree(void *);

// am I likely to want these?
//void *memRealloc(void *, size_t);
//void *memCalloc(size_t n, size_t s);

#endif
