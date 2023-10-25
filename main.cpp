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

#include <memory>
#include <locale>
#include <codecvt>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <stdint.h>

#include <windows.h>
#include <wrl.h>

#include <windows.foundation.h>
#include <windows.devices.bluetooth.advertisement.h>

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

typedef void (*zfnc)(void);
typedef HRESULT (*zgcs)(IUnknown* thiz, HSTRING* ret);
typedef ABI::Windows::Foundation::ITypedEventHandler<ADV::BluetoothLEAdvertisementWatcher*, ADV::BluetoothLEAdvertisementReceivedEventArgs*> zfReceivedTEH;

typedef HRESULT (*zfScanningMode_Set)(IUnknown* thiz, int32_t bluetoothLEScanningMode);
typedef HRESULT(*zfStart)(IUnknown* thiz);
//typedef void (*zfReceived)(zfReceivedTEH* handler, void *);
typedef HRESULT (*zfReceived)(IUnknown *thiz, zfReceivedTEH* handler, EventRegistrationToken *tok);


#define WSTR2STR(x) (std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(x))


[[maybe_unused]] UUID uuidTypedEventHandler = { 2648818996, 27361, 4576, 132, 225, 24, 169, 5, 188, 197, 63 }; // dont remember where i got this one
UUID uuidTypedEventHandlerReceivedTEH = { 2431340234, 54373, 24224,  166, 28, 3, 60, 140, 94, 206, 242 };

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
		zgcs GetCalendarSystem;
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
		zfnc _18;
		zfReceived Received;
	};
	vt* vt;
};


void d_hstring(HSTRING p) {
	WindowsDeleteString(p);
}

UUID mkuuid(std::string s)
{
	UUID u = {};
	if (UuidFromStringA((unsigned char*)s.data(), &u) != RPC_S_OK)
		throw std::runtime_error("");
	return u;
}


class ICb : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable* sender, IInspectable* args) = 0;
};

class Cb : public ICb
{
protected:
	volatile long m_ref;
public:
	Cb() : m_ref(1) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
		*ppv = NULL;
		if (!(riid == IID_IUnknown || riid == uuidTypedEventHandlerReceivedTEH))
			return E_NOINTERFACE;
		this->AddRef();
		*ppv = this;

		return S_OK;
	}
	ULONG   STDMETHODCALLTYPE AddRef(void) override {
		InterlockedIncrement(&m_ref);
		return this->m_ref;
	}
	ULONG   STDMETHODCALLTYPE Release(void) override {
		InterlockedDecrement(&m_ref);
		if (m_ref > 0)
			return m_ref;
  		delete this;
  		return 0;
	}
	HRESULT STDMETHODCALLTYPE Invoke(IInspectable *sender, IInspectable *args) override {
		println(cout, "Invoke");
		throw std::runtime_error("");
	}
};


void stuff()
{
	wrlw::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	wrlw::HStringReference className(L"Windows.Globalization.Calendar");

	wrl::ComPtr<IInspectable> ii0;
	if (FAILED(RoActivateInstance(className.Get(), &ii0)))
		throw std::runtime_error("");

	wrl::ComPtr<IUnknown> iu0;
	if (FAILED(ii0.AsIID(mkuuid("CA30221D-86D9-40FB-A26B-D44EB7CF08EA"), &iu0)))
		throw std::runtime_error("");

	auto rr = make_unique<HSTRING>();
	if (FAILED(((zICalendar*)iu0.Get())->vt->GetCalendarSystem(iu0.Get(), rr.get())))
		throw std::runtime_error("");

	println(cout, "{}", WSTR2STR(WindowsGetStringRawBuffer(*rr, NULL)));


	wrlw::HStringReference watcher_className(L"Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher");

	wrl::ComPtr<IInspectable> watcher_ii;
	if (FAILED(RoActivateInstance(watcher_className.Get(), &watcher_ii)))
		throw std::runtime_error("");

	UUID watcher_uuid_IBluetoothLEAdvertisementWatcher = mkuuid("A6AC336F-F3D3-4297-8D6C-C81EA6623F40");
	wrl::ComPtr<IUnknown> watcher_iu;
	if (FAILED(watcher_ii.AsIID(watcher_uuid_IBluetoothLEAdvertisementWatcher, &watcher_iu)))
		throw std::runtime_error("");

	((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->ScanningMode_Set(watcher_iu.Get(), (int32_t)zBluetoothLEScanningMode::Active);

	class Z
	{
	public:
		HRESULT Received(ADV::IBluetoothLEAdvertisementWatcher* sender, ADV::IBluetoothLEAdvertisementReceivedEventArgs* args)
		{
			println(cout, "Received");
			return 0;
		};
	};

	Z z;

	// HRESULT E_POINTER 0x80004003
	// [eventadd] HRESULT Received([in] Windows.Foundation.TypedEventHandler<Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher*, Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementReceivedEventArgs*>* handler, [out][retval] EventRegistrationToken* token);
	// is '[out][retval] EventRegistrationToken* token' mapped as an argument or as a return value? debugger says argument?
	auto zzz = ((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->Received;
	auto zzz2 = ((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->Start;

	//((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->Start(wrl::Callback<zfReceivedTEH>(&z, &Z::Received).Get());

	// argument to Received has to be an ITypedEventHandler not a ComPtr
	wrl::ComPtr<zfReceivedTEH> q = wrl::Callback<zfReceivedTEH>(&z, &Z::Received);

	Cb* cb = new Cb();

	EventRegistrationToken tok;
	HRESULT r = ((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->Received(watcher_iu.Get(), (zfReceivedTEH *)cb, &tok);
	//HRESULT r = ((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->Received(watcher_iu.Get(), q.Get(), &tok);
	((zIBluetoothLEAdvertisementWatcher*)watcher_iu.Get())->vt->Start(watcher_iu.Get());

	Sleep(10000);

	println(cout, "");
}


int main(void)
{
	stuff();

	return 0;
}
