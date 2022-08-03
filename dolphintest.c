#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/ipc.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
	VIDEO_Init();
	WPAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	printf("\x1b[2;0H");

	int fd = IOS_Open("/dev/dolphin", IPC_OPEN_NONE);
	if (fd < 0) {
		exit(0);
	}

	ioctlv clientVector[1] = { 0 };
	char clientID[] = "1004261560371380234";
	clientVector[0].data = clientID;
	clientVector[0].len = sizeof(clientID);
	int setclient = IOS_Ioctlv(fd, 0x7, 1, 0, clientVector);
	printf("setclient = %i\n", setclient);

	ioctlv statusVector[10] = { 0 };
	char details[] = "Testing";
	char state[] = "Number of cats:";
	char largeImageKey[] = "sand";
	char largeImageText[] = "A sand cat";
	char smallImageKey[] = "fennec";
	char smallImageText[] = "Not a cat";
	int partySize = 4;
	int partyMax = 24;
	//int64_t startTimestamp = time(NULL);
	int64_t endTimestamp = time(NULL) + (2 * 60);
	statusVector[0].data = details;
	statusVector[0].len = sizeof(details);
	statusVector[1].data = state;
	statusVector[1].len = sizeof(state);
	statusVector[2].data = largeImageKey;
	statusVector[2].len = sizeof(largeImageKey);
	statusVector[3].data = largeImageText;
	statusVector[3].len = sizeof(largeImageText);
	statusVector[4].data = smallImageKey;
	statusVector[4].len = sizeof(smallImageKey);
	statusVector[5].data = smallImageText;
	statusVector[5].len = sizeof(smallImageText);
	//statusVector[6].data = &startTimestamp;
	//statusVector[6].len = sizeof(int64_t);
	statusVector[7].data = &endTimestamp;
	statusVector[7].len = sizeof(int64_t);
	statusVector[8].data = &partySize;
	statusVector[8].len = sizeof(int);
	statusVector[9].data = &partyMax;
	statusVector[9].len = sizeof(int);
	int setpresence = IOS_Ioctlv(fd, 0x8, 10, 0, statusVector);
	printf("setpresence = %i\n", setpresence);

	printf("\nPress B on Wiimote 1 to exit\n");

	while(1) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_B ) exit(0);
		VIDEO_WaitVSync();
	}

	return 0;
}
