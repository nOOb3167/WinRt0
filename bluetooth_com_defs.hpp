#include <codecvt>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

#include <roapi.h>
#include <windows.h>
#include <wrl.h>


#include <windows.foundation.collections.h>
#include <windows.devices.bluetooth.genericattributeprofile.h>


namespace bt
{


static UUID MakeUUID(std::string s)
{
	UUID u = {};
	if (UuidFromStringA((unsigned char*)s.data(), &u) != RPC_S_OK)
		throw std::runtime_error("UuidFromStringA");
	return u;
}


enum class zBluetoothLEScanningMode : int32_t
{
	Passive = 0,
	Active = 1,
	None = 2,
};


enum class zBluetoothCacheMode : int32_t
{
	Cached = 0,
	Uncached = 1
};


enum class zGattCommunicationStatus : int32_t
{
	Success = 0,
	Unreachable = 1,
	ProtocolError = 2,
	AccessDenied = 3,
};


struct do_not_call_t
{
private:
	do_not_call_t() {}
};


typedef void (*zfnc)(do_not_call_t);


typedef HRESULT (*zfScanningMode_Set)(IUnknown* thiz, int32_t bluetoothLEScanningMode);
typedef HRESULT (*zfStart)(IUnknown* thiz);
typedef HRESULT (*zfStop)(IUnknown* thiz);
typedef HRESULT (*zfReceived)(IUnknown *thiz, IUnknown* handler, EventRegistrationToken *tok);
typedef HRESULT (*zfBluetoothAddress)(IUnknown* thiz, uint64_t *value);
typedef HRESULT (*zfAdvertisement)(IUnknown* thiz, IUnknown** out);
typedef HRESULT (*zfLocalName)(IUnknown* thiz, HSTRING* value);
typedef HRESULT (*zfFromBluetoothAddressAsync)(IUnknown* thiz, uint64_t bluetoothAddress, IUnknown** out);
typedef HRESULT (*zfGetGattServicesWithCacheModeAsync)(IUnknown* thiz, int32_t bluetoothCacheMode, IUnknown** out);
typedef HRESULT (*zfPut_Completed)(IUnknown* thiz, IUnknown* handler);
typedef HRESULT (*zfGetResults)(IUnknown* thiz, IUnknown** out);
typedef HRESULT (*zfStatus)(IUnknown* thiz, int32_t *out);
typedef HRESULT (*zfServices)(IUnknown* thiz, IUnknown** out);
typedef HRESULT (*zfSize)(IUnknown* thiz, uint32_t* value);
typedef HRESULT (*zfGetMany)(IUnknown* thiz, uint32_t start_index, uint32_t size_o_items, IUnknown** o_items, uint32_t* o_ncopied); // FIXME: this is for returning IUnknown - o_items will differ for other types
typedef HRESULT (*zfUuid)(IUnknown* thiz, UUID* value);
typedef HRESULT (*zfGetCharacteristicsWithCacheModeAsync)(IUnknown* thiz, int32_t cacheMode, IUnknown** out);
typedef HRESULT (*zfCharacteristics)(IUnknown* thiz, IUnknown** out);

UUID uuidTypedEventHandlerReceivedTEH = { 2431340234, 54373, 24224,  166, 28, 3, 60, 140, 94, 206, 242 }; // __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher*, ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs*>)
UUID uuidIBluetoothLEAdvertisementWatcher = MakeUUID("A6AC336F-F3D3-4297-8D6C-C81EA6623F40");
UUID uuidIBluetoothLEAdvertisementReceivedEventArgs = MakeUUID("27987DDF-E596-41BE-8D43-9E6731D4A913");
UUID uuidIBluetoothLEAdvertisementReceivedEventArgs2 = MakeUUID("12D9C87B-0399-5F0E-A348-53B02B6B162E");
UUID uuidIBluetoothLEDeviceStatics = MakeUUID("C8CF1A19-F0B6-4BF0-8689-41303DE2D9F4");
UUID uuidIBluetoothLEDevice3 = MakeUUID("AEE9E493-44AC-40DC-AF33-B2C13C01CA46");
UUID uuidIAsyncOperation__BluetoothLEDevice__ = { 929013095, 29858, 24465, 161, 29, 22, 144, 147, 113, 141, 65 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*>)
UUID uuidIAsyncOperationCompletedHandler__BluetoothLEDevice__ = { 2438379423, 50506, 21111, 143, 139, 210, 204, 67, 199, 224, 4 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*>)
UUID uuidIAsyncOperation__GattDeviceServicesResult_star__ = { 3888539638, 59508, 20495, 134, 255, 118, 12, 166, 240, 122, 88 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult*>)
UUID uuidIAsyncOperationCompletedHandler__GattDeviceServicesResult_star__ = { 1957365906, 42545, 23916, 177, 180, 189, 46, 26, 116, 26, 155 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult*>)
UUID uuidIAsyncOperation__GetCharacteristicsResult_star__ = { 158472522, 44060, 21814, 152, 134, 39, 229, 138, 24, 242, 115 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicsResult *>)
UUID uuidIAsyncOperationCompletedHandler__GetCharacteristicsResult_star__ = { 3600897141, 7794, 23638, 152, 232, 136, 244, 188, 62, 3, 19 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicsResult *>)
UUID uuidIBluetoothLEAdvertisement = { 107983543, 13265, 20093, 131, 103, 207, 129, 208, 247, 150, 83 }; // __uuidof(ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisement)
UUID uuidIVectorView__GattDeviceService_star__ = { 2089713629, 41377, 21130, 129, 209, 41, 103, 105, 34, 122, 8 }; // __uuidof(ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService*>)
UUID uuidIVectorView__GattCharacteristic_star__ = { 3409621934, 46433, 20559, 168, 8, 89, 157, 236, 235, 45, 244 }; // __uuidof(ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*>)
UUID uuidIGattDeviceService3 = { 2996021584, 3155, 17276, 169, 179, 92, 50, 16, 198, 229, 105 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService3)
UUID uuidIGattCharacterictic = { 1506496705, 22836, 20328, 161, 152, 235, 134, 79, 164, 78, 107 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic)
UUID uuidR = __uuidof(ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*>);

// special UUID parsed to discover GATT Services (https://bitbucket.org/bluetooth-SIG/public/src/main/assigned_numbers/uuids/service_uuids.yaml) also qbluetoothuuid.cpp@toUInt16
UUID uuidBluetoothBaseUUID = MakeUUID("00000000-0000-1000-8000-00805F9B34FB");


struct vt_iunknown
{
	zfnc _0;
	zfnc _1;
	zfnc _2;
};


struct vt_iinspectable
{
	vt_iunknown base;
	zfnc _3;
	zfnc _4;
	zfnc _5;
};


struct zIBluetoothLEAdvertisementWatcher
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfnc _7;
		zfnc _8;
		zfnc _9;
		zfnc _10;
		zfnc _11;
		zfScanningMode_Set ScanningMode_Set;
		zfnc _13;
		zfnc _14;
		zfnc _15;
		zfnc _16;
		zfStart Start;
		zfStop Stop;
		zfReceived Received;
	};
	vt* vt;
};


struct zIBluetoothLEAdvertisementReceivedEventArgs
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfBluetoothAddress BluetoothAddress;
		zfnc _8;
		zfnc _9;
		zfAdvertisement Advertisement;
	};
	vt* vt;
};


struct zIBluetoothLEDeviceStatics
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfFromBluetoothAddressAsync FromBluetoothAddressAsync;
	};
	vt* vt;
};


struct zIBluetoothLEDevice3
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfnc _7;
		zfnc _8;
		zfGetGattServicesWithCacheModeAsync GetGattServicesWithCacheModeAsync;
	};
	vt* vt;
};


struct zIAsyncOperation
{
	struct vt
	{
		vt_iinspectable base;
		zfPut_Completed Put_Completed;
		zfnc _7;
		zfGetResults GetResults;
	};
	vt* vt;
};


struct zIGattDeviceServicesResult
{
	struct vt
	{
		vt_iinspectable base;
		zfStatus Status;
		zfnc _7;
		zfServices Services;
	};
	vt* vt;
};


struct zIGattCharacteristicsResult
{
	struct vt
	{
		vt_iinspectable base;
		zfStatus Status;
		zfnc _7;
		zfCharacteristics Characteristics;
	};
	vt* vt;
};


struct zIGattDeviceService
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfnc _7;
		zfnc _8;
		zfUuid Uuid;
	};
	vt* vt;
};


struct zIGattDeviceService3
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfnc _7;
		zfnc _8;
		zfnc _9;
		zfnc _10;
		zfnc _11;
		zfGetCharacteristicsWithCacheModeAsync GetCharacteristicsWithCacheModeAsync;
	};
	vt* vt;
};


struct zIVectorView
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfSize Size;
		zfnc _8;
		zfGetMany GetMany;
	};
	vt* vt;
};


struct zIBluetoothLEAdvertisement
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfnc _7;
		zfLocalName LocalName;
	};
	vt* vt;
};


} // namespace bt
