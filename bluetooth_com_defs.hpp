#include <codecvt>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

#include <roapi.h>
#include <windows.h>
#include <wrl.h>


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
UUID uuidIBluetoothLEAdvertisement = { 107983543, 13265, 20093, 131, 103, 207, 129, 208, 247, 150, 83 }; // __uuidof(ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisement)


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
		zfnc _8;
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
