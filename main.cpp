// https://gist.github.com/socantre/111644880358c39b4998
// https://blog.xojo.com/2019/07/02/accessing-windows-runtime-winrt/
// https://bytes.com/topic/c/answers/942922-virtual-pointer-accessing-vtable-class-inside-program

#include <stdexcept>
#include <string>
#include <stdint.h>

#include <Windows.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;
namespace wrlw = Microsoft::WRL::Wrappers;

using namespace std;

typedef void (*zfnc)(void);
typedef HRESULT (*zgcs)(IUnknown* thiz, HSTRING* ret);

zgcs zGetCalendarSystem;

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
	wrlw::HString className;
	if (FAILED(className.Set(L"Windows.Globalization.Calendar")))
		throw std::runtime_error("");

	wrl::ComPtr<IInspectable> ii0;
	if (FAILED(RoActivateInstance(className, &ii0)))
		throw std::runtime_error("");

	wrl::ComPtr<IUnknown> iu0;
	if (FAILED(ii0.As(&iu0)))
		throw std::runtime_error("");

	wrl::ComPtr<IUnknown> iu1;
	if (FAILED(iu0.AsIID(mkuuid("CA30221D-86D9-40FB-A26B-D44EB7CF08EA"), &iu1)))
		throw std::runtime_error("");

	IUnknown* zzz = iu1.Get();

	zgcs** obj = (zgcs**)iu1.Get();
	zgcs*  vta = *obj;
	zgcs*  vta12 = vta + 12;
	zgcs   gcs = *vta12;

	//uint64_t *vtab = (uint64_t*)iu1.Get();
	//vtab = (uint64_t*)*vtab;
	//zgcs gcs = *(zgcs*)(vtab + 12);
	HSTRING rr;
	if (FAILED(gcs(iu1.Get(), &rr)))
		throw std::runtime_error("");
}


int main(void)
{
	stuff();

	return 0;
}
