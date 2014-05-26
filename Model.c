//
//  Model.c
//  
//
//  Created by Thomas Harte on 14/05/2014.
//
//

#include "Model.h"
#include <sys/types.h>
#include "Graphics.h"
#include <libgte.h>

/*typedef struct
{
	SVECTOR normal;
	unsigned int distance;
	unsigned char faceStartingIndex, numberOfFaces;
	unsigned int vertexMask1, vertexMask2;
	unsigned char colour;
} mdlFace;

typedef struct
{
	unsigned char numberOfVertices;
	SVECTOR *vertices;
	unsigned char numberOfFaces;
	mdlFace *faces;
	unsigned char *indexArray;
	short clipRadius;
} mdlModel;*/

/*

	A couple of models follow, cribbed from Bell & Braben's Elite.
	They'll have to go.

*/


#define GFX_COL_GREY_1		0
#define GFX_COL_GREY_2		1
#define GFX_COL_GREY_3		2
#define GFX_COL_GREY_4		3
#define GFX_COL_BLACK		4

#define GFX_COL_BLUE_1		5
#define GFX_COL_BLUE_2		6
#define GFX_COL_BLUE_3		7

#define GFX_COL_DARK_RED	8
#define GFX_COL_RED			9

unsigned char colourTable[10][3] =
{
	{255, 255, 255},
	{192, 192, 192},
	{128, 128, 128},
	{64, 64, 64},
	{32, 32, 32},

	{64, 64, 128},
	{32, 0, 192},
	{32, 32, 255},

	{128, 0, 0},
	{255, 64, 32},
};

/*

	The Dodec space station. From Elite. Not by me.

*/

SVECTOR dodecPoints[] =
{
	{   0,  1500,  1960},
	{ 1430,   460,  1960},
	{  880, -1210,  1960},
	{ -880, -1210,  1960},
	{-1430,   460,  1960},
	{   0,  2430,   460},
	{ 2310,   750,   460},
	{ 1430, -1960,   460},
	{-1430, -1960,   460},
	{-2310,   750,   460},
	{ 1430,  1960,  -460},
	{ 2310,  -750,  -460},
	{   0, -2430,  -460},
	{-2310,  -750,  -460},
	{-1430,  1960,  -460},
	{  880,  1210, -1960},
	{ 1430,  -460, -1960},
	{   0, -1500, -1960},
	{-1430,  -460, -1960},
	{ -880,  1210, -1960},
	{ -160,   320,  1960},
	{ -160,  -320,  1960},
	{  160,   320,  1960},
	{  160,  -320,  1960},
};

struct Face dodecFaces[] =
{
	{GFX_COL_GREY_4,	5, {19, 15, 16, 17, 18}},
	{GFX_COL_BLACK,		4, {22, 20, 21, 23,  0}},
	{GFX_COL_GREY_4,	5, { 3,  2,  1,  0,  4}},
	{GFX_COL_GREY_1,	5, { 6, 10,  5,  0,  1}},
	{GFX_COL_GREY_2,	5, { 7, 11,  6,  1,  2}},
	{GFX_COL_GREY_3,	5, { 8, 12,  7,  2,  3}},
	{GFX_COL_GREY_1,	5, { 9, 13,  8,  3,  4}},
	{GFX_COL_GREY_3,	5, { 5, 14,  9,  4,  0}},
 	{GFX_COL_GREY_1,	5, {15, 19, 14,  5, 10}},
	{GFX_COL_GREY_2,	5, {16, 15, 10,  6, 11}},
	{GFX_COL_GREY_1,	5, {17, 16, 11,  7, 12}},
	{GFX_COL_GREY_3,	5, {18, 17, 12,  8, 13}},
	{GFX_COL_GREY_2,	5, {19, 18, 13,  9, 14}},
};

Model dodec =
{
	dodecPoints, 24,
	dodecFaces, 13,
	2474, 3498,
	1
};

/*

	The Cobra Mk 3. Also not by me.

*/
SVECTOR cobraPoints[] =
{
	{  32,    0,   76},
	{ -32,    0,   76},
	{   0,   26,   24},
	{-120,   -3,   -8},
	{ 120,   -3,   -8},
	{ -88,   16,  -40},
	{  88,   16,  -40},
	{ 128,   -8,  -40},
	{-128,   -8,  -40},
	{   0,   26,  -40},
	{ -32,  -24,  -40},
	{  32,  -24,  -40},
	{ -36,    8,  -40},
	{  -8,   12,  -40},
	{   8,   12,  -40},
	{  36,    8,  -40},
	{  36,  -12,  -40},
	{   8,  -16,  -40},
	{  -8,  -16,  -40},
	{ -36,  -12,  -40},
	{   0,    0,   76},
	{   0,    0,   90},
	{ -80,   -6,  -40},
	{ -80,    6,  -40},
	{ -88,    0,  -40},
	{  80,    6,  -40},
	{  88,    0,  -40},
	{  80,   -6,  -40},
};

struct Face cobraFaces[] =
{
	{GFX_COL_RED,		4, {17, 14, 15, 16}},
	{GFX_COL_RED,		4, {19, 12, 13, 18}},

	{GFX_COL_GREY_2,	3, {1,  0,  2}},
	{GFX_COL_BLUE_1,	3, {5,  1,  2}},
	{GFX_COL_BLUE_1,	3, {2,  0,  6}},
	{GFX_COL_BLUE_3,	3, {3,  1,  5}},
	{GFX_COL_BLUE_3,	3, {6,  0,  4}},

	{GFX_COL_GREY_1,	3, {5,  2,  9}},
	{GFX_COL_GREY_1,	3, {9,  2,  6}},

	{GFX_COL_BLUE_2,	3, {8,  3,  5}},
	{GFX_COL_BLUE_2,	3, {6,  4,  7}},
        
	{GFX_COL_GREY_2,	7, {6,  7, 11, 10, 8, 5, 9}},

	{GFX_COL_GREY_3,	4, {10,  1,  3,  8}},
	{GFX_COL_DARK_RED,	4, {10, 11,  0,  1}},
	{GFX_COL_GREY_3,	4, { 7,  4,  0, 11}},

	{GFX_COL_DARK_RED,  3, {23, 22, 24}},
	{GFX_COL_DARK_RED,  3, {27, 25, 26}},
};

Model cobra =
{
	cobraPoints, 28,
	cobraFaces, 17,
	135, 191,
	1
};

/*

	Actual code follows...

*/

static EVECTOR clipData[16];

void mdlSetup(void)
{
	// the +10s deal with the fact that otherwise integer precision gives
	// raggedy edges around clipped geometry; we'll let the GPU deal with
	// overspill on a per-pixel basis
	InitClip(clipData, (kScreenCentreX*2)+10, (kScreenCentreY*2)+10, kScreenCentreX, 1, 65536);
}

void mdlDrawNoClip(Model *model, long centreDepth)
{
	/*

		Very dense algorithm ahead:
		
			(1) transform and project _everything_
			(ii) determine face visibility in 2d and draw if visible.

	*/

	int i;
	short points[24][2];
	long depths[24];

	// transform all points; I'm not even smart enough to use RotTransPers3
	// yet, though that's partly because it doesn't give back unique depths
	// (not that it matters, probably, as we're in the world of painter's
	// algorithm so what's one more approximation?)
	for(i  = 0; i < model->numberOfVertices; i++)
	{
		long p, flag;
		long distance =
			model->vertices[i].vx*model->vertices[i].vx +
			model->vertices[i].vy*model->vertices[i].vy +
			model->vertices[i].vz*model->vertices[i].vz;
		depths[i] = RotTransPers(&model->vertices[i], (long *)points[i], &p, &flag);
	}
	
	// then, for every face...
	for(i = 0; i < model->numberOfFaces; i++)
	{
		// ... check whether it's forward facing and, if so ...
		struct Face *face = &model->faces[i];
		if(
			NormalClip(
				*((long *)points[face->indices[0]]),
				*((long *)points[face->indices[1]]),
				*((long *)points[face->indices[2]])
			) < 0)
		{
			// ... draw, in effect, as a triangle fan
			int faceI;
			for(faceI = 1; faceI < face->points-1; faceI++)
			{
				gfxAddTriangle(
					points[face->indices[0]][0], points[face->indices[0]][1],
					points[face->indices[faceI]][0], points[face->indices[faceI]][1],
					points[face->indices[faceI+1]][0], points[face->indices[faceI+1]][1],
					model->isConvex ?
						(centreDepth >> 2) :
						AverageZ3(
							depths[face->indices[0]],
							depths[face->indices[faceI]],
							depths[face->indices[faceI+1]]
						),
					colourTable[face->colour][0], colourTable[face->colour][1], colourTable[face->colour][2]);
			}
		}
	}
}

void mdlDrawClip(Model *model, long centreDepth)
{
	/*

		Even worse algorithm than for no-clip drawing:
		
			(1) break every face into its constituent triangles (yes, I'm not smart enough to use quads);
			(ii) clip and project every one of those triangles;
			(iii) then in 2d check every one of the triangles in the resulting [second-level] fan for direction; and
			(iv) draw only the front-facing ones.

		What can I say? I'm hoping to get something done for a competition, with a deadline.

	*/
	int i;
	
	// for all faces ...
	for(i = 0; i < model->numberOfFaces; i++)
	{
		struct Face *face = &model->faces[i];
		int faceI;

		// ... break into a triangle fan, and ...
		for(faceI = 1; faceI < face->points-1; faceI++)
		{
			EVECTOR *outputVertices[16];
			long numberOfClippedVertices;
			int shouldDisplay = 0;

			// ... clip and project each triangle, then ...
			numberOfClippedVertices = Clip3FP(
					&model->vertices[face->indices[0]],
					&model->vertices[face->indices[faceI]],
					&model->vertices[face->indices[faceI+1]],
					outputVertices
				);

			if(numberOfClippedVertices)
			{
				int clippedFaceI;

				// ... draw as a triangle fan all results that aren't back facing.
				//
				// Sony's clipping routine often gives degenerate triangles or even sometimes
				// one that are slighly reverse even when the others are front facing, so this
				// test is needed per face (at least, once you decide you're doing it in 2d)
				for(clippedFaceI = 1; clippedFaceI < numberOfClippedVertices-1; clippedFaceI++)
				{
					if(
						NormalClip(
							*(long *)&outputVertices[0]->sxy,
							*(long *)&outputVertices[clippedFaceI]->sxy,
							*(long *)&outputVertices[clippedFaceI+1]->sxy
						) < 0
					)
					{
						gfxAddTriangle(
							outputVertices[0]->sxy.vx, outputVertices[0]->sxy.vy,
							outputVertices[clippedFaceI]->sxy.vx, outputVertices[clippedFaceI]->sxy.vy,
							outputVertices[clippedFaceI+1]->sxy.vx, outputVertices[clippedFaceI+1]->sxy.vy,
							model->isConvex ?
								(centreDepth >> 2) :
								AverageZ3(
									outputVertices[0]->sxyz.vz,
									outputVertices[clippedFaceI]->sxyz.vz,
									outputVertices[clippedFaceI+1]->sxyz.vz
								),
							colourTable[face->colour][0], colourTable[face->colour][1], colourTable[face->colour][2]);
					}
				}

			}
		}
	}
}
