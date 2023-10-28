#include <codecvt>
#include <condition_variable>
#include <functional>
#include <locale>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

#include <roapi.h>
#include <windows.h>
#include <wrl.h>


#include <bluetooth_com_defs.hpp>


#define WSTR2STR(x) (std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(x))
#define CHK(x) { if (FAILED(x)) throw std::runtime_error("Failure[" #x "]"); }


namespace bt {


namespace wrl = Microsoft::WRL;
namespace wrlw = Microsoft::WRL::Wrappers;


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


static void deleteHSTRING(HSTRING p)
{
	WindowsDeleteString(p);
}


static HRESULT ComIsA(const UUID &uuid, IUnknown *obj)
{
	wrl::ComPtr<IUnknown> tmp;
	return obj->QueryInterface(uuid, &tmp);
}


template<typename T>
HRESULT ComIsA(const UUID &uuid, const wrl::ComPtr<T> &obj)
{
	wrl::ComPtr<IUnknown> tmp;
	return obj->QueryInterface(uuid, &tmp);
}


template<typename T>
decltype(T::vt) GetVt(IUnknown* u) {
	return ((T*)u)->vt;
}


template<typename T, typename U>
decltype(T::vt) GetVt(wrl::ComPtr<U> u) {
	return ((T*)u.Get())->vt;
}


class ComBase : public IUnknown
{
protected:
	volatile long m_ref;
	std::vector<UUID> m_implementedUUIDs;
public:
	ComBase(const std::vector<UUID>& implementedUUIDs) : m_ref(1), m_implementedUUIDs(implementedUUIDs)
	{
		m_implementedUUIDs.push_back(IID_IUnknown);
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
	{
		wrl::ComPtr<IUnknown> p(this);
		*ppv = nullptr;
		for (auto&& uuid : m_implementedUUIDs)
			if (uuid == riid)
				*ppv = this;
		return *ppv ? (AddRef(), S_OK) : E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override { InterlockedIncrement(&m_ref); return this->m_ref; }
	ULONG STDMETHODCALLTYPE Release(void) override { InterlockedDecrement(&m_ref); if (m_ref > 0) return m_ref; delete this; return 0; }
};


class ComHandler_ITypedEventHandler_BluetoothLEAdvertisementWatcher_BluetoothLEAdvertisementReceivedEventArgs : public ComBase
{
public:
	typedef std::function<void(uint64_t bluetoothAddress, std::string localName)> cb_t;
	cb_t m_cb;

public:
	ComHandler_ITypedEventHandler_BluetoothLEAdvertisementWatcher_BluetoothLEAdvertisementReceivedEventArgs(cb_t cb) : ComBase({ uuidTypedEventHandlerReceivedTEH }), m_cb(cb) {}

	virtual HRESULT STDMETHODCALLTYPE Invoke(IUnknown *watcher, IInspectable *args)
	{
		wrl::ComPtr<IUnknown> args1;
		wrl::ComPtr<IUnknown> args2;
		
		wrl::ComPtr<IUnknown> advertisement;

		uint64_t address;

		HSTRING localName;

		CHK(ComIsA(uuidIBluetoothLEAdvertisementWatcher, watcher));
		CHK(args->QueryInterface(uuidIBluetoothLEAdvertisementReceivedEventArgs, &args1));
		CHK(args->QueryInterface(uuidIBluetoothLEAdvertisementReceivedEventArgs2, &args2));

		CHK(GetVt<zIBluetoothLEAdvertisementReceivedEventArgs>(args1)->BluetoothAddress(args1.Get(), &address));
		
		CHK(GetVt<zIBluetoothLEAdvertisementReceivedEventArgs>(args1)->Advertisement(args1.Get(), &advertisement));

		CHK(ComIsA(uuidIBluetoothLEAdvertisement, advertisement));

		CHK(GetVt<zIBluetoothLEAdvertisement>(advertisement)->LocalName(advertisement.Get(), &localName));

		const wchar_t* localName_ = WindowsGetStringRawBuffer(localName, nullptr);
		std::string localName__ = WSTR2STR(std::wstring(localName_));

		m_cb(address, localName__);

		return S_OK;
	}
};


class ComHandler_IAsyncOperationCompletedHandler__BluetoothLEDevice__ : public ComBase
{
public:
	typedef std::function<void()> cb_t;
	cb_t m_cb;

public:
	ComHandler_IAsyncOperationCompletedHandler__BluetoothLEDevice__(cb_t cb) : ComBase({ uuidIAsyncOperationCompletedHandler__BluetoothLEDevice__ }), m_cb(cb) {}

	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable *op) {
		CHK(ComIsA(uuidIAsyncOperation__BluetoothLEDevice__, op));

		m_cb();

		return S_OK;
	}
};


class ComHandler_IAsyncOperationCompletedHandler__GattDeviceServicesResult_star__ : public ComBase
{
public:
	typedef std::function<void()> cb_t;
	cb_t m_cb;

public:
	ComHandler_IAsyncOperationCompletedHandler__GattDeviceServicesResult_star__(cb_t cb) : ComBase({ uuidIAsyncOperationCompletedHandler__GattDeviceServicesResult_star__ }), m_cb(cb) {}

	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable *op) {
		CHK(ComIsA(uuidIAsyncOperation__GattDeviceServicesResult_star__, op));

		m_cb();

		return S_OK;
	}
};


class ComHandler_IAsyncOperationCompletedHandler__GetCharacteristicsResult_star__ : public ComBase
{
public:
	typedef std::function<void()> cb_t;
	cb_t m_cb;

public:
	ComHandler_IAsyncOperationCompletedHandler__GetCharacteristicsResult_star__(cb_t cb) : ComBase({ uuidIAsyncOperationCompletedHandler__GetCharacteristicsResult_star__ }), m_cb(cb) {}

	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable *op) {
		CHK(ComIsA(uuidIAsyncOperation__GetCharacteristicsResult_star__, op));

		m_cb();

		return S_OK;
	}
};


class ComHandlerWaitable_IAsyncOperation : public ComBase
{
public:
	typedef std::function<void()> cb_t;
	cb_t m_cb;

	MCompleted m_completed;
	UUID m_uuidIAsyncOperation__XXXX__;
	UUID m_uuidIAsyncOperationCompletedHandler__XXXX__;

public:
	ComHandlerWaitable_IAsyncOperation(UUID uuidIAsyncOperation__XXXX__, UUID uuidIAsyncOperationCompletedHandler__XXXX__) : ComBase({ uuidIAsyncOperationCompletedHandler__XXXX__ }), m_uuidIAsyncOperation__XXXX__(uuidIAsyncOperation__XXXX__) {}

	virtual HRESULT STDMETHODCALLTYPE Invoke(IInspectable *op) {
		CHK(ComIsA(m_uuidIAsyncOperation__XXXX__, op));

		m_completed.signal();

		return S_OK;
	}

	void wait() {
		m_completed.wait();
	}
};


} // namespace bt
