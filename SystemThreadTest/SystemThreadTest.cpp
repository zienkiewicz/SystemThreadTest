#include <ntddk.h>

void DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS CreateClose(PDEVICE_OBJECT, PIRP);

extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;

	return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	return;
}

NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return NTSTATUS();
}