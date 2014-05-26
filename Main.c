#include <sys/types.h>
#include <libetc.h>

#include "Graphics.h"
#include "Object.h"
#include "Model.h"

/*

	There's no particularly intelligent logic here; it more or less just strings
	together some basic calls to test the other systems.

*/

void logMatrix(MATRIX *matrix)
{
	int i;
	for(i = 0; i < 3; i++)
	{
		char matrixString[100];
		sprintf(matrixString, "\n%d\t%d\t%d\t%d", matrix->m[i][0], matrix->m[i][1], matrix->m[i][2], matrix->t[i]);
		gfxPrintString(matrixString);
	}
	gfxPrintString("\n");
}

#define kNumberShips 20

int main()
{
	int bannerDisplayTime = 500;
	int pitchMomentum = 0, rollMomentum = 0;
	int speed = 0;
	int i;
	Object player, station, ships[kNumberShips];

	// reset GPU and controller
	gfxSetup();
	mdlSetup();
	PadInit(0);

	// establish the objects
	objectSetup(&player);

	objectSetup(&station);
	station.model = &dodec;

	for(i = 0; i <kNumberShips; i++)
	{
		objectSetup(&ships[i]);
		ships[i].model = &cobra;
	}

	// move the player back
	{
		VECTOR translationVector = {.vx = 0, .vy = 0, .vz = 4096 * 1024 * 8};
		SVECTOR rotationVector = {.vx = 0, .vy = 2048, .vz = 0};
		objectTranslate(&player, &translationVector);
		objectRotate(&player, &rotationVector);
	}
	
	// move the ships forward
	for(i = 0; i <kNumberShips; i++)
	{
		VECTOR translationVector = {.vx = (i - 10) * ONE * 5, .vy = 0, .vz = 10 * 256 * 1024 * 8};
		objectTranslate(&ships[i], &translationVector);

		if(i&2)
		{
			SVECTOR rotationVector = {
					.vx = 0,
					.vy = 512,
					.vz = 0};
			objectRotate(&ships[i], &rotationVector);
		}
	}

	// Main loop
	while(1)
	{
		gfxBeginFrame();

		// show the graphics debug info
//		if(bannerDisplayTime > 0)
		{
			bannerDisplayTime -= gfxGetNumFramesPassed();
			gfxPrintString("\n\n\n\n");
			gfxPrintString("        PSXDEV Codeblast 2014 ");
		}
		
//		gfxPrintString(graphicsDebugString());

		setCameraObject(&player);
		objectDraw(&station);
		for(i = 0; i <kNumberShips; i++)
			objectDraw(&ships[i]);

		// perform 'logic', such as it currently is
		{
			int frameMultiplier = gfxGetNumFramesPassed();

			// update the space station — it spins in place
			{
				SVECTOR rotationVector = {.vx = 0, .vy = 0, .vz = frameMultiplier * 2};
				objectRotate(&station, &rotationVector);
			}

			// update the other ship - it moves around in a spiral
			for(i = 0; i <kNumberShips; i++)
			{
				SVECTOR rotationVector = {
						.vx = ((i&1) ? - 1 : 1) *  frameMultiplier * 3 * (i-5),
						.vy = 0,
						.vz = frameMultiplier * 5};
				VECTOR forwardVector = {.vx = 0, .vy = 0, .vz = ONE * 5 }; // * 16 * (2 + (i - 5))
				objectTranslate(&ships[i], &forwardVector);
				objectRotate(&ships[i], &rotationVector);
			}

			{
				u_long padStatus = PadRead(0);

				if(padStatus & PADLdown)
				{
					if(pitchMomentum < 0) pitchMomentum = 0;
					pitchMomentum += 32;
				}

				if(padStatus & PADLup)
				{
					if(pitchMomentum > 0) pitchMomentum = 0;
					pitchMomentum -= 32;
				}
				
				if(padStatus & PADLright)
				{
					if(rollMomentum < 0) rollMomentum = 0;
					rollMomentum += 32;
				}

				if(padStatus & PADLleft)
				{
					if(rollMomentum > 0) rollMomentum = 0;
					rollMomentum -= 32;
				}
				
				if(pitchMomentum > 0)
					pitchMomentum = (pitchMomentum * 250) >> 8;
				else
					pitchMomentum = -((-pitchMomentum * 250) >> 8);

				if(rollMomentum > 0)
					rollMomentum = (rollMomentum * 250) >> 8;
				else
					rollMomentum = -((-rollMomentum * 250) >> 8);

				{
					SVECTOR playerRotation = {.vx = 0, .vy = 0, .vz = 0};

					if(pitchMomentum < 0)
						playerRotation.vx = (pitchMomentum - 64) >> 7;
					else
						playerRotation.vx = (pitchMomentum + 64) >> 7;

					if(rollMomentum < 0)
						playerRotation.vz = (rollMomentum - 64) >> 7;
					else
						playerRotation.vz = (rollMomentum + 64) >> 7;

					if(playerRotation.vx || playerRotation.vz)
						objectRotate(&player, &playerRotation);
				}

				if(padStatus & PADRup)
				{
					speed += 1024;
					if(speed > ONE * 16) speed = ONE * 16;
				}

				if(padStatus & PADRright)
				{
					speed -= 1024;
					if(speed < 0) speed = 0;
				}

				if(speed)
				{
					VECTOR playerVector = {.vx = 0, .vy = 0, .vz = speed};
					objectTranslate(&player, &playerVector);
				}
			}
		}
	}
}
