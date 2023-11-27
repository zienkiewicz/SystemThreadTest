#include <iostream>
#include <Windows.h>

#define MY_DEVICE_TYPE 0x8123
#define MY_IOCTL_FUNCTION CTL_CODE(MY_DEVICE_TYPE, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

int main()
{
	std::cout << "Caller's PID: " << GetCurrentProcessId() << '\n';

	HANDLE hDevice = CreateFile(L"\\\\.\\SystemThreadTest",
		GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (INVALID_HANDLE_VALUE == hDevice) {
		std::cerr << "Failed to open the device!\n";
		return 1;
	}

	BOOL result;
	DWORD dummy;
	result = DeviceIoControl(
		hDevice,
		MY_IOCTL_FUNCTION,
		NULL,
		0,
		NULL,
		0,
		&dummy,
		NULL
	);

	if (!result) {
		std::cerr << "DeviceIoControl error: " << GetLastError();
		CloseHandle(hDevice);
		return 1;
	}

	CloseHandle(hDevice);
	return 0;
}