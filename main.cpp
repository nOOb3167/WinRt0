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

#include <algorithm>
#include <iterator>
#include <thread>
#include <mutex>
#include <memory>
#include <numeric>
#include <locale>
#include <codecvt>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
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


wrl::ComPtr<IUnknown> operationwait(wrl::ComPtr<IUnknown> asyncOperation, UUID uuidAsyncOperation, UUID uuidAsyncOperationCompletedHandler, UUID uuidResult, bool nullResultAllowed = false)
{
	CHK(ComIsA(uuidAsyncOperation, asyncOperation.Get()));
	wrl::ComPtr<ComHandlerWaitable_IAsyncOperation> cb = new ComHandlerWaitable_IAsyncOperation(uuidAsyncOperation, uuidAsyncOperationCompletedHandler);
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->Put_Completed(asyncOperation.Get(), cb.Get()));
	cb->wait();

	wrl::ComPtr<IUnknown> asyncInfo;
	int32_t status;
	HRESULT errorCode;
	CHK(asyncOperation->QueryInterface(uuidIAsyncInfo, &asyncInfo));
	CHK(GetVt<zIAsyncInfo>(asyncInfo)->Status(asyncInfo.Get(), &status));
	CHK(GetVt<zIAsyncInfo>(asyncInfo)->ErrorCode(asyncInfo.Get(), &errorCode));
	CHK(status == (int32_t)zAsyncStatus::Completed && errorCode == S_OK ? S_OK : E_FAIL);

	wrl::ComPtr<IUnknown> result;
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->GetResults(asyncOperation.Get(), &result));

	if (nullResultAllowed && result == nullptr)
		return nullptr;
	CHK(ComIsA(uuidResult, result.Get()));
	return result;
}


void deviceFromBluetoothAddress(uint64_t bluetoothAddress, wrl::ComPtr<IUnknown> *outBluetoothLEDevice, wrl::ComPtr<IUnknown>* outBluetoothLEDevice3)
{
	wrl::ComPtr<IUnknown> bluetoothLEDeviceStatics;
	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> bluetoothLEDevice;
	wrl::ComPtr<IUnknown> bluetoothLEDevice3;

	CHK(RoGetActivationFactory(wrlw::HString::MakeReference(L"Windows.Devices.Bluetooth.BluetoothLEDevice").Get(), uuidIBluetoothLEDeviceStatics, &bluetoothLEDeviceStatics));
	
	CHK(GetVt<zIBluetoothLEDeviceStatics>(bluetoothLEDeviceStatics)->FromBluetoothAddressAsync(bluetoothLEDeviceStatics.Get(), bluetoothAddress, &asyncOperation));

	bluetoothLEDevice = operationwait(
		asyncOperation,
		uuidIAsyncOperation__BluetoothLEDevice__,
		uuidIAsyncOperationCompletedHandler__BluetoothLEDevice__,
		uuidIBluetoothLEDevice);

	CHK(bluetoothLEDevice->QueryInterface(uuidIBluetoothLEDevice3, &bluetoothLEDevice3));

	*outBluetoothLEDevice = bluetoothLEDevice;
	*outBluetoothLEDevice3 = bluetoothLEDevice3;
}


std::vector<wrl::ComPtr<IUnknown>> getDeviceServices(wrl::ComPtr<IUnknown> bluetoothLEDevice3)
{

	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> result;
	int32_t status;
	wrl::ComPtr<IUnknown> services;

	CHK(GetVt<zIBluetoothLEDevice3>(bluetoothLEDevice3)->GetGattServicesWithCacheModeAsync(bluetoothLEDevice3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &asyncOperation));

	result = operationwait(
		asyncOperation,
		uuidIAsyncOperation__GattDeviceServicesResult_star__,
		uuidIAsyncOperationCompletedHandler__GattDeviceServicesResult_star__,
		uuidIGattDeviceServicesResult);

	CHK(GetVt<zIGattDeviceServicesResult>(result)->Status(result.Get(), &status));
	CHK(status == (int32_t)zGattCommunicationStatus::Success ? S_OK : E_FAIL);

	CHK(GetVt<zIGattDeviceServicesResult>(result)->Services(result.Get(), &services));
	CHK(ComIsA(uuidIVectorView__GattDeviceService_star__, services.Get()));

	return VectorViewGetManyHelper(services, uuidIGattDeviceService);;
}


std::vector<wrl::ComPtr<IUnknown>> getServiceCharacteristics(wrl::ComPtr<IUnknown> gattDeviceService)
{
	wrl::ComPtr<IUnknown> deviceService3;
	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> result;
	int32_t status;
	wrl::ComPtr<IUnknown> characteristics;

	CHK(gattDeviceService->QueryInterface(uuidIGattDeviceService3, &deviceService3));

	CHK(GetVt<zIGattDeviceService3>(deviceService3)->GetCharacteristicsWithCacheModeAsync(deviceService3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &asyncOperation));

	result = operationwait(
		asyncOperation,
		uuidIAsyncOperation__GetCharacteristicsResult_star__,
		uuidIAsyncOperationCompletedHandler__GetCharacteristicsResult_star__,
		uuidIGattCharacteristicsResult);

	CHK(GetVt<zIGattCharacteristicsResult>(result)->Status(result.Get(), &status));
	CHK(status == (int32_t)zGattCommunicationStatus::Success ? S_OK : E_FAIL);

	CHK(GetVt<zIGattCharacteristicsResult>(result)->Characteristics(result.Get(), &characteristics));
	CHK(ComIsA(uuidIVectorView__GattCharacteristic_star__, characteristics));

	return VectorViewGetManyHelper(characteristics, uuidIGattCharacterictic);
}


void writeCharacteristic(wrl::ComPtr<IUnknown> characteristic, std::string data)
{
	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> result;

	[[maybe_unused]] wrl::ComPtr<IUnknown> writerFactory;

	CHK(RoGetActivationFactory(wrlw::HString::MakeReference(L"Windows.Storage.Streams.DataWriter").Get(), uuidIDataWriterFactory, &writerFactory));

	wrl::ComPtr<IInspectable> writer;
	wrl::ComPtr<IInspectable> buffer;

	CHK(RoActivateInstance(wrlw::HString::MakeReference(L"Windows.Storage.Streams.DataWriter").Get(), &writer));

	CHK(GetVt<zfIDataWriter>(writer)->WriteBytes(writer.Get(), (uint32_t)data.size(), data.data()));
	CHK(GetVt<zfIDataWriter>(writer)->DetachBuffer(writer.Get(), &buffer));
	CHK(ComIsA(uuidIBuffer, buffer.Get()));

	CHK(GetVt<zIGattCharacteristic>(characteristic)->WriteValueAsync(characteristic.Get(), buffer.Get(), &asyncOperation));

	result = operationwait(
		asyncOperation,
		uuidIASyncOperation__GattCommunicationStatus__,
		uuidIASyncOperationCompletedHandler__GattCommunicationStatus__,
		uuidSentinel,
		true);

	CHK(result == nullptr ? S_OK : E_FAIL);
}


void subscribeNotifyCharacteristic(wrl::ComPtr<IUnknown> characteristic)
{
	CHK(ComIsA(uuidIGattCharacterictic, characteristic));

	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> result;
	EventRegistrationToken token_unused;

	CHK(GetVt<zIGattCharacteristic>(characteristic)->WriteClientCharacteristicConfigurationDescriptorAsync(characteristic.Get(), (int32_t)zGattClientCharacteristicConfigurationDescriptorValue::Notify, &asyncOperation));

	result = operationwait(
		asyncOperation,
		uuidIASyncOperation__GattCommunicationStatus__,
		uuidIASyncOperationCompletedHandler__GattCommunicationStatus__,
		uuidSentinel,
		true);

	CHK(result == nullptr ? S_OK : E_FAIL);

	wrl::ComPtr<ComHandler_ITypedEventHandler_GattCharacteristic_GattValueChangedEventArgs> cb = new ComHandler_ITypedEventHandler_GattCharacteristic_GattValueChangedEventArgs(
		[]() {
			println(cout, "response");
		}
	);

	CHK(GetVt<zIGattCharacteristic>(characteristic)->Add_ValueChanged(characteristic.Get(), cb.Get(), &token_unused));
}


struct DataCharacteristic
{
	wrl::ComPtr<IUnknown> m_ptr;
	int32_t m_properties = (int32_t)zGattCharacteristicProperties::None;
	bool m_read = false;
	bool m_writ = false;
};


struct DataService
{
	wrl::ComPtr<IUnknown>           m_ptr;
	std::vector<DataCharacteristic> m_characteristicVec;
	UUID                            m_UUID;
};


struct DataDevice
{
	wrl::ComPtr<IUnknown>    m_ptr;
	std::vector<DataService> m_serviceVec;
};


struct SelectedService
{
	DataDevice  m_device;
	DataService m_service;
	DataCharacteristic m_characteristic_read;
	DataCharacteristic m_characteristic_writ;
};


DataDevice deviceDiscover(wrl::ComPtr<IUnknown> bluetoothLEDevice3)
{
	DataDevice dataDevice;

	// fill ptr

	for (wrl::ComPtr<IUnknown>& service : getDeviceServices(bluetoothLEDevice3)) {
		std::vector<wrl::ComPtr<IUnknown>> c = getServiceCharacteristics(service);
		std::vector<DataCharacteristic> c2;
		std::transform(std::begin(c), std::end(c), std::back_inserter(c2), [](const wrl::ComPtr<IUnknown>& a) { return DataCharacteristic { .m_ptr = a }; });
		dataDevice.m_serviceVec.push_back(DataService { .m_ptr = service, .m_characteristicVec = c2 });
	}

	// fill rest

	for (DataService& service : dataDevice.m_serviceVec) {

		CHK(GetVt<zIGattDeviceService>(service.m_ptr)->Uuid(service.m_ptr.Get(), &service.m_UUID));

		for (DataCharacteristic& characteristic : service.m_characteristicVec) {
			CHK(GetVt<zIGattCharacteristic>(characteristic.m_ptr)->CharacteristicProperties(characteristic.m_ptr.Get(), &characteristic.m_properties));
			characteristic.m_read = characteristic.m_properties & (int32_t)zGattCharacteristicProperties::Read;
			characteristic.m_writ = characteristic.m_properties & (int32_t)zGattCharacteristicProperties::Write;
		}
	}

	return dataDevice;
}


SelectedService serviceSelect(const DataDevice &dataDevice)
{
	auto first_readable = [](const std::vector<DataCharacteristic>& a) { return std::ranges::find_first_of(a, std::vector<bool> { true }, std::ranges::equal_to{}, & DataCharacteristic::m_read); };
	auto first_writable = [](const std::vector<DataCharacteristic>& a) { return std::ranges::find_first_of(a, std::vector<bool> { true }, std::ranges::equal_to{}, & DataCharacteristic::m_writ); };

	std::vector<DataService> s;
	std::vector<DataService> t;
	std::vector<DataService> u;

	// filter standard bluetooth services
	std::copy_if(std::begin(dataDevice.m_serviceVec), std::end(dataDevice.m_serviceVec), std::back_inserter(s),
		[](const DataService &a) {
			return uint16FromBluetoothServiceUUID(a.m_UUID) == 0xFFFF;
		});
	// keep simultaneously readable and writable
	std::copy_if(std::begin(s), std::end(s), std::back_inserter(t),
		[&](const DataService& a) {
			return first_readable(a.m_characteristicVec) != std::end(a.m_characteristicVec) && first_writable(a.m_characteristicVec) != std::end(a.m_characteristicVec);
		});

	CHK(t.size() == 1 ? S_OK : E_FAIL);

	auto& first = t.at(0);

	SelectedService selected = {
		.m_device = dataDevice,
		.m_service = first,
		.m_characteristic_read = *first_readable(first.m_characteristicVec),
		.m_characteristic_writ = *first_writable(first.m_characteristicVec),
	};

	return selected;
}


void probe(const ScannedDevice& scannedDevice)
{
	wrl::ComPtr<IUnknown> bluetoothLEDevice;
	wrl::ComPtr<IUnknown> bluetoothLEDevice3;

	wrl::ComPtr<IUnknown> services;

	deviceFromBluetoothAddress(scannedDevice.m_bluetoothAddress, &bluetoothLEDevice, &bluetoothLEDevice3);

	DataDevice dataDiscover = deviceDiscover(bluetoothLEDevice3);
	SelectedService selectedService = serviceSelect(dataDiscover);

	subscribeNotifyCharacteristic(selectedService.m_characteristic_read.m_ptr);
	writeCharacteristic(selectedService.m_characteristic_writ.m_ptr, "\xcd\x40\xfa\xf6\x09\x00\x00\x00\x00\x00\x00\x00\x00\x00");
	Sleep(5000);
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
	_printuuid();
	stuff();

	return 0;
}
