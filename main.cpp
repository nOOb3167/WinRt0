// https://gist.github.com/socantre/111644880358c39b4998
// https://blog.xojo.com/2019/07/02/accessing-windows-runtime-winrt/
// https://bytes.com/topic/c/answers/942922-virtual-pointer-accessing-vtable-class-inside-program

// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/RadialController/cpp/DeviceListener.cpp
//   callback registration
//   RETURN_IF_FAILED(_controller->add_ScreenContactContinued(
//       Callback<ITypedEventHandler<RadialController*, RadialControllerScreenContactContinuedEventArgs*>>(this, &DeviceListener::OnScreenContactContinued).Get(),
//       & _screenContactContinuedToken));

// https://www.codeproject.com/script/Articles/ViewDownloads.aspx?aid=338268
//   create COM with no ATL, just C++

// https://learn.microsoft.com/en-us/previous-versions/hh438425(v=vs.85)
//   The ITypedEventHandler<TSender,TArgs> interface inherits from the IUnknown interface. ITypedEventHandler also has these types of members:
//     ITypedEventHandler::Invoke

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <thread>
#include <mutex>
#include <memory>
#include <locale>
#include <codecvt>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <stdint.h>

#include <roapi.h>
#include <windows.h>
#include <wrl.h>

#include <bluetooth_scan.hpp>


namespace wrl = Microsoft::WRL;
namespace wrlw = Microsoft::WRL::Wrappers;


using namespace bt;
using namespace std;


uint16_t uint16FromBluetoothServiceUUID(UUID serviceUUID)
{
	// check that UUID is of the form 0000XXXX-0000-1000-8000-00805F9B34FB and return the XXXX returning FFFF otherwise
	if (serviceUUID.Data1 & 0xFFFF0000 || serviceUUID.Data2 != uuidBluetoothBaseUUID.Data2 || serviceUUID.Data3 != uuidBluetoothBaseUUID.Data3 || memcmp(serviceUUID.Data4, uuidBluetoothBaseUUID.Data4, 8) != 0)
		return 0xFFFF;
	return (uint16_t)serviceUUID.Data1;
}


bool isStandardServiceUUID(UUID serviceUUID)
{
	return uint16FromBluetoothServiceUUID(serviceUUID) != 0xFFFF;
}


void deviceFromBluetoothAddress(uint64_t bluetoothAddress, wrl::ComPtr<IUnknown> *outBluetoothLEDevice, wrl::ComPtr<IUnknown>* outBluetoothLEDevice3)
{
	wrl::ComPtr<IUnknown> bluetoothLEDeviceStatics;
	wrl::ComPtr<IUnknown> bluetoothLEDevice;
	wrl::ComPtr<IUnknown> bluetoothLEDevice3;

	wrl::ComPtr<IUnknown> asyncOperation;

	MCompleted completed;
	wrl::ComPtr<ComHandler_IAsyncOperationCompletedHandler__BluetoothLEDevice__> cb2 = new ComHandler_IAsyncOperationCompletedHandler__BluetoothLEDevice__(
		[&completed]() {
			completed.signal();
		}
	);

	CHK(RoGetActivationFactory(wrlw::HString::MakeReference(L"Windows.Devices.Bluetooth.BluetoothLEDevice").Get(), uuidIBluetoothLEDeviceStatics, &bluetoothLEDeviceStatics));
	CHK(GetVt<zIBluetoothLEDeviceStatics>(bluetoothLEDeviceStatics)->FromBluetoothAddressAsync(bluetoothLEDeviceStatics.Get(), bluetoothAddress, &asyncOperation));
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->Put_Completed(asyncOperation.Get(), cb2.Get()));
	completed.wait();
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->GetResults(asyncOperation.Get(), &bluetoothLEDevice));
	CHK(bluetoothLEDevice->QueryInterface(uuidIBluetoothLEDevice3, &bluetoothLEDevice3));

	*outBluetoothLEDevice = bluetoothLEDevice;
	*outBluetoothLEDevice3 = bluetoothLEDevice3;
}


wrl::ComPtr<IUnknown> gattDeviceServicesResult(wrl::ComPtr<IUnknown> bluetoothLEDevice3)
{
	wrl::ComPtr<IUnknown> gattResult;
	int32_t status;

	wrl::ComPtr<IUnknown> asyncOperation;

	MCompleted completed;
	wrl::ComPtr<ComHandler_IAsyncOperationCompletedHandler__GattDeviceServicesResult_star__> cb3 = new ComHandler_IAsyncOperationCompletedHandler__GattDeviceServicesResult_star__(
		[&completed]() {
			completed.signal();
		}
	);

	CHK(GetVt<zIBluetoothLEDevice3>(bluetoothLEDevice3)->GetGattServicesWithCacheModeAsync(bluetoothLEDevice3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &asyncOperation));

	CHK(ComIsA(uuidIAsyncOperation__GattDeviceServicesResult_star__, asyncOperation));

	CHK(GetVt<zIAsyncOperation>(asyncOperation)->Put_Completed(asyncOperation.Get(), cb3.Get()));

	completed.wait();

	CHK(GetVt<zIAsyncOperation>(asyncOperation)->GetResults(asyncOperation.Get(), &gattResult));

	CHK(GetVt<zIGattDeviceServicesResult>(gattResult)->Status(gattResult.Get(), &status));
	CHK(status == (int32_t)zGattCommunicationStatus::Success ? S_OK : E_FAIL);

	return gattResult;
}


wrl::ComPtr<IUnknown> gattCharacteristicsResult(wrl::ComPtr<IUnknown> gattDeviceService)
{
	wrl::ComPtr<IUnknown> deviceService3;
	CHK(gattDeviceService->QueryInterface(uuidIGattDeviceService3, &deviceService3));

	wrl::ComPtr<IUnknown> asyncOperation;

	MCompleted completed;
	wrl::ComPtr<ComHandler_IAsyncOperationCompletedHandler__GetCharacteristicsResult_star__> cb = new ComHandler_IAsyncOperationCompletedHandler__GetCharacteristicsResult_star__(
		[&completed]() {
			completed.signal();
		}
	);

	CHK(GetVt<zIGattDeviceService3>(deviceService3)->GetCharacteristicsWithCacheModeAsync(deviceService3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &asyncOperation));
	CHK(ComIsA(uuidIAsyncOperation__GetCharacteristicsResult_star__, asyncOperation.Get()));
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->Put_Completed(asyncOperation.Get(), cb.Get()));
	completed.wait();
	wrl::ComPtr<IUnknown> gattCharacteristicResult;
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->GetResults(asyncOperation.Get(), &gattCharacteristicResult));
	int32_t status;
	CHK(GetVt<zIGattCharacteristicsResult>(gattCharacteristicResult)->Status(gattCharacteristicResult.Get(), &status));
	CHK(status == (int32_t)zGattCommunicationStatus::Success ? S_OK : E_FAIL);

	return gattCharacteristicResult;
}


void probe(const ScannedDevice& scannedDevice)
{
	wrl::ComPtr<IUnknown> bluetoothLEDevice;
	wrl::ComPtr<IUnknown> bluetoothLEDevice3;

	wrl::ComPtr<IUnknown> services;
	uint32_t size_serv = 0;
	uint32_t ncopied_serv = 0;
	std::vector<IUnknown*> serv;
	std::vector<UUID> uuid;

	size_t FIXME_VERY_BIG = 256;

	deviceFromBluetoothAddress(scannedDevice.m_bluetoothAddress, &bluetoothLEDevice, &bluetoothLEDevice3);

	wrl::ComPtr<IUnknown> gattResult = gattDeviceServicesResult(bluetoothLEDevice3);

	CHK(GetVt<zIGattDeviceServicesResult>(gattResult)->Services(gattResult.Get(), &services));
	CHK(ComIsA(uuidIVectorView__GattDeviceService_star__, services.Get()));

	CHK(GetVt<zIVectorView>(services)->Size(services.Get(), &size_serv));
	CHK(size_serv < FIXME_VERY_BIG ? S_OK : E_FAIL);

	serv = std::vector<IUnknown*>(size_serv);
	uuid = std::vector<UUID>(size_serv);

	CHK(GetVt<zIVectorView>(services)->GetMany(services.Get(), 0, size_serv, serv.data(), &ncopied_serv));

	for (auto & v : serv) {
		UUID u;
		CHK(ComIsA(uuidIGattDeviceService3, v));  // FIXME: should be uuidIGattDeviceService
		CHK(GetVt<zIGattDeviceService>(v)->Uuid(v, &u));
		uuid.push_back(u);
	}

	wrl::ComPtr<IUnknown> gattCharacteristicsResult_ = gattCharacteristicsResult(serv.at(0));
	wrl::ComPtr<IUnknown> characteristics;
	CHK(GetVt<zIGattCharacteristicsResult>(gattCharacteristicsResult_)->Characteristics(gattCharacteristicsResult_.Get(), &characteristics));
	CHK(ComIsA(uuidIVectorView__GattCharacteristic_star__, characteristics));
}


void stuff()
{
	wrlw::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

	auto scanner = make_unique<Scanner>();

	scanner->wait();

	probe(scanner->m_devices.at(0));

	println(cout, "");
}


int main(void)
{
	stuff();

	return 0;
}
