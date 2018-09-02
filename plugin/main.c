/*
	INARW plugin v3
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

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) {
// redirect based on theflow's
	tai_module_info_t info;
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceIofilemgr", &info) < 0) return -1;
	switch (info.module_nid) {
		case 0x9642948C: // 3.60 retail
			module_get_offset(KERNEL_PID, info.modid, 0, 0x138C1, (uintptr_t *)&sceIoFindMountPoint);
			break;

		case 0xA96ACE9D: // 3.65 retail
		case 0x3347A95F: // 3.67 retail
		case 0x90DA33DE: // 3.68 retail
			module_get_offset(KERNEL_PID, info.modid, 0, 0x182F5, (uintptr_t *)&sceIoFindMountPoint);
			break;

		default:
			return -1;
	}
	SceIoMountPoint *mount = sceIoFindMountPoint(0xA00);
	if (!mount) {
		return -1;
	}
	if (mount->dev != &hybrid && mount->dev2 != &hybrid) {
		ori_dev = mount->dev;
		ori_dev2 = mount->dev2;
	}

	mount->dev = &hybrid;
	mount->dev2 = &hybrid;
	ksceIoUmount(0xA00, 0, 0, 0);
	ksceIoUmount(0xA00, 1, 0, 0);
	ksceIoMount(0xA00, NULL, 0, 0, 0, 0);
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp) {
	return SCE_KERNEL_STOP_SUCCESS;
}
