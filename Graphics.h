//
//  graphics.h
//  
//
//  Created by Thomas Harte on 08/05/2014.
//
//

/*

	This is my basic wrapper for handling video output and resulting timing.

	Anything that involves talking to the GPU occurs in here.

	At present it supports [only]:

		- 640x480, interlaced output;
		- triple buffered;
		- for filled triangles; and
		- a debugging layer of text on top.

	Triple buffering is at the geometry level as per hardware requirements.
	So 600 kb of VRAM is spent. Plus whatever the default font costs.

*/

#ifndef _Graphics_h
#define _Graphics_h

#define kScreenCentreX		320
#define kScreenCentreY		240

extern void gfxSetup();
extern void gfxBeginFrame();

extern int gfxGetNumFramesPassed();
extern char *gfxDebugString();

extern void gfxAddTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int depth, unsigned char r, unsigned char g, unsigned char b);

// the SDK doesn't implement vsprintf or anything equivalent, so this can't take a format and vararg list, sadly
extern void gfxPrintString(char *string);

#endif
