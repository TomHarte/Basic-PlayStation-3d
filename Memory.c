//
//  Memory.c
//  
//
//  Created by Thomas Harte on 14/05/2014.
//
//

#include "Memory.h"
#include <malloc.h>

void memSetup()
{
	// TODO: how to pick parameters for this intelligently?
//	InitHeap3();
}

#define kNumberOfLowMemoryCallbackSlots 16

static memLowMemoryCallback lowMemoryCallbacks[kNumberOfLowMemoryCallbackSlots];
static unsigned int numberOfLowMemoryCallbacks = 0;

void memAddLowMemoryCallback(memLowMemoryCallback callback)
{
	if(numberOfLowMemoryCallbacks < kNumberOfLowMemoryCallbackSlots)
	{
		lowMemoryCallbacks[numberOfLowMemoryCallbacks] = callback;
		numberOfLowMemoryCallbacks++;
	}
}

static void memSendLowMemoryWarning(memLowMemoryWarningLevel level)
{
	int callback;
	for(callback = 0; callback < numberOfLowMemoryCallbacks; callback++)
		lowMemoryCallbacks[callback](level);
}

void *memMalloc(size_t s)
{
	/*
		Simple stuff: if alloc fails, try asking everyone to
		release something. If it still fails, try asking them
		to release everything. After that there's no extra help
		we can supply.
	*/
	void *buffer = malloc3(s);
	if(buffer) return buffer;

	memSendLowMemoryWarning(LowMemoryWarningLevelReleaseSomething);

	buffer = malloc3(s);
	if(buffer) return buffer;

	memSendLowMemoryWarning(LowMemoryWarningLevelReleaseEverything);

	return malloc3(s);
}

void memFree(void *buffer)
{
	free3(buffer);
}
