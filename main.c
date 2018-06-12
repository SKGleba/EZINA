/* 
 EZINA v1.1
 By SKGleba
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <vitasdk.h>
#include <taihen.h>
#include "graphics.h"
#define MOD_PATH "ux0:app/SKGINARW0/sce_sys/dic0.png" //Just to piss out some autistic kid's RE (guess who im thinking about)

void wait_key_press()
{
	SceCtrlData pad;
	psvDebugScreenSetFgColor(COLOR_GREEN);
psvDebugScreenPrintf("Done, press X to exit this app...\n");

	while (1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_CROSS)
			break;
		sceKernelDelayThread(20 * 10);
	}
}
int main()
{
	tai_module_args_t argg;
	psvDebugScreenInit();
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	int ret;
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	psvDebugScreenPrintf("                         By SKGleba                              \n");
			psvDebugScreenSetFgColor(COLOR_CYAN);
						SceUID mod_id;
								psvDebugScreenPrintf("Remounting...\n");
									argg.size = sizeof(argg);
									argg.pid = KERNEL_PID;
									argg.args = 0;
									argg.argp = NULL;
									argg.flags = 0;
									mod_id = taiLoadStartKernelModuleForUser(MOD_PATH, &argg);
								if (mod_id < 0){ psvDebugScreenSetFgColor(COLOR_RED); psvDebugScreenPrintf("ERROR!\n"); sceKernelDelayThread(5 * 1000 * 1000); sceKernelExitProcess(0);}

wait_key_press();
				sceKernelExitProcess(0);
}
