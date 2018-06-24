/* 
 EZINA v2.0
 By SKGleba
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <taihen.h>
#include "graphics.h"
#define MOD_PATH "ux0:app/SKGINARW0/sce_sys/dic0.png"
/*
// Used in another project
char menu_items[][100] = {" -> Mount inactive os0 as grw0:"," -> Dump inactive os0"," -> Dump active os0"," -> Dump vs0"," -> Dump whole NAND"," -> Exit"};
int item_count = 6;
*/
// In inarw/ezina we use this:
char menu_items[][50] = {" -> Mount inactive os0 as grw0:"," -> Dump inactive os0"," -> Dump whole NAND"," -> Exit"};
int item_count = 4;

int selected = 0;
int sub_selected = 0;
int i;
int pressed;

int filecopy(const char *from, const char *to) {
	long plugin_size;
	FILE *fp = fopen(from,"rb");

	fseek(fp, 0, SEEK_END);
	plugin_size = ftell(fp);
	rewind(fp);

	char* plugin_buffer = (char*) malloc(sizeof(char) * plugin_size);
	fread(plugin_buffer, sizeof(char), (size_t)plugin_size, fp);

	FILE *pFile = fopen(to, "wb");
	
	for (int i = 0; i < plugin_size; ++i) {
			fputc(plugin_buffer[i], pFile);
	}
   
	fclose(fp);
	fclose(pFile);
	return 1;
}

int exists(const char *fname) {
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void select_menu(){
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	psvDebugScreenPrintf("                       EZINA v2 fw3.60                           \n");
	psvDebugScreenPrintf("                         By SKGleba                              \n");
			psvDebugScreenSetFgColor(COLOR_RED);
	for(i = 0; i < item_count; i++){
		if(selected==i){
			psvDebugScreenSetFgColor(COLOR_GREEN);
		}
		
		psvDebugScreenPrintf("%s\n", menu_items[i]);
		psvDebugScreenSetFgColor(COLOR_RED);
	}
	
	psvDebugScreenSetFgColor(COLOR_GREEN);
}

int callkp(const char *cfgn) {
SceUID mod_id;
tai_module_args_t argg;
psvDebugScreenPrintf("Preparing...\n");
filecopy(MOD_PATH, cfgn);
psvDebugScreenPrintf("Working...\n");
	argg.size = sizeof(argg);
	argg.pid = KERNEL_PID;
	argg.args = 0;
	argg.argp = NULL;
	argg.flags = 0;
	mod_id = taiLoadStartKernelModuleForUser(MOD_PATH, &argg);
if (mod_id < 0){ psvDebugScreenPrintf("KERNEL ERROR!\n"); sceKernelDelayThread(5 * 1000 * 1000); sceKernelExitProcess(0);}
if (exists(cfgn) == 1){ psvDebugScreenPrintf("FAILED: WEIRD ADRESS\n"); sceKernelDelayThread(5 * 1000 * 1000); sceKernelExitProcess(0);}
psvDebugScreenPrintf("Success...\n");
psvDebugScreenPrintf("Stopping kmodule...\n");
		argg.size = sizeof(argg);
		argg.pid = KERNEL_PID;
		argg.args = 0;
		argg.argp = NULL;
		argg.flags = 0;
		taiStopUnloadKernelModuleForUser(mod_id, &argg, NULL, NULL);
psvDebugScreenPrintf("Success...\n");
return 1;
}

int main()
{
	psvDebugScreenInit();
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	SceCtrlData pad;
	
	int l = 1;// happens
	
	select_menu();
		
	while (l == 1) {
			sceCtrlPeekBufferPositive(0, &pad, 1);

			if (pad.buttons == SCE_CTRL_CROSS) {
				switch (selected){
/*
// Used in another project
					case 0:
							callkp("ur0:temp/inagrw");
					break;
					case 1:
							callkp("ur0:temp/dinaos");
					break;
					case 2:
							callkp("ur0:temp/dactos");
					break;
					case 3:
							callkp("ur0:temp/dignvs");
					break;
					case 4:
							callkp("ur0:temp/dfnand");
					break;
					case 5:
								sceKernelExitProcess(0);
					break;
*/
// In inarw/ezina we use this:
					case 0:
							callkp("ur0:temp/inagrw");
					break;
					case 1:
							callkp("ur0:temp/dinaos");
					break;
					case 2:
							callkp("ur0:temp/dfnand");
					break;
					case 4:
								sceKernelExitProcess(0);
					break;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			if (pad.buttons == SCE_CTRL_START) {
				sceKernelExitProcess(0);
			}
			
			if (pad.buttons == SCE_CTRL_UP) {
				if(selected!=0){
					selected--;
					sub_selected = 0;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			if (pad.buttons == SCE_CTRL_DOWN) {
				if(selected+1<item_count){
					selected++;
					sub_selected = 0;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			
			if (pad.buttons == SCE_CTRL_LEFT) {
				if(sub_selected!=0){
					sub_selected--;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
	}
	
	sceKernelDelayThread(10 * 1000 * 1000);
	sceKernelExitProcess(0);
    return 0;
}
