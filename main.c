/* 
 EZINA v3
 By SKGleba
 Code taken from github.com/skgleba/vita-ndp
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

// kNDP plugin - https://github.com/SKGleba/Vita-NDP/tree/master/kndp
#define KNDP_PATH "ux0:app/SKGINARW0/kndp.skprx"

#define ARRAYSIZE(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// partition blknames
#define iosblk "int-lp-ina-os"
#define iblblk "int-lp-ina-sloader"
#define idsblk "int-lp-ign-idstor"
#define emcblk "int-lp-act-entire"


char menu_items[][128] = {" -> Mount inactive os0 as grw0:"," -> Dump inactive os0"," -> Dump inactive slb2"," -> Dump idstorage"," -> Dump whole NAND"," -> Exit"};
const char ddev[5][32] = {"ux0", "ur0", "uma0", "imc0", "xmc0"};
int item_count = 6;
int rapechk = 0;
int smode = 0;
int opmode = 1;
int szode = 2;
int selected = 0;
int sub_selected = 0;
int i;
int pressed;

char utx[256];
char dumpdev[256];
char rmdev[128];

typedef struct {
	uint32_t off;
	uint32_t sz;
	uint8_t code;
	uint8_t type;
	uint8_t active;
	uint32_t flags;
	uint16_t unk;
} __attribute__((packed)) partition_t;

typedef struct {
	char magic[0x20];
	uint32_t version;
	uint32_t device_size;
	char unk1[0x28];
	partition_t partitions[0x10];
	char unk2[0x5e];
	char unk3[0x10 * 4];
	uint16_t sig;
} __attribute__((packed)) master_block_t;

typedef struct {
	char unk1[0x20]; //shh too lazy
	uint32_t device_size;
	char unk2[0x1C]; // IDK, device specific?
} __attribute__((packed)) master_block_t_crop16;

typedef struct {
	char unk1[0x40]; //shh too lazy
	char flags1[0x8]; // flags?
	uint32_t device_size;
	char flags2[0x4]; // flags?
} __attribute__((packed)) master_block_t_cropex;



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


int fpsz(master_block_t *master, uint8_t wpc) {
	int prsz;
	for (size_t i = 0; i < ARRAYSIZE(master->partitions); ++i) {
		partition_t *p = &master->partitions[i];
		if (p->code == wpc)
			prsz = i;}
	return prsz;
}

int chknorape(){
sceKernelDelayThread(0.35 * 1000 * 1000);
SceCtrlData pad;
rapechk = 1;
psvDebugScreenPrintf("\nPress CROSS to continue or CIRCLE to abort...\n\n");
while (rapechk == 1){
sceCtrlPeekBufferPositive(0, &pad, 1);
if (pad.buttons == SCE_CTRL_CROSS) {rapechk = 0; return 0;}
if (pad.buttons == SCE_CTRL_CIRCLE) {rapechk = 0; return 1;}}}

void select_menu(){
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	psvDebugScreenPrintf("                          EZINA v3                               \n");
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

int ndpWorkDevBRBasedFull(const char* inp, const char* oup, int type){
int cdevsz;
psvDebugScreenPrintf("Preparing ( %s )...\n", oup);
if (opmode == 1){
sprintf(utx, "sdstor0:%s", inp);
sprintf(dumpdev, "%s:ndp/%s.img", ddev[smode], oup);
if (exists(dumpdev) == 1) sceIoRemove(dumpdev);}
if (opmode == 0){
sprintf(dumpdev, "sdstor0:%s", inp);
sprintf(utx, "%s:ndp_f/%s.img", ddev[smode], oup);}
if (exists("ur0:temp/tempmbr.x") == 1) sceIoRemove("ur0:temp/tempmbr.x");
kndpWorkDevice(utx, "ur0:temp/tempmbr.x", 1, 0x200, 0, 0, 2);
if (exists("ur0:temp/tempmbr.x") == 1){
if (type == 0){ //MBR
  SceUID fd;
  static master_block_t master;
  fd = sceIoOpen("ur0:temp/tempmbr.x", SCE_O_RDONLY, 0777);
  sceIoRead(fd, &master, sizeof(master));
  cdevsz = master.device_size * 0x200;
  sceIoClose(fd);
}
if (type == 1){ //FAT16 PBR
  SceUID fd;
  static master_block_t_crop16 master;
  fd = sceIoOpen("ur0:temp/tempmbr.x", SCE_O_RDONLY, 0777);
  sceIoRead(fd, &master, sizeof(master));
  cdevsz = master.device_size * 0x200;
  sceIoClose(fd);
}
if (type == 2){ //exFAT PBR
  SceUID fd;
  static master_block_t_cropex master;
  fd = sceIoOpen("ur0:temp/tempmbr.x", SCE_O_RDONLY, 0777);
  sceIoRead(fd, &master, sizeof(master));
  cdevsz = master.device_size * 0x200;
  sceIoClose(fd);
}
sceIoRemove("ur0:temp/tempmbr.x");
psvDebugScreenPrintf("Target IN: %s ( %s )...\n", utx, oup);
psvDebugScreenPrintf("Target OUT: %s ( %s )...\n", dumpdev, oup);
psvDebugScreenPrintf("Device size: 0x%X ( %s )...\n", cdevsz, oup);
int rape = chknorape();
if (rape == 0){
psvDebugScreenPrintf("Working ( %s )...\n", oup);
kndpWorkDevice(utx, dumpdev, opmode, cdevsz, 0, 0, szode);}
}
psvDebugScreenPrintf("Done...\n");
sceKernelDelayThread(1 * 1000 * 1000);}

int ndpWorkDeviceMBRBased(const char* minp, const char* inp, const char* oup, int pcd){
int cdevsz;
psvDebugScreenPrintf("Preparing ( %s )...\n", oup);
if (opmode == 1){
sprintf(utx, "sdstor0:%s", inp);
sprintf(dumpdev, "%s:ndp/%s.img", ddev[smode], oup);
if (exists(dumpdev) == 1) sceIoRemove(dumpdev);}
if (opmode == 0){
sprintf(dumpdev, "sdstor0:%s", inp);
sprintf(utx, "%s:ndp_f/%s.img", ddev[smode], oup);}
sprintf(rmdev, "sdstor0:%s", minp);
if (exists("ur0:temp/tempmbr.x") == 1) sceIoRemove("ur0:temp/tempmbr.x");
kndpWorkDevice(rmdev, "ur0:temp/tempmbr.x", 1, 0x200, 0, 0, 2);
if (exists("ur0:temp/tempmbr.x") == 1){
  SceUID fd;
  static master_block_t master;
  fd = sceIoOpen("ur0:temp/tempmbr.x", SCE_O_RDONLY, 0777);
  sceIoRead(fd, &master, sizeof(master));
  int devszp = fpsz(&master, pcd);
  cdevsz = master.partitions[devszp].sz * 0x200;
  sceIoClose(fd);
sceIoRemove("ur0:temp/tempmbr.x");
psvDebugScreenPrintf("Target IN: %s ( %s )...\n", utx, oup);
psvDebugScreenPrintf("Target OUT: %s ( %s )...\n", dumpdev, oup);
psvDebugScreenPrintf("Device size: 0x%X ( %s )...\n", cdevsz, oup);
int rape = chknorape();
if (rape == 0){
psvDebugScreenPrintf("Working ( %s )...\n", oup);
kndpWorkDevice(utx, dumpdev, opmode, cdevsz, 0, 0, szode);}
}
psvDebugScreenPrintf("Done...\n");
sceKernelDelayThread(1 * 1000 * 1000);}

int grwm(){
SceUID mod_id;
tai_module_args_t argg;
	argg.size = sizeof(argg);
	argg.pid = KERNEL_PID;
	argg.args = 0;
	argg.argp = NULL;
	argg.flags = 0;
	mod_id = taiLoadStartKernelModuleForUser(MOD_PATH, &argg);
sceKernelExitProcess(0);}

int main()
{
	psvDebugScreenInit();
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_GREEN);
	SceCtrlData pad;

SceUID mod_id;
tai_module_args_t argg;
psvDebugScreenPrintf("Preparing...\n");
	argg.size = sizeof(argg);
	argg.pid = KERNEL_PID;
	argg.args = 0;
	argg.argp = NULL;
	argg.flags = 0;
	mod_id = taiLoadStartKernelModuleForUser(KNDP_PATH, &argg);
if (mod_id >= 0) sceAppMgrLoadExec("app0:eboot.bin", NULL, NULL);
	extern int sceIoMkdir(const char *, int);
	sceIoMkdir("ux0:ndp/", 6);
	
	int l = 1;// happens
	
	select_menu();
		
	while (l == 1) {
			sceCtrlPeekBufferPositive(0, &pad, 1);

			if (pad.buttons == SCE_CTRL_CROSS) {
				switch (selected){
					case 0:
							grwm();
					break;
					case 1:
							ndpWorkDeviceMBRBased(emcblk, iosblk, "os0_ina", 0x3);
					break;
					case 2:
							ndpWorkDeviceMBRBased(emcblk, iblblk, "slb2_ina", 0x2);
					break;
					case 3:
							ndpWorkDeviceMBRBased(emcblk, idsblk, "idstorage", 0x1);
					break;
					case 4:
							ndpWorkDevBRBasedFull(emcblk, "emmc", 0);
					break;
					case 5:
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
