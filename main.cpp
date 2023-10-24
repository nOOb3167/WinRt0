// https://gist.github.com/socantre/111644880358c39b4998
// https://blog.xojo.com/2019/07/02/accessing-windows-runtime-winrt/
// https://bytes.com/topic/c/answers/942922-virtual-pointer-accessing-vtable-class-inside-program

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

namespace wrl = Microsoft::WRL;
namespace wrlw = Microsoft::WRL::Wrappers;

using namespace std;

typedef void (*zfnc)(void);
typedef HRESULT (*zgcs)(IUnknown* thiz, HSTRING* ret);

#define WSTR2STR(x) (std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(x))

zgcs zGetCalendarSystem;

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

	zgcs** obj = (zgcs**)iu0.Get();
	zgcs*  vta = *obj;
	zgcs*  vta12 = vta + 12;
	zgcs   gcs = *vta12;

	auto rr = make_unique<HSTRING>();
	if (FAILED(gcs(iu0.Get(), rr.get())))
		throw std::runtime_error("");

	println(cout, "{}", WSTR2STR(WindowsGetStringRawBuffer(*rr, NULL)));
}


int main(void)
{
	stuff();

	return 0;
}
