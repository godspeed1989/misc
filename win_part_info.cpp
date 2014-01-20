#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>

#ifndef STORAGE_DEVICE_NUMBER
typedef struct _STORAGE_DEVICE_NUMBER {
  DEVICE_TYPE  DeviceType;
  ULONG  DeviceNumber;
  ULONG  PartitionNumber;
} STORAGE_DEVICE_NUMBER, *PSTORAGE_DEVICE_NUMBER;
#endif

int main(int argc, char* argv[])
{
	char DiskPath[] = "\\\\.\\PHYSICALDRIVE0";
	int res;
	DWORD dwRet;

	HANDLE hVolRead = CreateFile(DiskPath,
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);
	if ( hVolRead == INVALID_HANDLE_VALUE )
	{
		printf("error %u opening the volume for read access -> abort\n", GetLastError());
		return -1;
	}

	/**************************************************************************/
	printf("1. Get Disk Layout...");
	int nDiskBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION) + sizeof(PARTITION_INFORMATION) * 106;
	PDRIVE_LAYOUT_INFORMATION DiskLayout =
		(PDRIVE_LAYOUT_INFORMATION) malloc (nDiskBufferSize);
	res = DeviceIoControl(
		hVolRead,
		IOCTL_DISK_GET_DRIVE_LAYOUT,
		NULL,
		0,
		DiskLayout,
		nDiskBufferSize,
		&dwRet,
		NULL);
	if ( res )
		printf(" OK\n");
	else
		printf(" failed  err=%u\n", GetLastError());

	printf("%20s%32s\n", "offset", "length");
	int count = 0;
	for (int i=0; i < DiskLayout->PartitionCount; i++)
	{
		LONGLONG offset = DiskLayout->PartitionEntry[i].StartingOffset.QuadPart;
		LONGLONG length = DiskLayout->PartitionEntry[i].PartitionLength.QuadPart;
		if (length == 0)
			continue;
		count++;
		printf ( "%d %16I64d(%12I64d)    %16I64d %8I64dM %4I64dG\n",
			DiskLayout->PartitionEntry[i].PartitionNumber,
			offset, offset/512,
			length, length/1024/1024, length/1024/1024/1024
		);
	}
	printf("PartitionCount: %d\n", count);

	/**************************************************************************/
	printf("2. Get Disk Length...");
	GET_LENGTH_INFORMATION LengthInfo;
	res = DeviceIoControl(
		hVolRead,
		IOCTL_DISK_GET_LENGTH_INFO,
		NULL,
		0,
		&LengthInfo,
		sizeof(GET_LENGTH_INFORMATION),
		&dwRet,
		NULL);
	if ( res )
		printf(" OK\n");
	else
		printf(" failed  err=%u\n", GetLastError());

	printf("Disk Size: %16I64d %8I64dM %4I64dG\n",
			LengthInfo.Length.QuadPart,
			LengthInfo.Length.QuadPart/1024/1024,
			LengthInfo.Length.QuadPart/1024/1024/1024
	);

	/**************************************************************************/
	printf("3. Get Disk Number...");
	STORAGE_DEVICE_NUMBER DeviceNumber;
	res = DeviceIoControl(
		hVolRead,
		IOCTL_STORAGE_GET_DEVICE_NUMBER,
		NULL,
		0,
		&DeviceNumber,
		sizeof(STORAGE_DEVICE_NUMBER),
		&dwRet,
		NULL);
	if ( res )
		printf(" OK\n");
	else
		printf(" failed  err=%u\n", GetLastError());

	printf("Disk Number: %u\n", DeviceNumber.DeviceNumber);

	/**************************************************************************/
	printf("4. Get Disk Geometry...");
	DISK_GEOMETRY DiskGeo;
	res = DeviceIoControl(
		hVolRead,
		IOCTL_DISK_GET_DRIVE_GEOMETRY,
		NULL,
		0,
		&DiskGeo,
		sizeof(DISK_GEOMETRY),
		&dwRet,
		NULL);
	if ( res )
		printf(" OK\n");
	else
		printf(" failed  err=%u\n", GetLastError());

	printf("Disk Sector Size: %u\n", DiskGeo.BytesPerSector);
	printf("Disk SectorsPerTrack: %u\n", DiskGeo.SectorsPerTrack);
	printf("Disk TracksPerCylinder : %u\n", DiskGeo.TracksPerCylinder);
	printf("Disk Cylinders : %I64d\n", DiskGeo.Cylinders.QuadPart);

	CloseHandle(hVolRead);
	if ( res )
		return res;
	return 0;
}
