#pragma once

#include <codecvt>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

#include <roapi.h>
#include <windows.h>
#include <wrl.h>

#ifdef zNOTMINGW
#include <windows.foundation.collections.h>
#include <windows.devices.bluetooth.genericattributeprofile.h>
#include <windows.storage.streams.h>
#endif // zNOTMINGW

namespace bt
{


struct zEventRegistrationToken
{
	char m_do_not_use[1024];
};


static UUID MakeUUID(std::string s)
{
	UUID u = {};
	if (UuidFromStringA((unsigned char*)s.data(), &u) != RPC_S_OK)
		throw std::runtime_error("UuidFromStringA");
	return u;
}


static void PrintUUID(UUID uuid, std::string s)
{
#ifdef zNOTMINGW
	println(std::cout, "{{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }}; // {}", uuid.Data1, uuid.Data2, uuid.Data3,
		uuid.Data4[0], uuid.Data4[1], uuid.Data4[2], uuid.Data4[3], uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7], s);
#endif // zNOTMINGW
}


static void _printuuid()
{
#define _PRINTUUID(x) PrintUUID(x, #x)
	//_PRINTUUID(__uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService));
	//_PRINTUUID(__uuidof(ABI::Windows::Storage::Streams::IBuffer));
	//_PRINTUUID(__uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus>));
	//_PRINTUUID(__uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus>));
	//_PRINTUUID(__uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadResult*>));
	//_PRINTUUID(__uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadResult*>));
	//_PRINTUUID(__uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattReadResult));
	//_PRINTUUID(__uuidof(ABI::Windows::Storage::Streams::IDataWriter));
	//_PRINTUUID(__uuidof(ABI::Windows::Storage::Streams::IDataWriterFactory));
	//_PRINTUUID(__uuidof(ABI::Windows::Storage::Streams::IDataReaderStatics));
	//_PRINTUUID(__uuidof(ABI::Windows::Storage::Streams::IDataReader));
	//_PRINTUUID(__uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*, ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*>));
	//_PRINTUUID(__uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs));
	//_PRINTUUID(__uuidof(ABI::Windows::Foundation::IAsyncInfo));
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


enum class zGattCharacteristicProperties : int32_t
{
	None = 0x0,
	Broadcast = 0x1,
	Read = 0x2,
	WriteWithoutResponse = 0x4,
	Write = 0x8,
	Notify = 0x10,
	Indicate = 0x20,
	AuthenticatedSignedWrites = 0x40,
	ExtendedProperties = 0x80,
	ReliableWrites = 0x100,
	WritableAuxiliaries = 0x200
};


enum class zGattClientCharacteristicConfigurationDescriptorValue
{
	None = 0,
	Notify = 1,
	Indicate = 2
};


enum class zAsyncStatus : int32_t
{
	Canceled = 2,
	Completed = 1,
	Error = 3,
	Started = 0,
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
typedef HRESULT (*zfReceived)(IUnknown *thiz, IUnknown* handler, zEventRegistrationToken *tok);
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
typedef HRESULT (*zfCharacteristicProperties)(IUnknown* thiz, int32_t *value);
typedef HRESULT (*zfWriteValueAsync)(IUnknown* thiz, IUnknown* buffer, IUnknown** asyncOp);
typedef HRESULT (*zfCreateDataWriter)(IUnknown* thiz, IUnknown** out);
typedef HRESULT (*zfWriteBytes)(IUnknown* thiz, uint32_t __valueSize, const char* value);
typedef HRESULT (*zfDetachBuffer)(IUnknown* thiz, IUnknown** out);
typedef HRESULT (*zfWriteClientCharacteristicConfigurationDescriptorAsync)(IUnknown* thiz, int32_t value, IUnknown** out);
typedef HRESULT (*zfAdd_ValueChanged)(IUnknown* thiz, IUnknown* handler, zEventRegistrationToken* tok);
typedef HRESULT (*zfErrorCode)(IUnknown* thiz, HRESULT* value);
typedef HRESULT (*zfReadValueWithCacheModeAsync)(IUnknown* thiz, int32_t cacheMode, IUnknown** asyncOp);
typedef HRESULT (*zfValue)(IUnknown* thiz, IUnknown** out);
typedef HRESULT (*zfFromBuffer)(IUnknown* thiz, IUnknown* buffer, IUnknown** out);
typedef HRESULT (*zfReadBytes)(IUnknown* thiz, uint32_t, const char* value);
typedef HRESULT (*zfUnconsumedBufferLength)(IUnknown* thiz, uint32_t* length);
typedef HRESULT (*zfLength)(IUnknown* thiz, uint32_t* length);


UUID uuidIUnknown = { 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 70 }; // __uuidof(IUnknown)
UUID uuidTypedEventHandlerReceivedTEH = { 2431340234, 54373, 24224,  166, 28, 3, 60, 140, 94, 206, 242 }; // __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher*, ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs*>)
UUID uuidIBluetoothLEAdvertisementWatcher = MakeUUID("A6AC336F-F3D3-4297-8D6C-C81EA6623F40");
UUID uuidIBluetoothLEAdvertisementReceivedEventArgs = MakeUUID("27987DDF-E596-41BE-8D43-9E6731D4A913");
UUID uuidIBluetoothLEAdvertisementReceivedEventArgs2 = MakeUUID("12D9C87B-0399-5F0E-A348-53B02B6B162E");
UUID uuidIBluetoothLEDeviceStatics = MakeUUID("C8CF1A19-F0B6-4BF0-8689-41303DE2D9F4");
UUID uuidIBluetoothLEDevice = { 3052285819, 19160, 17986, 172, 72, 128, 160, 181, 0, 232, 135 }; // __uuidof(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice)
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
UUID uuidIGattDeviceService = { 2893773829, 45884, 18383, 153, 15, 107, 143, 85, 119, 223, 113 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService)
UUID uuidIGattDeviceService3 = { 2996021584, 3155, 17276, 169, 179, 92, 50, 16, 198, 229, 105 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService3)
UUID uuidIGattCharacterictic = { 1506496705, 22836, 20328, 161, 152, 235, 134, 79, 164, 78, 107 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic)
UUID uuidIGattCharacteristicsResult = { 294949980, 45655, 20286, 157, 183, 246, 139, 201, 169, 174, 242 } ; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristicsResult)
UUID uuidIGattDeviceServicesResult = { 387830766, 365, 16797, 131, 138, 87, 108, 244, 117, 163, 216 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult)
UUID uuidIBuffer = { 2421821408, 48211, 4575, 140, 73, 0, 30, 79, 198, 134, 218 }; // __uuidof(ABI::Windows::Storage::Streams::IBuffer)
UUID uuidIASyncOperation__GattCommunicationStatus__ = { 1073124630, 7163, 21225, 158, 230, 229, 205, 183, 142, 22, 131 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus>)
UUID uuidIASyncOperationCompletedHandler__GattCommunicationStatus__ = { 559157626, 38797, 23003, 153, 207, 107, 105, 12, 179, 56, 155 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus>)
UUID uuidIAsyncOperation__GattReadResult_star__ = { 3557044904, 7700, 20944, 180, 155, 174, 44, 225, 170, 5, 229 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadResult*>)
UUID uuidIAsyncOperationCompletedHandler__GattReadResult_star__ = { 3633916576, 60098, 21943, 146, 197, 137, 72, 134, 190, 176, 202 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadResult*>)
UUID uuidIGattReadResult = { 1671851784, 6890, 19532, 165, 15, 151, 186, 228, 116, 179, 72 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattReadResult)
UUID uuidIDataWriter = { 1689817701, 54081, 18722, 179, 138, 221, 74, 248, 128, 140, 78 }; // __uuidof(ABI::Windows::Storage::Streams::IDataWriter)
UUID uuidIDataWriterFactory = { 864839618, 35716, 19499, 156, 80, 123, 135, 103, 132, 122, 31 }; // __uuidof(ABI::Windows::Storage::Streams::IDataWriterFactory)
UUID uuidITypedEventHandler__GattCharacteristic_star__GattValueChangedEventArgs_star__ = { 3254001910, 25234, 22368, 162, 201, 157, 223, 152, 104, 60, 252 }; // __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*, ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*>)
UUID uuidIGattValueChangedEventArgs = { 3525040980, 1763, 20184, 162, 99, 172, 250, 200, 186, 115, 19 }; // __uuidof(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs)
UUID uuidIAsyncInfo = { 54, 0, 0, 192, 0, 0, 0, 0, 0, 0, 70 }; // __uuidof(ABI::Windows::Foundation::IAsyncInfo)
UUID uuidIDataReaderStatics = { 301776840, 63802, 18203, 177, 33, 243, 121, 227, 73, 49, 60 }; // __uuidof(ABI::Windows::Storage::Streams::IDataReaderStatics);
UUID uuidIDataReader = { 3803512873, 46273, 17172, 164, 184, 251, 129, 58, 47, 39, 94 }; // __uuidof(ABI::Windows::Storage::Streams::IDataReader)


// special UUID parsed to discover GATT Services (https://bitbucket.org/bluetooth-SIG/public/src/main/assigned_numbers/uuids/service_uuids.yaml) also qbluetoothuuid.cpp@toUInt16
UUID uuidBluetoothBaseUUID = MakeUUID("00000000-0000-1000-8000-00805F9B34FB");

UUID uuidSentinel = MakeUUID("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF");


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


struct zIAsyncInfo
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfStatus Status;
		zfErrorCode ErrorCode;
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


struct zIGattCharacteristic
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfCharacteristicProperties CharacteristicProperties;
		zfnc _8;
		zfnc _9;
		zfnc _10;
		zfUuid Uuid;
		zfnc _12;
		zfnc _13;
		zfnc _14;
		zfReadValueWithCacheModeAsync ReadValueWithCacheModeAsync;
		zfWriteValueAsync WriteValueAsync;
		zfnc _17;
		zfnc _18;
		zfWriteClientCharacteristicConfigurationDescriptorAsync WriteClientCharacteristicConfigurationDescriptorAsync;
		zfAdd_ValueChanged Add_ValueChanged;
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


struct zfIDataWriterFactory
{
	struct vt
	{
		vt_iinspectable base;
		zfCreateDataWriter CreateDataWriter;
	};
	vt* vt;
};


struct zfIDataWriter
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
		zfWriteBytes WriteBytes;
		zfnc _13;
		zfnc _14;
		zfnc _15;
		zfnc _16;
		zfnc _17;
		zfnc _18;
		zfnc _19;
		zfnc _20;
		zfnc _21;
		zfnc _22;
		zfnc _23;
		zfnc _24;
		zfnc _25;
		zfnc _26;
		zfnc _27;
		zfnc _28;
		zfnc _29;
		zfnc _30;
		zfDetachBuffer DetachBuffer;
	};
	vt* vt;
};


struct zfIDataReaderStatics
{
	struct vt
	{
		vt_iinspectable base;
		zfFromBuffer FromBuffer;
	};
	vt* vt;
};


struct zfIDataReader
{
	struct vt
	{
		vt_iinspectable base;
		zfUnconsumedBufferLength UnconsumedBufferLength;
		zfnc _7;
		zfnc _8;
		zfnc _9;
		zfnc _10;
		zfnc _11;
		zfnc _12;
		zfnc _13;
		zfReadBytes ReadBytes;
	};
	vt* vt;
};


struct zIGattReadResult
{
	struct vt
	{
		vt_iinspectable base;
		zfStatus Status;
		zfValue Value;
	};
	vt* vt;
};


struct zIBuffer
{
	struct vt
	{
		vt_iinspectable base;
		zfnc _6;
		zfLength Length;
	};
	vt* vt;
};


} // namespace bt
