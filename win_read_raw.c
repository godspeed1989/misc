#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BytesPerSector    512

char Drive[] = "\\\\.\\PHYSICALDRIVE0";
//char Drive[] = "\\\\.\\C:";
BOOL ReadPhysicalSector(LONGLONG SectorStart, ULONG SectorCount, PVOID p)
{
	ULONG nBytes;
	BOOL result = FALSE;
	HANDLE hDeviceHandle = NULL;

	hDeviceHandle = CreateFile(
		Drive,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0, 0
	);

	if(hDeviceHandle)
	{
		LARGE_INTEGER pointer;
		pointer.QuadPart = SectorStart;
		pointer.QuadPart *= BytesPerSector;

		SetFilePointer(hDeviceHandle, pointer.LowPart, &pointer.HighPart, FILE_BEGIN);
		if(ReadFile(hDeviceHandle, p, SectorCount*BytesPerSector, &nBytes, NULL))
			result = TRUE;
		else
			printf("read error %d\n", GetLastError());

		CloseHandle(hDeviceHandle);
	}
	else
	{
		printf("open error %d\n", GetLastError());
	}
	return result;
}
#define Printable(cc) ((cc>='a' && cc<='z')||(cc>='A' && cc<='Z'))
int main()
{
	int n = 2, s, i, j;
	UCHAR data[n*BytesPerSector], *p;
	if (ReadPhysicalSector(0, n, data))
	{
		for (s=0; s<n; s++)
		{
			for (i=1; i<=BytesPerSector; i++)
			{
				p = data + s*BytesPerSector;
				printf("%02x ", p[i-1]);
				if (i % 16 == 0)
				{
					for (j=i-16; j<i; j++)
						printf("%c", Printable(p[j])?p[j]:'.');
					printf("\n");
				}
				else if (i % 8 == 0) printf(" ");
			}
			printf("-----------------------------------------------------------------\n");
		}
	}
	return 0;
}
