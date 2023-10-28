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


wrl::ComPtr<IUnknown> operationwait(wrl::ComPtr<IUnknown> asyncOperation, UUID uuidAsyncOperation, UUID uuidAsyncOperationCompletedHandler, UUID uuidResult)
{
	CHK(ComIsA(uuidAsyncOperation, asyncOperation.Get()));
	wrl::ComPtr<ComHandlerWaitable_IAsyncOperation> cb = new ComHandlerWaitable_IAsyncOperation(uuidAsyncOperation, uuidAsyncOperationCompletedHandler);
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->Put_Completed(asyncOperation.Get(), cb.Get()));
	cb->wait();
	wrl::ComPtr<IUnknown> result;
	CHK(GetVt<zIAsyncOperation>(asyncOperation)->GetResults(asyncOperation.Get(), &result));
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


wrl::ComPtr<IUnknown> gattDeviceServicesResult(wrl::ComPtr<IUnknown> bluetoothLEDevice3)
{

	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> result;
	int32_t status;

	CHK(GetVt<zIBluetoothLEDevice3>(bluetoothLEDevice3)->GetGattServicesWithCacheModeAsync(bluetoothLEDevice3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &asyncOperation));

	result = operationwait(
		asyncOperation,
		uuidIAsyncOperation__GattDeviceServicesResult_star__,
		uuidIAsyncOperationCompletedHandler__GattDeviceServicesResult_star__,
		uuidIGattDeviceServicesResult);

	CHK(GetVt<zIGattDeviceServicesResult>(result)->Status(result.Get(), &status));
	CHK(status == (int32_t)zGattCommunicationStatus::Success ? S_OK : E_FAIL);

	return result;
}


wrl::ComPtr<IUnknown> gattCharacteristicsResult(wrl::ComPtr<IUnknown> gattDeviceService)
{
	wrl::ComPtr<IUnknown> deviceService3;
	wrl::ComPtr<IUnknown> asyncOperation;
	wrl::ComPtr<IUnknown> result;
	int32_t status;

	CHK(gattDeviceService->QueryInterface(uuidIGattDeviceService3, &deviceService3));

	CHK(GetVt<zIGattDeviceService3>(deviceService3)->GetCharacteristicsWithCacheModeAsync(deviceService3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &asyncOperation));

	result = operationwait(
		asyncOperation,
		uuidIAsyncOperation__GetCharacteristicsResult_star__,
		uuidIAsyncOperationCompletedHandler__GetCharacteristicsResult_star__,
		uuidIGattCharacteristicsResult);

	CHK(GetVt<zIGattCharacteristicsResult>(result)->Status(result.Get(), &status));
	CHK(status == (int32_t)zGattCommunicationStatus::Success ? S_OK : E_FAIL);

	return result;
}


struct DataCharacteristic
{
	UUID m_uuid;
	int32_t m_properties = (int32_t)zGattCharacteristicProperties::None;
	bool m_read = false;
	bool m_writ = false;
};


struct DataService
{
	UUID m_uuid;
	std::vector<DataCharacteristic> m_characteristicVec;
};


struct DataDiscover
{
	std::vector<DataService> m_serviceVec;
};


struct SelectedService
{
	UUID m_service;
	UUID m_characteristic_read;
	UUID m_characteristic_writ;
};


struct CommService
{
	SelectedService m_selectedService;
	wrl::ComPtr<IUnknown> m_gattDeviceService;
};


DataDiscover serviceDiscover(const std::vector<wrl::ComPtr<IUnknown>> &gattDeviceServiceVec)
{
	CHK(ComIsAV(uuidIGattDeviceService, gattDeviceServiceVec));

	DataDiscover dataDiscover;

	for (auto& v : gattDeviceServiceVec) {
		DataService dataService;

		UUID serviceUUID;

		CHK(GetVt<zIGattDeviceService>(v)->Uuid(v.Get(), &serviceUUID));

		wrl::ComPtr<IUnknown> gattCharacteristicsResult_ = gattCharacteristicsResult(v);
		wrl::ComPtr<IUnknown> characteristics;
		CHK(GetVt<zIGattCharacteristicsResult>(gattCharacteristicsResult_)->Characteristics(gattCharacteristicsResult_.Get(), &characteristics));
		CHK(ComIsA(uuidIVectorView__GattCharacteristic_star__, characteristics));
		std::vector<wrl::ComPtr<IUnknown>> characteristicVec = VectorViewGetManyHelper(characteristics, uuidIGattCharacterictic);
		CHK(ComIsAV(uuidIGattCharacterictic, characteristicVec));

		for (auto& c : characteristicVec) {
			DataCharacteristic dataCharacteristic;

			UUID characteristicUUID;

			CHK(GetVt<zIGattCharacteristic>(c)->Uuid(c.Get(), &characteristicUUID));

			int32_t gattCharacteristicProperties;

			CHK(GetVt<zIGattCharacteristic>(c)->CharacteristicProperties(c.Get(), &gattCharacteristicProperties));

			bool read = gattCharacteristicProperties & (int32_t)zGattCharacteristicProperties::Read;
			bool writ = gattCharacteristicProperties & (int32_t)zGattCharacteristicProperties::Write;

			dataCharacteristic.m_uuid = characteristicUUID;
			dataCharacteristic.m_properties = gattCharacteristicProperties;
			dataCharacteristic.m_read = read;
			dataCharacteristic.m_writ = writ;

			dataService.m_characteristicVec.push_back(dataCharacteristic);
		}

		dataService.m_uuid = serviceUUID;

		dataDiscover.m_serviceVec.push_back(dataService);
	}

	return dataDiscover;
}


SelectedService serviceSelect(const DataDiscover &dataDiscover)
{
	typedef std::vector<DataCharacteristic>::iterator i_t;

	std::vector<DataService> s;
	std::vector<SelectedService> e;
	std::vector<SelectedService> f;
	std::copy_if(std::begin(dataDiscover.m_serviceVec), std::end(dataDiscover.m_serviceVec), std::back_inserter(s),
		[](const DataService &a) {
			return uint16FromBluetoothServiceUUID(a.m_uuid) == 0xFFFF;
		});
	std::transform(std::begin(s), std::end(s), std::back_inserter(e),
		[](const DataService& a) -> SelectedService {
			auto read = std::find_if(std::begin(a.m_characteristicVec), std::end(a.m_characteristicVec), [](const DataCharacteristic& b) { return b.m_read; });
			auto writ = std::find_if(std::begin(a.m_characteristicVec), std::end(a.m_characteristicVec), [](const DataCharacteristic& b) { return b.m_writ; });
			return {
				.m_service = a.m_uuid,
				.m_characteristic_read = read != std::end(a.m_characteristicVec) ? read->m_uuid : uuidSentinel,
				.m_characteristic_writ = writ != std::end(a.m_characteristicVec) ? writ->m_uuid : uuidSentinel };
		});
	std::copy_if(std::begin(e), std::end(e), std::back_inserter(f),
		[](const SelectedService& a) {
			return a.m_characteristic_read != uuidSentinel && a.m_characteristic_writ != uuidSentinel;
		});
	CHK(f.size() == 1 ? S_OK : E_FAIL);

	return f.at(0);
}


CommService CommServiceSelect(const SelectedService& selectedService, const std::vector<wrl::ComPtr<IUnknown>>& serv)
{
	auto read = std::find_if(std::begin(serv), std::end(serv),
		[&selectedService](const wrl::ComPtr<IUnknown>& s) {
			UUID u;
			CHK(ComIsA(uuidIGattDeviceService, s.Get()));
			CHK(GetVt<zIGattDeviceService>(s)->Uuid(s.Get(), &u));
			return selectedService.m_service == u;
		});
	CHK(read != std::end(serv) ? S_OK : E_FAIL);
	return { .m_selectedService = selectedService, .m_gattDeviceService = *read };
}


void probe(const ScannedDevice& scannedDevice)
{
	wrl::ComPtr<IUnknown> bluetoothLEDevice;
	wrl::ComPtr<IUnknown> bluetoothLEDevice3;

	wrl::ComPtr<IUnknown> services;

	deviceFromBluetoothAddress(scannedDevice.m_bluetoothAddress, &bluetoothLEDevice, &bluetoothLEDevice3);

	wrl::ComPtr<IUnknown> gattResult = gattDeviceServicesResult(bluetoothLEDevice3);

	CHK(GetVt<zIGattDeviceServicesResult>(gattResult)->Services(gattResult.Get(), &services));
	CHK(ComIsA(uuidIVectorView__GattDeviceService_star__, services.Get()));

	std::vector<wrl::ComPtr<IUnknown>> serv = VectorViewGetManyHelper(services, uuidIGattDeviceService);

	DataDiscover dataDiscover = serviceDiscover(serv);
	SelectedService selectedService = serviceSelect(dataDiscover);
	CommService commServiceSelect = CommServiceSelect(selectedService, serv);
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
