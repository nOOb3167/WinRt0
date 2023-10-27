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

#include <windows.foundation.h>
#include <windows.devices.bluetooth.advertisement.h>
#include <windows.devices.bluetooth.genericattributeprofile.h>

#define WSTR2STR(x) (std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(x))
#define CHK(x) { if (FAILED(x)) throw std::runtime_error("Failure[" #x "]"); }

UUID mkuuid(std::string s);
void d_hstring(HSTRING p);

namespace wrl = Microsoft::WRL;
namespace wrlw = Microsoft::WRL::Wrappers;
namespace ADV = ABI::Windows::Devices::Bluetooth::Advertisement;

using namespace std;

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

typedef void (*zfnc)(void);
typedef HRESULT (*zGetCalendarSystem)(IUnknown* thiz, HSTRING* ret);
typedef ABI::Windows::Foundation::ITypedEventHandler<ADV::BluetoothLEAdvertisementWatcher*, ADV::BluetoothLEAdvertisementReceivedEventArgs*> zfReceivedTEH;

typedef HRESULT (*zfScanningMode_Set)(IUnknown* thiz, int32_t bluetoothLEScanningMode);
typedef HRESULT (*zfStart)(IUnknown* thiz);
typedef HRESULT (*zfStop)(IUnknown* thiz);
typedef HRESULT (*zfReceived)(IUnknown *thiz, IUnknown* handler, EventRegistrationToken *tok);
typedef HRESULT (*zfBluetoothAddress)(IUnknown* thiz, uint64_t *value);
typedef HRESULT(*zfFromBluetoothAddressAsync)(IUnknown* thiz, uint64_t bluetoothAddress, IUnknown** out);
typedef HRESULT (*zfGetGattServicesWithCacheModeAsync)(IUnknown* thiz, int32_t bluetoothCacheMode, IUnknown** out);
typedef HRESULT (*zfPut_Completed)(IUnknown* thiz, IUnknown* handler);
typedef HRESULT (*zfGetResults)(IUnknown* thiz, IUnknown** out);

UUID uuidTypedEventHandlerReceivedTEH = { 2431340234, 54373, 24224,  166, 28, 3, 60, 140, 94, 206, 242 }; // __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ADV::BluetoothLEAdvertisementWatcher*, ADV::BluetoothLEAdvertisementReceivedEventArgs*>)
UUID uuidIBluetoothLEAdvertisementWatcher = mkuuid("A6AC336F-F3D3-4297-8D6C-C81EA6623F40");
UUID uuidIBluetoothLEAdvertisementReceivedEventArgs = mkuuid("27987DDF-E596-41BE-8D43-9E6731D4A913");
UUID uuidIBluetoothLEAdvertisementReceivedEventArgs2 = mkuuid("12D9C87B-0399-5F0E-A348-53B02B6B162E");
UUID uuidIBluetoothLEDeviceStatics = mkuuid("C8CF1A19-F0B6-4BF0-8689-41303DE2D9F4");
UUID uuidIBluetoothLEDevice3 = mkuuid("AEE9E493-44AC-40DC-AF33-B2C13C01CA46");
UUID uuidIAsyncOperation__BluetoothLEDevice__ = { 929013095, 29858, 24465, 161, 29, 22, 144, 147, 113, 141, 65 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*>)
UUID uuidIAsyncOperationCompletedHandler__BluetoothLEDevice__ = { 2438379423, 50506, 21111, 143, 139, 210, 204, 67, 199, 224, 4 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*>)
//wrong UUID uuidIAsyncOperation__IGattDeviceServicesResult__ = { 387830766, 365, 16797, 131, 138, 87, 108, 244, 117, 163, 216 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult>)
UUID uuidIGattDeviceServicesResult = mkuuid("171DD3EE-016D-419D-838A-576CF475A3D8"); // Windows.Devices.Bluetooth.GenericAttributeProfile.0.h
UUID uuidIAsyncOperation__GattDeviceServicesResult_star__ = { 3888539638, 59508, 20495, 134, 255, 118, 12, 166, 240, 122, 88 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult*>)
UUID uuidIAsyncOperationCompletedHandler__GattDeviceServicesResult_star__ = { 1957365906, 42545, 23916, 177, 180, 189, 46, 26, 116, 26, 155 }; // __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult*>)
UUID uuidR = __uuidof(ABI::Windows::Foundation::IAsyncOperationCompletedHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult*>);
UUID uuidR3 = __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult*>);

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

struct zICalendar
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
		zGetCalendarSystem GetCalendarSystem;
	};
	vt* vt;
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


template<typename T, typename U>
decltype(T::vt) GetVt(wrl::ComPtr<U> u) { return ((T*)u.Get())->vt; }


void d_hstring(HSTRING p)
{
	WindowsDeleteString(p);
}

UUID mkuuid(std::string s)
{
	UUID u = {};
	if (UuidFromStringA((unsigned char*)s.data(), &u) != RPC_S_OK)
		throw std::runtime_error("");
	return u;
}


class MCompleted
{
public:
	std::mutex m_m;
	std::condition_variable m_cv;
	bool m_completed = false;

	void signal() {
		{
			std::unique_lock l(m_m);
			m_completed = true;
		}
		m_cv.notify_all();
	}

	void wait() {
		std::unique_lock l(m_m);
		m_cv.wait(l, [this]() { return m_completed == true; });
	}
};


class Cb : public IUnknown
{
protected:
	volatile long m_ref;
public:
	MCompleted m_completed;
public:
	Cb() : m_ref(1) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override { return (*ppv = (riid == IID_IUnknown || riid == uuidTypedEventHandlerReceivedTEH) ? (this->AddRef(), this) : nullptr) ? S_OK : E_NOINTERFACE; }
	ULONG   STDMETHODCALLTYPE AddRef(void) override { InterlockedIncrement(&m_ref); return this->m_ref; }
	ULONG   STDMETHODCALLTYPE Release(void) override { InterlockedDecrement(&m_ref); if (m_ref > 0) return m_ref; delete this; return 0; }
	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable *sender, IInspectable *args) {
		wrl::ComPtr<IUnknown> watcher;
		wrl::ComPtr<IUnknown> args1;
		wrl::ComPtr<IUnknown> args2;

		CHK(sender->QueryInterface(uuidIBluetoothLEAdvertisementWatcher, &watcher));
		CHK(args->QueryInterface(uuidIBluetoothLEAdvertisementReceivedEventArgs, &args1));
		CHK(args->QueryInterface(uuidIBluetoothLEAdvertisementReceivedEventArgs2, &args2));

		CHK(GetVt<zIBluetoothLEAdvertisementReceivedEventArgs>(args1)->BluetoothAddress(args1.Get(), &m_addr));

		m_completed.signal();

		return S_OK;
	}

	uint64_t m_addr = 0;
};


class Cb2 : public IUnknown
{
protected:
	volatile long m_ref;
public:
	MCompleted m_completed;
public:
	Cb2() : m_ref(1) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override { return (*ppv = (riid == IID_IUnknown || riid == uuidIAsyncOperationCompletedHandler__BluetoothLEDevice__) ? (this->AddRef(), this) : nullptr) ? S_OK : E_NOINTERFACE; }
	ULONG   STDMETHODCALLTYPE AddRef(void) override { InterlockedIncrement(&m_ref); return this->m_ref; }
	ULONG   STDMETHODCALLTYPE Release(void) override { InterlockedDecrement(&m_ref); if (m_ref > 0) return m_ref; delete this; return 0; }
	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable *sender) {
		wrl::ComPtr<IUnknown> ledev;

		CHK(sender->QueryInterface(uuidIAsyncOperation__BluetoothLEDevice__, &ledev));

		m_completed.signal();

		return S_OK;
	}
};


class Cb3 : public IUnknown
{
protected:
	volatile long m_ref;
public:
	MCompleted m_completed;
public:
	Cb3() : m_ref(1) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override { return (*ppv = (riid == IID_IUnknown || riid == uuidIAsyncOperationCompletedHandler__GattDeviceServicesResult_star__) ? (this->AddRef(), this) : nullptr) ? S_OK : E_NOINTERFACE; }
	ULONG   STDMETHODCALLTYPE AddRef(void) override { InterlockedIncrement(&m_ref); return this->m_ref; }
	ULONG   STDMETHODCALLTYPE Release(void) override { InterlockedDecrement(&m_ref); if (m_ref > 0) return m_ref; delete this; return 0; }
	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable* sender) {
		wrl::ComPtr<IUnknown> op;
		
		CHK(sender->QueryInterface(uuidIAsyncOperation__GattDeviceServicesResult_star__, &op));

		m_completed.signal();

		return S_OK;
	}
};


void stuff()
{
	wrlw::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

	wrl::ComPtr<IInspectable> watcher;

	CHK(RoActivateInstance(wrlw::HStringReference(L"Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher").Get(), &watcher));

	GetVt<zIBluetoothLEAdvertisementWatcher>(watcher)->ScanningMode_Set(watcher.Get(), (int32_t)zBluetoothLEScanningMode::Active);

	wrl::ComPtr<Cb> cb = new Cb();
	wrl::ComPtr<Cb2> cb2 = new Cb2();
	wrl::ComPtr<Cb3> cb3 = new Cb3();
	
	EventRegistrationToken tok;

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(watcher)->Received(watcher.Get(), cb.Get(), &tok));

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(watcher)->Start(watcher.Get()));

	cb->m_completed.wait();

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(watcher)->Stop(watcher.Get()));

	wrl::ComPtr<IUnknown> ledevicesta;
	CHK(RoGetActivationFactory(wrlw::HString::MakeReference(L"Windows.Devices.Bluetooth.BluetoothLEDevice").Get(), uuidIBluetoothLEDeviceStatics, &ledevicesta));

	wrl::ComPtr<IUnknown> fbaa_op;
	CHK(GetVt<zIBluetoothLEDeviceStatics>(ledevicesta)->FromBluetoothAddressAsync(ledevicesta.Get(), cb->m_addr, &fbaa_op));

	CHK(GetVt<zIAsyncOperation>(fbaa_op)->Put_Completed(fbaa_op.Get(), cb2.Get()));

	cb2->m_completed.wait();

	wrl::ComPtr<IUnknown> ledev;
	wrl::ComPtr<IUnknown> ledev3;

	CHK(GetVt<zIAsyncOperation>(fbaa_op)->GetResults(fbaa_op.Get(), &ledev));
	
	CHK(ledev->QueryInterface(uuidIBluetoothLEDevice3, &ledev3));

	wrl::ComPtr<IUnknown> ggswcma_op;
	CHK(GetVt<zIBluetoothLEDevice3>(ledev3)->GetGattServicesWithCacheModeAsync(ledev3.Get(), (int32_t)zBluetoothCacheMode::Uncached, &ggswcma_op));

	wrl::ComPtr<IUnknown> qqq;
	CHK(ggswcma_op->QueryInterface(uuidIAsyncOperation__GattDeviceServicesResult_star__, &qqq));

	CHK(GetVt<zIAsyncOperation>(qqq)->Put_Completed(qqq.Get(), cb3.Get()));

	cb3->m_completed.wait();

	println(cout, "");
}


int main(void)
{
	stuff();

	return 0;
}
