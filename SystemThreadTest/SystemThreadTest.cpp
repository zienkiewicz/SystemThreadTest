#include <ntddk.h>

#define MY_DEVICE_TYPE 0x8123
#define MY_IOCTL_FUNCTION CTL_CODE(MY_DEVICE_TYPE, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

UNICODE_STRING Name = RTL_CONSTANT_STRING(L"\\Device\\SystemThreadTest");
UNICODE_STRING SymbolicLink = RTL_CONSTANT_STRING(L"\\??\\SystemThreadTest");

void DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS CreateClose(PDEVICE_OBJECT DevicObject, PIRP Irp);
NTSTATUS WorkIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = WorkIoctl;
	
	PDEVICE_OBJECT DeviceObject;
	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&Name,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
	);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create a device object!"));
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicLink, &Name);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create a symbolic link!!"));
		IoDeleteDevice(DeviceObject);
		return status;
	}
	
	KdPrint(("Created a symbolic link and a device object!"));
	return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT DriverObject) {
	IoDeleteSymbolicLink(&SymbolicLink);
	IoDeleteDevice(DriverObject->DeviceObject);
	return;
}

NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return NTSTATUS();
}

VOID ThreadFunction(PVOID StartContext) {
	UNREFERENCED_PARAMETER(StartContext);
	HANDLE id = PsGetCurrentProcessId();
	KdPrint(("SystemThreadTest: PID %lu", id));
	PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS WorkIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	if (stack->Parameters.DeviceIoControl.IoControlCode == MY_IOCTL_FUNCTION) {
		HANDLE a, b;
		(void)PsCreateSystemThread(
			&a,
			THREAD_ALL_ACCESS,
			NULL,
			NtCurrentProcess(),
			NULL,
			ThreadFunction,
			NULL
		);
		ZwClose(a);

		(void)PsCreateSystemThread(
			&b,
			THREAD_ALL_ACCESS,
			NULL,
			NULL,
			NULL,
			ThreadFunction,
			NULL
		);
		ZwClose(b);
	}
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}