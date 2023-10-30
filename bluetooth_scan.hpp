#include <thread>
#include <mutex>
#include <regex>
#include <vector>

#include <stdint.h>

#include <bluetooth_com_helper.hpp>


namespace bt
{


class ScannedDevice
{
public:
	uint64_t m_bluetoothAddress;
	std::string m_localName;
public:
	ScannedDevice(uint64_t bluetoothAddress, std::string localName) : m_bluetoothAddress(bluetoothAddress), m_localName(localName) {}
};


class Scanner
{
public:
	std::vector<ScannedDevice> m_devices;
	MCompleted m_completed;

	wrl::ComPtr<IInspectable> m_watcher;
public:

	Scanner();

	void wait();

private:
	static wrl::ComPtr<IInspectable> _makeWatcher();

	bool _isI330R(std::string localName);
};


Scanner::Scanner() : m_watcher(_makeWatcher())
{
	zEventRegistrationToken token_unused;

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(m_watcher)->ScanningMode_Set(m_watcher.Get(), (int32_t)zBluetoothLEScanningMode::Active));

	wrl::ComPtr<ComHandler_ITypedEventHandler_BluetoothLEAdvertisementWatcher_BluetoothLEAdvertisementReceivedEventArgs> cb = new ComHandler_ITypedEventHandler_BluetoothLEAdvertisementWatcher_BluetoothLEAdvertisementReceivedEventArgs(
		[this](uint64_t bluetoothAddress, std::string localName) {
			m_devices.push_back(ScannedDevice(bluetoothAddress, localName));
			if (_isI330R(localName))
				m_completed.signal();
		}
	);

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(m_watcher)->Received(m_watcher.Get(), cb.Get(), &token_unused));

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(m_watcher)->Start(m_watcher.Get()));
}


void Scanner::wait()
{
	m_completed.wait();

	CHK(GetVt<zIBluetoothLEAdvertisementWatcher>(m_watcher)->Stop(m_watcher.Get()));
}


wrl::ComPtr<IInspectable> Scanner::_makeWatcher()
{
	wrl::ComPtr<IInspectable> watcher;

	CHK(RoActivateInstance(wrlw::HStringReference(L"Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher").Get(), &watcher));
	CHK(ComIsA(uuidIBluetoothLEAdvertisementWatcher, watcher));

	return watcher;
}


bool Scanner::_isI330R(std::string localName) {
	// is what counts as a digit locale dependant?
	std::regex r("^GD\\d\\d\\d\\d\\d\\d$", std::regex_constants::ECMAScript);
	return std::regex_search(localName, r);
}


} // namespace bt
