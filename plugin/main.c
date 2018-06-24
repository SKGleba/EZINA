/*
	INARW plugin v2.0
	By SKGleba
*/
/*
	VitaShell
	Copyright (C) 2015-2017, TheFloW

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <psp2kern/kernel/cpu.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/sysmem.h>
#include <psp2kern/io/fcntl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include <taihen.h>

int module_get_offset(SceUID pid, SceUID modid, int segidx, size_t offset, uintptr_t *addr);

typedef struct {
	const char *dev;
	const char *dev2;
	const char *blkdev;
	const char *blkdev2;
	int id;
} SceIoDevice;

typedef struct {
	int id;
	const char *dev_unix;
	int unk;
	int dev_major;
	int dev_minor;
	const char *dev_filesystem;
	int unk2;
	SceIoDevice *dev;
	int unk3;
	SceIoDevice *dev2;
	int unk4;
	int unk5;
	int unk6;
	int unk7;
} SceIoMountPoint;

static SceIoDevice hybrid = { "grw0:", "exfatgrw", "sdstor0:int-lp-ina-os", "sdstor0:int-lp-ina-os", 0xA00 };

static SceIoMountPoint *(* sceIoFindMountPoint)(int id) = NULL;

static SceIoDevice *ori_dev = NULL, *ori_dev2 = NULL;

int exists(const char* filloc) {
  SceUID fd;
  fd = ksceIoOpen(filloc, SCE_O_RDONLY, 0);
  if (fd < 0) {
ksceIoClose(fd);
return 0;
  }
ksceIoClose(fd);
return 1;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) {
	ksceIoMkdir("ux0:ndp/", 6);
if (exists("ur0:temp/inagrw") == 1) {
// redirect based on theflow's
	tai_module_info_t info;
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceIofilemgr", &info) < 0) return -1;
	module_get_offset(KERNEL_PID, info.modid, 0, 0x138C1, (uintptr_t *)&sceIoFindMountPoint);
	SceIoMountPoint *mount = sceIoFindMountPoint(0xA00);

	if (mount->dev != &hybrid && mount->dev2 != &hybrid) {
		ori_dev = mount->dev;
		ori_dev2 = mount->dev2;
	}

	mount->dev = &hybrid;
	mount->dev2 = &hybrid;
	ksceIoUmount(0xA00, 0, 0, 0);
	ksceIoUmount(0xA00, 1, 0, 0);
	ksceIoMount(0xA00, NULL, 0, 0, 0, 0);
	ksceIoRemove("ur0:temp/inagrw");
}
// dump wae based on zecoxao's nand dumper
if (exists("ur0:temp/dinaos") == 1){

	SceUID fd = ksceIoOpen("sdstor0:int-lp-ina-os", SCE_O_RDONLY, 0777);
	SceUID wfd = ksceIoOpen("ux0:ndp/os0_ina", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 6);
	static char buffer[0x4000];
	unsigned int i = 0;
	for(i=0;i<0x1000000;i=i+0x4000){
		ksceIoRead(fd, buffer, 0x4000);
		ksceIoWrite(wfd, buffer, 0x4000);
	}
	if (fd > 0)
		ksceIoClose(fd);
	if (wfd > 0)
		ksceIoClose(wfd);
	ksceIoRemove("ur0:temp/dinaos");
}
/*
// Used in another project
if (exists("ur0:temp/dactos") == 1){

	SceUID fd = ksceIoOpen("sdstor0:int-lp-act-os", SCE_O_RDONLY, 0777);
	SceUID wfd = ksceIoOpen("ux0:ndp/os0_act", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 6);
	static char buffer[0x4000];
	unsigned int i = 0;
	for(i=0;i<0x1000000;i=i+0x4000){
		ksceIoRead(fd, buffer, 0x4000);
		ksceIoWrite(wfd, buffer, 0x4000);
	}
	if (fd > 0)
		ksceIoClose(fd);
	if (wfd > 0)
		ksceIoClose(wfd);
	ksceIoRemove("ur0:temp/dactos");
}
if (exists("ur0:temp/dignvs") == 1){

	SceUID fd = ksceIoOpen("sdstor0:int-lp-ign-vsh", SCE_O_RDONLY, 0777);
	SceUID wfd = ksceIoOpen("ux0:ndp/vs0", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 6);
	static char buffer[0x4000];
	unsigned int i = 0;
	for(i=0;i<0x10000000;i=i+0x4000){
		ksceIoRead(fd, buffer, 0x4000);
		ksceIoWrite(wfd, buffer, 0x4000);
	}
	if (fd > 0)
		ksceIoClose(fd);
	if (wfd > 0)
		ksceIoClose(wfd);
	ksceIoRemove("ur0:temp/dignvs");
}
*/
if (exists("ur0:temp/dfnand") == 1){
	SceUID fd = ksceIoOpen("sdstor0:int-lp-act-entire", SCE_O_RDONLY, 0777);
	SceUID wfd = ksceIoOpen("ux0:ndp/nand.bin", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 6);
	static char buffer[0x4000];
	unsigned int i = 0;
	for(i=0;i<0xE1400000;i=i+0x4000){
		ksceIoRead(fd, buffer, 0x4000);
		ksceIoWrite(wfd, buffer, 0x4000);
	}
	if (fd > 0)
		ksceIoClose(fd);
	if (wfd > 0)
		ksceIoClose(wfd);
	ksceIoRemove("ur0:temp/dfnand");
}
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp) {
	return SCE_KERNEL_STOP_SUCCESS;
}
