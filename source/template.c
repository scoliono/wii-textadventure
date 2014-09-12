#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
int dragonHealth = 10;
int playerHealth = 10;

void close()
{
	printf("\nThanks for playing this game! Press B to exit.");
	while (1)
	{
		WPAD_ScanPads();
		u32 input = WPAD_ButtonsDown(0);
		if (input & WPAD_BUTTON_B)
		{
			exit(0);
		}
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();
	
	// This function initialises the attached controllers
	WPAD_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");
	

	printf("While you are taking a walk in the park, a hungry dragon the size of Godzilla approaches you.\nQuick! What do you do?\nPress A to fight! Press B to flee!");

	while(1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME )
		{
			printf("\nQuitting...");
			exit(0);
		}
		
		switch (pressed)
		{
			case WPAD_BUTTON_A:
			{
				srand(time(NULL));
				int randNum = rand() % 8 + 1; // picks a random number from 1-8
				if (randNum > 3)
				{
					printf("\nYou swing your sword and do %d damage to the dragon!", randNum);
					dragonHealth -= randNum;
					while (1)
					{
						if (dragonHealth <= 0)
						{
							printf("\nYou defeated the dragon! And continue strolling in the park.");
							close();
						}
						if (pressed & WPAD_BUTTON_A)
						{
							break;
						}
					}
					int randNum2 = rand() % 2 + 1;
					printf("\nThe dragon swipes at you and does %d damage to you!", randNum2);
					playerHealth -= randNum2;
					while (1)
					{
						if (playerHealth <= 0)
						{
							printf("\nOh no! The dragon defeated you!");
							close();
						}
						if (pressed & WPAD_BUTTON_A)
						{
							break;
						}
					}
				}
				break;
			}
			case WPAD_BUTTON_B:
			{
				srand(time(NULL));
				int randNum = rand() % 8 + 1;
				if (randNum > 3)
				{
					printf("\nGot away safely!");
					close();
				}
				else
				{
					printf("\nYou attempt to run away. Ahh! The dragon crushes you with its mighty foot.");
					close();
				}
				break;
			}
			default:
				break;
		}

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}