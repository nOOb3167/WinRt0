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


void probe(const ScannedDevice& scannedDevice)
{
	wrl::ComPtr<IUnknown> bluetoothLEDevice;
	wrl::ComPtr<IUnknown> bluetoothLEDevice3;

	deviceFromBluetoothAddress(scannedDevice.m_bluetoothAddress, &bluetoothLEDevice, &bluetoothLEDevice3);

	MCompleted cb2mcompleted;
	wrl::ComPtr<ComHandler_IAsyncOperationCompletedHandler__BluetoothLEDevice__> cb2 = new ComHandler_IAsyncOperationCompletedHandler__BluetoothLEDevice__(
		[&cb2mcompleted]() {
			cb2mcompleted.signal();
		}
	);

	MCompleted cb3mcompleted;
	wrl::ComPtr<ComHandler_IAsyncOperationCompletedHandler__GattDeviceServicesResult_star__> cb3 = new ComHandler_IAsyncOperationCompletedHandler__GattDeviceServicesResult_star__(
		[&cb3mcompleted]() {
			cb3mcompleted.signal();
		}
	);

	wrl::ComPtr<IUnknown> ggswcma_op;
	CHK(GetVt<zIBluetoothLEDevice3>(bluetoothLEDevice3)->GetGattServicesWithCacheModeAsync(bluetoothLEDevice3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &ggswcma_op));

	CHK(ComIsA(uuidIAsyncOperation__GattDeviceServicesResult_star__, ggswcma_op));

	CHK(GetVt<zIAsyncOperation>(ggswcma_op)->Put_Completed(ggswcma_op.Get(), cb3.Get()));

	cb3mcompleted.wait();

	wrl::ComPtr<IUnknown> gdsr;
	int32_t gdsr_status = 0;

	CHK(GetVt<zIAsyncOperation>(ggswcma_op)->GetResults(ggswcma_op.Get(), &gdsr));

	CHK(GetVt<zIGattDeviceServicesResult>(gdsr)->Status(gdsr.Get(), &gdsr_status));

	CHK(gdsr_status == 0 ? S_OK : E_FAIL);
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
