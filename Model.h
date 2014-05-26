//
//  Model.h
//  
//
//  Created by Thomas Harte on 14/05/2014.
//
//

/*

	A model is a 3d shape. There's not a lot more to it than that.

	This code draws a model, using the currently set rotation and
	translation matrix; it either clips it prior to drawing or else
	lets the GPU apply its per-pixel clip.

	At present it uses wildly inefficient means.

*/

#ifndef _Model_h
#define _Model_h

#include <sys/types.h>
#include <libgte.h>
struct Face
{
	int colour;
	int points;
	int indices[8];
};

typedef struct
{
	SVECTOR *vertices;
	int numberOfVertices;

	struct Face *faces;
	int numberOfFaces;

	long radius, radiusTimesRootTwo;
	int isConvex;
} Model;

extern Model dodec, cobra;

extern void mdlDrawNoClip(Model *model, long centreDepth);
extern void mdlDrawClip(Model *model, long centreDepth);
extern void mdlSetup();

#endif
