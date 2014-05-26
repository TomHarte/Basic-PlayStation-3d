//
//  Object.h
//  
//
//  Created by Thomas Harte on 14/05/2014.
//
//

#ifndef _Object_h
#define _Object_h

#include "Model.h"
#include <sys/types.h>
#include <libgte.h>

/*

	An object is an instance of a model (as filenames need to be DOS
	compatible, ModelInstance wasn't available). It's expected that
	actual gameplay objects will be layout compatible so as to be
	castable directly to Object, though we'll see.

	So it is:

		- a reference to a model;
		- a location; and
		- an orientation.

*/
typedef struct
{
	Model *model;

	MATRIX orientation;	// the 't' components aren't used; I could reproduce just the
						// 'm' part here and cast creatively but I favoured getting
						// things working before making them slightly obfuscated

	VECTOR location;	// these are taken to have 12 bits beyond the
						// decimal point, so they line up with the values
						// in the orientation
} Object;

/*
	Seeds an object with the identity matrix, located at the origin
*/
void objectSetup(Object *object);

/*
	All mutations below operate in the local
	coordinate space of the object. Think of a
	flight simulator.
*/
void objectRotate(Object *object, SVECTOR *r);
void objectTranslate(Object *object, VECTOR *vector);

/*
	A helper for establishing a scene; the Object is assumed
	to be static from the instance of this call through any
	calls to drawObject that occur this frame
*/
void setCameraObject(Object *object);

/*
	... and here's how you draw an object, from the point of
	view of the current camera object
*/
void objectDraw(Object *object);

#endif
