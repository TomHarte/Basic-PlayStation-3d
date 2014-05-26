//
//  graphics.c
//  
//
//  Created by Thomas Harte on 08/05/2014.
//
//

/*

	TODO:

		1) switch to dynamic creation and usage of primitives; probably via a linked list of
		available batches;

		2) support other types of geometry — they're just structs and that's why Sony provides
		SetPolyF3 and the rest, so geometry buffer storage doesn't actually need to be typed
		at the time of allocation.

		3) ... come up with suitable macros to deal with the vararg issue and gfxPrintString.

*/

#include "graphics.h"
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

#include <kernel.h>
#include <stdarg.h>

// Display and draw environments
static DISPENV disp;
static DRAWENV draw;
static RECT ClearRect = {0, 0, 640, 480};

#define kOrderingTableLength		8192
#define kMaximumPrimitivesPerFrame	256

struct Frame
{
	// the ordering table for this frame
	u_long orderingTable[kOrderingTableLength];

	// a buffer for building an output string
	char string[1024], *stringPointer;

	// the clear colour
	u_char clearColour[3];

	// a little local heap of primitives; TODO: make this dynamic?
	POLY_F3	primitives[kMaximumPrimitivesPerFrame];
	size_t primitiveWritePointer;
};

static struct Frame frames[3];

static int frameWritePointer = 0;
static int frameReadPointer = 0;

volatile static int vSyncCount;
static int frameCount;


/*

	Basic frame management: handling the triple buffer, mainly but also the frame counter

*/

static void gfxEndOfFrameCallback()
{
	int nextFrameReadPointer;
	static int ode = 0;

	// here's another vsync
	vSyncCount++;

	// cancel any ongoing drawing
	ResetGraph(1);

	// figure out which would be the next table to read from,
	// and set it as the new read table if the write pointer
	// has moved beyond it
	nextFrameReadPointer = (frameReadPointer+1)%3;
	if(frameReadPointer != frameWritePointer)
	{
		frameReadPointer = nextFrameReadPointer;
	}

	// make sure the ODE is correct; this code runs only in interlaced mode
	// so we can skip checking the draw environment
	while(GetODE() == ode);
	ode ^= 1;

	// clear the background, draw any queued geometry, flush the font
	ClearImage(&ClearRect, 0, 0, 0);
	DrawOTag(&frames[frameReadPointer].orderingTable[kOrderingTableLength-1]);

	if(strlen(frames[frameReadPointer].string))
	{
		FntPrint(frames[frameReadPointer].string);
		FntFlush(0);
	}
}

void gfxSetup()
{
	// set video mode appropriately
	if(*(char *)0xbfc7ff52=='E')
	{
		SetVideoMode(MODE_PAL);
	}
	else
	{
		SetVideoMode(MODE_NTSC);
	}

	// reset the GPU
	ResetGraph(0);

	// setup the GTE, to project up to 640x480
	InitGeom();
	SetGeomOffset(kScreenCentreX, kScreenCentreY);	// set the centre of projection
	SetGeomScreen(kScreenCentreX);					// set the number of pixels that equals a distance of 1

	// set appropriate environment values
	SetDefDispEnv(&disp, 0, 0, 640, 480);
	SetDefDrawEnv(&draw, 0, 0, 640, 480);

	// push the environment
	PutDispEnv(&disp);
	PutDrawEnv(&draw);

	// make sure the first ordering table is initially empty
	ClearOTagR(frames[0].orderingTable, kOrderingTableLength);

	// prepare debug font
	FntLoad(960, 0);
	FntOpen(0, 0, 640, 480, 0, 512);

	// prepare our primitives
	{
		int framePointer;
		int primitivePointer;
		for(framePointer = 0; framePointer < 3; framePointer++)
		{
			for(primitivePointer = 0; primitivePointer < kMaximumPrimitivesPerFrame; primitivePointer++)
			{
				SetPolyF3(&frames[framePointer].primitives[primitivePointer]);
			}
		}
	}

	// now we're setup, switch the display on and install the end-of-frame callback
	VSyncCallback(gfxEndOfFrameCallback);
	SetDispMask(1);
}

void gfxBeginFrame()
{
	int needsVSync;
	int nextFrameWritePointer;

	// we'll definitely need to advance the write pointer
	nextFrameWritePointer = (frameWritePointer+1)%3;

	// we don't want endOfFrameCallback to interrupt here but what
	// we'll do in the clear is: check whether we've run up to the
	// read pointer (in which case we'll plan to wait on vsync) and
	// grab and reset the current vsync count
	EnterCriticalSection();
		needsVSync = (nextFrameWritePointer == frameReadPointer);
		frameCount = vSyncCount;
		vSyncCount = 0;
	ExitCriticalSection();

	// do a sync if we needed one, outside the critical section
	if(needsVSync)
		VSync(0);

	frameWritePointer = nextFrameWritePointer;

	// clear the new ordering table, ready to receive geometry
	ClearOTagR(frames[frameWritePointer].orderingTable, kOrderingTableLength);

	// flush the string
	frames[frameWritePointer].stringPointer = frames[frameWritePointer].string;
	frames[frameWritePointer].string[0] = '\0';

	// reset the primitive list
	frames[frameWritePointer].primitiveWritePointer = 0;
}

int gfxGetNumFramesPassed()
{
	return frameCount;
}

/*

	Debugging and reporting info

*/

char *gfxDebugString()
{
	static char string[1000];

	sprintf(string, "%d %d [%d]", frameWritePointer, frameReadPointer, frameCount);
	return string;
}

/*

	Graphics output routines

*/

void gfxPrintString(char *string)
{
	strcpy(frames[frameWritePointer].stringPointer, string);
	frames[frameWritePointer].stringPointer += strlen(string);
}

void gfxAddTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int depth, unsigned char r, unsigned char g, unsigned char b)
{
	// find a suitable POLY_F3
	struct Frame *frame = &frames[frameWritePointer];
	POLY_F3 *primitive = &frame->primitives[frame->primitiveWritePointer];
	frame->primitiveWritePointer++;

	if(frame->primitiveWritePointer >= kMaximumPrimitivesPerFrame) return;

	// fill the POLY_F3
	// these are just macros that stuff the struct; honestly I've
	// no idea why Sony advocated them
	setRGB0(primitive, r, g, b);
	setXY3(primitive, x1, y1, x2, y2, x3, y3);

	// don't allow depth to be out of bounds
	if(depth > kOrderingTableLength-1)
		depth = kOrderingTableLength-1;
	if(depth < 0)
		depth = 0;

	// throw it into the ordering table as instructed
	AddPrim(&frame->orderingTable[depth], primitive);
}
