#define HANA_LOG_ENABLE
#include <hana/log.hpp>
#include <hana/dll/module_manager.hpp>

namespace DividedByZero
{
	void run() {
		// int a = 10 / 0;
	}
}

namespace SegFault
{
	void run() {
		int* ptr = nullptr;
		*ptr = 10;
	}
}

namespace StackOverflow
{
	int f(int a = 0) {
		if (a > 1e5) return a + 4;
		return f(a + 1) + f(a + 2) + f(a + 3);
	}

	void run() {
		int h = f();
		LOG_INFO(u8"{}", h);
	}
}

namespace Terminate
{
	void run() {
		terminate();
	}
}

namespace PureVirtual
{
	class CDerived;
	class CBase {
	public:
		explicit CBase(CDerived* derived): m_pDerived(derived) {};
		virtual ~CBase();
		virtual void function() = 0;

		CDerived* m_pDerived;
	};

	class CDerived : public CBase {
	public:
		CDerived() : CBase(this) {}; // C4355
		void function() override {};
	};

	CBase::~CBase() {
		m_pDerived->function();
	}

	void run() {
		CDerived myDerived;
	}
}

namespace OpNewError
{
	struct A {
		char byte[10000];
	};

	void run() {
		A* a = new A[100000000000000];
		delete a;
	}
}

namespace InvalidParam
{
	void run() {
		try {
			char* formatString = NULL;
			printf(formatString);
		} catch (...) {}
	}
}

int main(int argc, char8_t** argv) {
	using namespace hana;
	ModuleManager::set_root();
	ModuleManager::load(u8"HanaBase", true);

	LogSystem::set_header_pattern(u8"[{l}] {M}");
	Terminate::run();
}
