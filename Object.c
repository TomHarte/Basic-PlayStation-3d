//
//  Object.c
//  
//
//  Created by Thomas Harte on 14/05/2014.
//
//

#include "Object.h"
#include "Graphics.h"

void objectRotate(Object *object, SVECTOR *r)
{
	MATRIX rotationMatrix, preNormalMatrix;
	RotMatrix(r, &rotationMatrix);

	MulMatrix0(&object->orientation, &rotationMatrix, &preNormalMatrix);
	MatrixNormal_0(&preNormalMatrix, &object->orientation);
}

void objectTranslate(Object *object, VECTOR *vector)
{
	VECTOR translation;
	ApplyMatrixLV(&object->orientation, vector, &translation);

	object->location.vx += translation.vx;
	object->location.vy += translation.vy;
	object->location.vz += translation.vz;
}

static Object *cameraObject;
static MATRIX cameraMatrix, combinationMatrix;

void setCameraObject(Object *object)
{
	// we're taking the matrix as orthonormal, so we can transpose to
	// get the inverse of the rotational part
	TransposeMatrix(&object->orientation, &cameraMatrix);

	// we'll need the location for later...
	cameraObject = object;
}

void setDrawingObject(Object *object)
{
	// work out the translational part by getting the difference in
	// position of the two things and then putting that through
	// the camera matrix alone
	VECTOR offset =
	{
		.vx = object->location.vx - cameraObject->location.vx,
		.vy = object->location.vy - cameraObject->location.vy,
		.vz = object->location.vz - cameraObject->location.vz
	};
	VECTOR transformedOffset;
	ApplyMatrixLV(&cameraMatrix, &offset, &transformedOffset);

	// work out the rotational part
	MulMatrix0(&cameraMatrix, &object->orientation, &combinationMatrix);

	// copy in the translation (we could probably avoid this with
	// creative casting?)
	combinationMatrix.t[0] = transformedOffset.vx >> 12;
	combinationMatrix.t[1] = transformedOffset.vy >> 12;
	combinationMatrix.t[2] = transformedOffset.vz >> 12;

	// post this thing
	SetRotMatrix(&combinationMatrix);
	SetTransMatrix(&combinationMatrix);
}

void objectDraw(Object *object)
{
	// establish the appropriate matrices
	setDrawingObject(object);

	// do the broad-phase clip
	if(combinationMatrix.t[2] < - object->model->radiusTimesRootTwo) return;
	if(combinationMatrix.t[2] > 0)
	{
		if(combinationMatrix.t[0] < - combinationMatrix.t[2] - object->model->radiusTimesRootTwo) return;
		if(combinationMatrix.t[0] > combinationMatrix.t[2] + object->model->radiusTimesRootTwo) return;
		if(combinationMatrix.t[1] < - combinationMatrix.t[2] - object->model->radiusTimesRootTwo) return;
		if(combinationMatrix.t[1] > combinationMatrix.t[2] + object->model->radiusTimesRootTwo) return;
	}
	else
	{
		if(combinationMatrix.t[0] < combinationMatrix.t[2] - object->model->radiusTimesRootTwo) return;
		if(combinationMatrix.t[0] > - combinationMatrix.t[2] + object->model->radiusTimesRootTwo) return;
		if(combinationMatrix.t[1] < combinationMatrix.t[2] - object->model->radiusTimesRootTwo) return;
		if(combinationMatrix.t[1] > - combinationMatrix.t[2] + object->model->radiusTimesRootTwo) return;
	}

	// determine whether to draw clipped or otherwise;
	// clipping incurs quite a lot of penalty on reverse
	// face work so we allow the normal pixel clip to organise
	// things unless the model is sufficiently close that potential
	// z plane clipping or general overflow become concerns
	if(combinationMatrix.t[2] < object->model->radius * 2)
	{
		mdlDrawClip(object->model, combinationMatrix.t[2]);
	}
	else
	{
		mdlDrawNoClip(object->model, combinationMatrix.t[2]);
	}
}

void objectSetup(Object *object)
{
	object->orientation.m[0][0] =
	object->orientation.m[1][1] =
	object->orientation.m[2][2] = 4096;

	object->orientation.m[0][1] = object->orientation.m[0][2] =
	object->orientation.m[1][0] = object->orientation.m[1][2] =
	object->orientation.m[2][0] = object->orientation.m[2][1] = 0;

	object->location.vx = object->location.vy = object->location.vz = 0;
}

