#include <doctest/doctest.h>
#include <hana/memory/rc.hpp>

static uint64_t rc_base_count = 0;
static uint64_t rc_derived_count = 0;
static uint64_t rc_custom_deleter_count = 0;

struct TestRCBase : hana::RCWeakInterface {
	TestRCBase() {
		++rc_base_count;
	}

	virtual ~TestRCBase() {
		--rc_base_count;
	}
};

struct TestRCDerived : TestRCBase {
	TestRCDerived() {
		++rc_derived_count;
	}

	~TestRCDerived() override {
		--rc_derived_count;
	}
};

struct TestCustomDeleterRC : hana::RCWeakInterface {
	void rc_delete() const {
		++rc_custom_deleter_count;
		delete this;
	}
};

TEST_CASE("Test RCUnique") {
	using namespace hana;

	SUBCASE("ctor & dtor") {
		RCUnique<TestRCBase> empty_ctor{};
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(empty_ctor.get(), nullptr);
		CHECK_EQ(empty_ctor.weak_ref_count(), 0);

		RCUnique<TestRCBase> null_ctor{nullptr};
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(null_ctor.get(), nullptr);
		CHECK_EQ(null_ctor.weak_ref_count(), 0);
		//
		{
			RCUnique<TestRCBase> pointer_ctor{new TestRCBase};
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK_NE(pointer_ctor.get(), nullptr);
			CHECK_EQ(pointer_ctor.weak_ref_count(), 0);
		}

		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
	}

	SUBCASE("copy & move") {
		RCUnique<TestRCBase> base{new TestRCBase};
		RCUnique<TestRCDerived> derived{new TestRCDerived};

		auto cached_base = base.get();
		RCUnique<TestRCBase> move_base{std::move(base)};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.get(), nullptr);
		CHECK_EQ(move_base.get(), cached_base);

		auto cached_derived = derived.get();
		RCUnique<TestRCBase> move_derived{std::move(derived)};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(derived.get(), nullptr);
		CHECK_EQ(move_derived.get(), cached_derived);

		move_base = nullptr;
		move_derived = nullptr;

		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
	}

	SUBCASE("assign & move assign") {
		RCUnique<TestRCBase> base{new TestRCBase};
		RCUnique<TestRCDerived> derived{new TestRCDerived};

		RCUnique<TestRCBase> move_assign{};

		// move assign base
		auto cached_base = base.get();
		move_assign = std::move(base);
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.get(), nullptr);
		CHECK_EQ(move_assign.get(), cached_base);

		// move assign derived
		auto cached_derived = derived.get();
		move_assign = std::move(derived);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(derived.get(), nullptr);
		CHECK_EQ(move_assign.get(), cached_derived);
		CHECK_EQ(derived.get(), nullptr);

		move_assign = nullptr;
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(move_assign.get(), nullptr);
	}

	SUBCASE("test compare") {
		RCUnique<TestRCBase> base1{new TestRCBase};
		RCUnique<TestRCBase> base2{new TestRCBase};
		RCUnique<TestRCDerived> derived1{new TestRCDerived};
		RCUnique<TestRCDerived> derived2{new TestRCDerived};

		// eq
		CHECK_EQ(base1 == base1, true);
		CHECK_EQ(base1 == base2, false);
		CHECK_EQ(base1 == derived1, false);

		// neq
		CHECK_EQ(base1 != base1, false);
		CHECK_EQ(base1 != base2, true);
		CHECK_EQ(base1 != derived1, true);

		// nullptr
		CHECK_EQ(base1 == nullptr, false);
		CHECK_EQ(nullptr == base1, false);
		CHECK_EQ(base1 != nullptr, true);
		CHECK_EQ(nullptr != base1, true);

		// lt (just test for compile)
		CHECK_EQ(base1 < base1, false);

		// gt (just test for compile)
		CHECK_EQ(base1 > base1, false);

		// le (just test for compile)
		CHECK_EQ(base1 <= base1, true);

		// ge (just test for compile)
		CHECK_EQ(base1 >= base1, true);
	}

	SUBCASE("test is empty") {
		RCUnique<TestRCBase> empty{};
		RCUnique<TestRCBase> something{new TestRCBase};

		CHECK_EQ(static_cast<bool>(empty), false);
		CHECK_EQ(static_cast<bool>(something), true);
	}

	SUBCASE("test ops") {
		RCUnique<TestRCBase> rc{new TestRCBase};
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(rc.get(), nullptr);

		rc.reset();
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(rc.get(), nullptr);

		rc.reset(new TestRCBase);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(rc.get(), nullptr);

		RCUnique<TestRCBase> other;
		rc.swap(other);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(rc.get(), nullptr);
		CHECK_NE(other.get(), nullptr);

		other.reset(new TestRCBase);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(other.get(), nullptr);

		auto* cached_other = other.get();
		auto* released_other = other.release();
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(other.get(), nullptr);
		CHECK_EQ(released_other, cached_other);
		delete released_other;
	}

	SUBCASE("test empty") {
		RCUnique<TestRCBase> empty{};
		RCUnique<TestRCBase> empty_move{std::move(empty)};
		RCUnique<TestRCBase> empty_move_assign;
		empty_move_assign = std::move(empty);

		empty.get();
		empty.weak_ref_count();
		empty.operator bool();
		empty.reset();
		empty.reset(nullptr);
		empty.swap(empty_move);
		empty.release();
	}
}

TEST_CASE("Test RC") {
	using namespace hana;

	SUBCASE("ctor & dtor") {
		RC<TestRCBase> empty_ctor{};
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(empty_ctor.get(), nullptr);
		CHECK_EQ(empty_ctor.ref_count(), 0);
		CHECK_EQ(empty_ctor.weak_ref_count(), 0);

		RC<TestRCBase> null_ctor{nullptr};
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(null_ctor.get(), nullptr);
		CHECK_EQ(null_ctor.ref_count(), 0);
		CHECK_EQ(null_ctor.weak_ref_count(), 0);
		//
		{
			RC<TestRCBase> pointer_ctor{new TestRCBase};
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK_NE(pointer_ctor.get(), nullptr);
			CHECK_EQ(pointer_ctor.ref_count(), 1);
			CHECK_EQ(pointer_ctor.weak_ref_count(), 0);
		}
		//
		{
			RCUnique<TestRCBase> unique{new TestRCBase};
			RC<TestRCBase> rc{std::move(unique)};
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK_NE(rc.get(), nullptr);
			CHECK_EQ(rc.ref_count(), 1);
			CHECK_EQ(rc.weak_ref_count(), 0);

			CHECK_EQ(unique.get(), nullptr);
		}

		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
	}

	SUBCASE("copy & move") {
		RC<TestRCBase> base{new TestRCBase};
		RC<TestRCDerived> derived{new TestRCDerived};

		RC<TestRCBase> copy_base{base};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.get(), copy_base.get());
		CHECK_EQ(base.ref_count(), 2);
		CHECK_EQ(copy_base.ref_count(), 2);
		CHECK_EQ(base.weak_ref_count(), 0);
		CHECK_EQ(copy_base.weak_ref_count(), 0);

		RC<TestRCBase> copy_derived{derived};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(derived.get(), copy_derived.get());
		CHECK_EQ(derived.ref_count(), 2);
		CHECK_EQ(copy_derived.ref_count(), 2);
		CHECK_EQ(derived.weak_ref_count(), 0);
		CHECK_EQ(copy_derived.weak_ref_count(), 0);

		RC<TestRCBase> move_base{std::move(base)};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.get(), nullptr);
		CHECK_EQ(move_base.get(), copy_base.get());
		CHECK_EQ(base.ref_count(), 0);
		CHECK_EQ(copy_base.ref_count(), 2);
		CHECK_EQ(move_base.ref_count(), 2);
		CHECK_EQ(base.weak_ref_count(), 0);
		CHECK_EQ(copy_base.weak_ref_count(), 0);

		RC<TestRCBase> move_derived{std::move(derived)};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(derived.get(), nullptr);
		CHECK_EQ(move_derived.get(), copy_derived.get());
		CHECK_EQ(derived.ref_count(), 0);
		CHECK_EQ(copy_derived.ref_count(), 2);
		CHECK_EQ(move_derived.ref_count(), 2);
		CHECK_EQ(derived.weak_ref_count(), 0);
		CHECK_EQ(copy_derived.weak_ref_count(), 0);

		copy_base = nullptr;
		move_base = nullptr;
		copy_derived = nullptr;
		move_derived = nullptr;

		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
	}

	SUBCASE("assign & move assign") {
		RC<TestRCBase> base{new TestRCBase};
		RC<TestRCDerived> derived{new TestRCDerived};

		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.ref_count(), 1);
		CHECK_EQ(derived.ref_count(), 1);
		// assign
		{
			RC<TestRCBase> assign{};
			assign = base;
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(base.ref_count(), 2);
			CHECK_EQ(derived.ref_count(), 1);
			CHECK_EQ(assign.ref_count(), 2);
			CHECK_EQ(assign.get(), base.get());

			// assign derived
			assign = derived;
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(base.ref_count(), 1);
			CHECK_EQ(derived.ref_count(), 2);
			CHECK_EQ(assign.ref_count(), 2);
			CHECK_EQ(assign.get(), derived.get());
		}
		// move assign
		{
			RC<TestRCBase> move_assign{};
			auto cached_base = base.get();
			move_assign = std::move(base);
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(base.ref_count(), 0);
			CHECK_EQ(derived.ref_count(), 1);
			CHECK_EQ(move_assign.ref_count(), 1);
			CHECK_EQ(move_assign.get(), cached_base);
			CHECK_EQ(base.get(), nullptr);

			// move assign derived
			auto cached_derived = derived.get();
			move_assign = std::move(derived);
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(base.ref_count(), 0);
			CHECK_EQ(derived.ref_count(), 0);
			CHECK_EQ(move_assign.ref_count(), 1);
			CHECK_EQ(move_assign.get(), cached_derived);
			CHECK_EQ(derived.get(), nullptr);
		}
		//
		{
			RCUnique<TestRCDerived> unique{new TestRCDerived};
			RC<TestRCBase> move_assign_unique{};
			auto cached_unique = unique.get();
			move_assign_unique = std::move(unique);
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(base.ref_count(), 0);
			CHECK_EQ(derived.ref_count(), 0);
			CHECK_EQ(move_assign_unique.ref_count(), 1);
			CHECK_EQ(move_assign_unique.get(), cached_unique);
			CHECK_EQ(derived.get(), nullptr);
		}
	}

	SUBCASE("test compare") {
		RC<TestRCBase> base1{new TestRCBase};
		RC<TestRCBase> base2{new TestRCBase};
		RC<TestRCDerived> derived1{new TestRCDerived};
		RC<TestRCDerived> derived2{new TestRCDerived};

		RC<TestRCBase> copy_base1 = base1;
		RC<TestRCBase> copy_derived1{derived1};

		// eq
		CHECK_EQ(base1 == base1, true);
		CHECK_EQ(base1 == base2, false);
		CHECK_EQ(base1 == derived1, false);
		CHECK_EQ(base1 == copy_base1, true);
		CHECK_EQ(derived1 == copy_derived1, true);
		CHECK_EQ(derived2 == copy_derived1, false);

		// neq
		CHECK_EQ(base1 != base1, false);
		CHECK_EQ(base1 != base2, true);
		CHECK_EQ(base1 != derived1, true);
		CHECK_EQ(base1 != copy_base1, false);
		CHECK_EQ(derived1 != copy_derived1, false);
		CHECK_EQ(derived2 != copy_derived1, true);

		// nullptr
		CHECK_EQ(base1 == nullptr, false);
		CHECK_EQ(nullptr == base1, false);
		CHECK_EQ(base1 != nullptr, true);
		CHECK_EQ(nullptr != base1, true);

		// lt (just test for compile)
		CHECK_EQ(base1 < base1, false);
		CHECK_EQ(derived1 < copy_derived1, false);
		CHECK_EQ(copy_derived1 < derived1, false);

		// gt (just test for compile)
		CHECK_EQ(base1 > base1, false);
		CHECK_EQ(derived1 > copy_derived1, false);
		CHECK_EQ(copy_derived1 > derived1, false);

		// le (just test for compile)
		CHECK_EQ(base1 <= base1, true);
		CHECK_EQ(derived1 <= copy_derived1, true);
		CHECK_EQ(copy_derived1 <= derived1, true);

		// ge (just test for compile)
		CHECK_EQ(base1 >= base1, true);
		CHECK_EQ(derived1 >= copy_derived1, true);
		CHECK_EQ(copy_derived1 >= derived1, true);
	}

	SUBCASE("test is empty") {
		RC<TestRCBase> empty{};
		RC<TestRCBase> something{new TestRCBase};

		CHECK_EQ(static_cast<bool>(empty), false);
		CHECK_EQ(static_cast<bool>(something), true);
	}

	SUBCASE("test ops") {
		RC<TestRCBase> rc{new TestRCBase};
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(rc.get(), nullptr);

		rc.reset();
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(rc.get(), nullptr);

		rc.reset(new TestRCBase);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(rc.get(), nullptr);

		RC<TestRCBase> other;
		rc.swap(other);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_EQ(rc.get(), nullptr);
		CHECK_NE(other.get(), nullptr);

		other.reset(new TestRCBase);
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(other.get(), nullptr);
	}

	SUBCASE("test empty") {
		RC<TestRCBase> empty{};
		RC<TestRCBase> empty_copy{empty};
		RC<TestRCBase> empty_move{std::move(empty)};
		RC<TestRCBase> empty_assign, empty_move_assign;
		empty_assign = empty;
		empty_move_assign = std::move(empty);

		empty.get();
		empty.ref_count();
		empty.weak_ref_count();
		empty.operator bool();
		empty.reset();
		empty.reset(nullptr);
		empty.swap(empty_copy);
	}
}

TEST_CASE("Test RCWeak") {
	using namespace hana;

	SUBCASE("test ctor") {
		RCWeak<TestRCBase> empty_ctor{};
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_FALSE(empty_ctor.lock());
		CHECK_EQ(empty_ctor.weak_ref_count(), 0);

		RCWeak<TestRCBase> null_ctor{nullptr};
		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_FALSE(null_ctor.lock());
		CHECK_EQ(null_ctor.weak_ref_count(), 0);

		// from raw ptr
		{
			auto rc_unique = RCUnique<TestRCBase>{new TestRCBase};
			RCWeak<TestRCBase> rc_weak{rc_unique.get()};
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK(rc_weak.lock());
			CHECK_EQ(rc_weak.weak_ref_count(), 1);
			CHECK_EQ(rc_unique.weak_ref_count(), 1);
			if (auto locked = rc_weak.lock()) {
				CHECK_EQ(rc_base_count, 1);
				CHECK_EQ(rc_derived_count, 0);
				CHECK_EQ(locked.get(), rc_unique.get());
			} else {
				abort();
			}
		}

		// from raw ptr derived
		{
			auto rc_unique = RCUnique<TestRCDerived>{new TestRCDerived};
			RCWeak<TestRCBase> rc_weak{rc_unique.get()};
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 1);
			CHECK(rc_weak.lock());
			CHECK_EQ(rc_weak.weak_ref_count(), 1);
			CHECK_EQ(rc_unique.weak_ref_count(), 1);
			if (auto locked = rc_weak.lock()) {
				CHECK_EQ(rc_base_count, 1);
				CHECK_EQ(rc_derived_count, 1);
				CHECK_EQ(locked.get(), rc_unique.get());
			} else {
				abort();
			}
		}

		// from rc
		{
			auto rc = RC<TestRCBase>{new TestRCBase};
			auto rc_weak = RCWeak<TestRCBase>(rc);
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK(rc_weak.lock());
			CHECK_EQ(rc_weak.weak_ref_count(), 1);
			CHECK_EQ(rc.weak_ref_count(), 1);
			CHECK_EQ(rc.ref_count(), 1);
			if (auto locked = rc_weak.lock()) {
				CHECK_EQ(rc_base_count, 1);
				CHECK_EQ(rc_derived_count, 0);
				CHECK_EQ(locked.get(), rc.get());
			} else {
				abort();
			}
		}

		// from rc derived
		{
			auto rc = RC<TestRCDerived>{new TestRCDerived};
			auto rc_weak = RCWeak<TestRCBase>(rc);
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 1);
			CHECK(rc_weak.lock());
			CHECK_EQ(rc_weak.weak_ref_count(), 1);
			CHECK_EQ(rc.weak_ref_count(), 1);
			CHECK_EQ(rc.ref_count(), 1);
			if (auto locked = rc_weak.lock()) {
				CHECK_EQ(rc_base_count, 1);
				CHECK_EQ(rc_derived_count, 1);
				CHECK_EQ(locked.get(), rc.get());
			} else {
				abort();
			}
		}

		// from unique
		{
			auto rc_unique = RCUnique<TestRCBase>{new TestRCBase};
			auto rc_weak = RCWeak<TestRCBase>(rc_unique);
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK(rc_weak.lock());
			CHECK_EQ(rc_weak.weak_ref_count(), 1);
			CHECK_EQ(rc_unique.weak_ref_count(), 1);
			if (auto locked = rc_weak.lock()) {
				CHECK_EQ(rc_base_count, 1);
				CHECK_EQ(rc_derived_count, 0);
				CHECK_EQ(locked.get(), rc_unique.get());
			} else {
				abort();
			}
		}

		// from unique derived
		{
			auto rc_unique = RCUnique<TestRCDerived>{new TestRCDerived};
			auto rc_weak = RCWeak<TestRCBase>(rc_unique);
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 1);
			CHECK(rc_weak.lock());
			CHECK_EQ(rc_weak.weak_ref_count(), 1);
			CHECK_EQ(rc_unique.weak_ref_count(), 1);
			if (auto locked = rc_weak.lock()) {
				CHECK_EQ(rc_base_count, 1);
				CHECK_EQ(rc_derived_count, 1);
				CHECK_EQ(locked.get(), rc_unique.get());
			} else {
				abort();
			}
		}

		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
	}

	SUBCASE("copy & move") {
		auto base = RCUnique<TestRCBase>{new TestRCBase};
		auto derived = RCUnique<TestRCDerived>{new TestRCDerived};

		RCWeak<TestRCBase> base_weak{base};
		RCWeak<TestRCDerived> derived_weak{derived};

		RCWeak<TestRCBase> copy_base{base_weak};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base_weak.weak_ref_count(), 2);
		CHECK_EQ(copy_base.weak_ref_count(), 2);
		CHECK_EQ(copy_base.lock().get(), base_weak.lock().get());

		RCWeak<TestRCBase> copy_derived{derived_weak};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(derived_weak.weak_ref_count(), 2);
		CHECK_EQ(copy_derived.weak_ref_count(), 2);
		CHECK_EQ(copy_derived.lock().get(), derived_weak.lock().get());

		RCWeak<TestRCBase> move_base{std::move(base_weak)};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.weak_ref_count(), 2);
		CHECK_EQ(move_base.weak_ref_count(), 2);
		CHECK_EQ(move_base.lock().get(), base.get());

		RCWeak<TestRCBase> move_derived{std::move(derived_weak)};
		CHECK_EQ(rc_base_count, 2);
		CHECK_EQ(rc_derived_count, 1);
		CHECK_EQ(base.weak_ref_count(), 2);
		CHECK_EQ(move_derived.weak_ref_count(), 2);
		CHECK_EQ(move_derived.lock().get(), derived.get());

		base = nullptr;
		derived = nullptr;
		base_weak = nullptr;
		derived_weak = nullptr;
		copy_base = nullptr;
		copy_derived = nullptr;
		move_base = nullptr;
		move_derived = nullptr;

		CHECK_EQ(rc_base_count, 0);
		CHECK_EQ(rc_derived_count, 0);
	}

	SUBCASE("assign & move assign") {
		auto base = RCUnique<TestRCBase>{new TestRCBase};
		auto derived = RC<TestRCDerived>{new TestRCDerived};

		RCWeak<TestRCBase> base_weak{base};
		RCWeak<TestRCDerived> derived_weak{derived};

		// assign weak
		{
			RCWeak<TestRCBase> assign{};

			// assign
			assign = base_weak;
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(base_weak.weak_ref_count(), 2);
			CHECK_EQ(assign.weak_ref_count(), 2);
			CHECK_EQ(assign.lock().get(), base_weak.lock().get());

			// assign derived
			assign = derived_weak;
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(derived_weak.weak_ref_count(), 2);
			CHECK_EQ(assign.weak_ref_count(), 2);
			CHECK_EQ(assign.lock().get(), derived_weak.lock().get());

			CHECK_EQ(base_weak.weak_ref_count(), 1);
		}

		// move assign weak
		{
			RCWeak<TestRCBase> move_assign{};

			// move assign base
			auto cached_base = base_weak.lock().get();
			move_assign = std::move(base_weak);
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK(base_weak.is_empty());
			CHECK_EQ(move_assign.weak_ref_count(), 1);
			CHECK_EQ(move_assign.lock().get(), cached_base);

			// move assign derived
			auto cached_derived = derived_weak.lock().get();
			move_assign = std::move(derived_weak);
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK(derived_weak.is_empty());
			CHECK_EQ(move_assign.weak_ref_count(), 1);
			CHECK_EQ(move_assign.lock().get(), cached_derived);

			CHECK_EQ(base_weak.weak_ref_count(), 0);
		}

		// assign rc & unique rc
		{
			RCWeak<TestRCBase> assign{};
			assign = base;
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(assign.lock().get(), base.get());

			// assign derived
			assign = derived;
			CHECK_EQ(rc_base_count, 2);
			CHECK_EQ(rc_derived_count, 1);
			CHECK_EQ(assign.lock().get(), derived.get());
		}
	}

	SUBCASE("test compare") {
		RC<TestRCBase> rc_base1{new TestRCBase};
		RC<TestRCBase> rc_base2{new TestRCBase};
		RC<TestRCDerived> rc_derived1{new TestRCDerived};
		RC<TestRCDerived> rc_derived2{new TestRCDerived};

		RCWeak<TestRCBase> base1{rc_base1};
		RCWeak<TestRCBase> base2{rc_base2};
		RCWeak<TestRCDerived> derived1{rc_derived1};
		RCWeak<TestRCDerived> derived2{rc_derived2};

		RCWeak<TestRCBase> copy_base1 = base1;
		RCWeak<TestRCBase> copy_derived1{derived1};

		// eq
		CHECK_EQ(base1 == base1, true);
		CHECK_EQ(base1 == base2, false);
		CHECK_EQ(base1 == derived1, false);
		CHECK_EQ(base1 == copy_base1, true);
		CHECK_EQ(derived1 == copy_derived1, true);
		CHECK_EQ(derived2 == copy_derived1, false);

		// neq
		CHECK_EQ(base1 != base1, false);
		CHECK_EQ(base1 != base2, true);
		CHECK_EQ(base1 != derived1, true);
		CHECK_EQ(base1 != copy_base1, false);
		CHECK_EQ(derived1 != copy_derived1, false);
		CHECK_EQ(derived2 != copy_derived1, true);

		// nullptr
		CHECK_EQ(base1 == nullptr, false);
		CHECK_EQ(nullptr == base1, false);
		CHECK_EQ(base1 != nullptr, true);
		CHECK_EQ(nullptr != base1, true);

		// lt (just test for compile)
		CHECK_EQ(base1 < base1, false);
		CHECK_EQ(derived1 < copy_derived1, false);
		CHECK_EQ(copy_derived1 < derived1, false);

		// gt (just test for compile)
		CHECK_EQ(base1 > base1, false);
		CHECK_EQ(derived1 > copy_derived1, false);
		CHECK_EQ(copy_derived1 > derived1, false);

		// le (just test for compile)
		CHECK_EQ(base1 <= base1, true);
		CHECK_EQ(derived1 <= copy_derived1, true);
		CHECK_EQ(copy_derived1 <= derived1, true);

		// ge (just test for compile)
		CHECK_EQ(base1 >= base1, true);
		CHECK_EQ(derived1 >= copy_derived1, true);
		CHECK_EQ(copy_derived1 >= derived1, true);
	}

	SUBCASE("lock") {
		RC<TestRCBase> rc{new TestRCBase};

		// basic lock
		RCWeak<TestRCBase> weak{rc};
		CHECK_FALSE(weak.is_expired());
		CHECK(weak.lock());
		//
		{
			auto locked = weak.lock();
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK_EQ(locked.get(), rc.get());
		}

		// lock a destroyed weak
		rc.reset();
		CHECK(weak.is_expired());
		CHECK_FALSE(weak.lock());
		//
		{
			auto locked = weak.lock();
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK_EQ(locked.get(), nullptr);
		}

		// lock and cast to rc
		rc = new TestRCBase;
		weak = rc;
		CHECK_FALSE(weak.is_expired());
		CHECK(weak.lock());
		//
		{
			RC<TestRCBase> locked_rc = weak.lock();
			CHECK_EQ(rc_base_count, 1);
			CHECK_EQ(rc_derived_count, 0);
			CHECK_EQ(locked_rc.get(), rc.get());
			CHECK_EQ(locked_rc.ref_count(), 2);
		}
	}

	SUBCASE("test ops") {
		RC<TestRCBase> rc{new TestRCBase};
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_NE(rc.get(), nullptr);

		RCWeak<TestRCBase> weak{rc};
		CHECK_EQ(rc_base_count, 1);
		CHECK_EQ(rc_derived_count, 0);
		CHECK_FALSE(weak.is_empty());
		CHECK_EQ(weak.get_unsafe(), rc.get());

		weak.reset();
		CHECK(weak.is_empty());
		CHECK_EQ(weak.weak_ref_count(), 0);
		CHECK_EQ(weak.get_unsafe(), nullptr);

		weak.reset(rc);
		CHECK_FALSE(weak.is_empty());
		CHECK_EQ(weak.get_unsafe(), rc.get());

		RCUnique<TestRCBase> new_rc{new TestRCBase};
		weak.reset(new_rc);
		CHECK_FALSE(weak.is_empty());
		CHECK_EQ(weak.get_unsafe(), new_rc.get());

		weak.reset(rc.get());
		CHECK_FALSE(weak.is_empty());
		CHECK_EQ(weak.get_unsafe(), rc.get());

		RCWeak<TestRCBase> new_weak{new_rc};
		new_weak.swap(weak);
		CHECK_EQ(new_rc.get(), weak.get_unsafe());
		CHECK_EQ(rc.get(), new_weak.get_unsafe());
	}

	SUBCASE("test empty") {
		RCWeak<TestRCBase> empty{};
		RCWeak<TestRCBase> empty_copy{empty};
		RCWeak<TestRCBase> empty_move{std::move(empty)};
		RCWeak<TestRCBase> empty_assign, empty_move_assign;
		empty_assign = empty;
		empty_move_assign = std::move(empty);

		empty.weak_ref_count();
		empty.is_empty();
		empty.is_expired();
		empty.operator bool();
		empty.reset();
		empty.reset(nullptr);
		empty.swap(empty_copy);
		empty.lock();
	}
}

TEST_CASE("Test Custom Deleter") {
	using namespace hana;

	RC<TestCustomDeleterRC> rc{new TestCustomDeleterRC};
	rc.reset();
	CHECK_EQ(rc_custom_deleter_count, 1);

	RCUnique<TestCustomDeleterRC> rc_unique{new TestCustomDeleterRC};
	rc_unique.reset();
	CHECK_EQ(rc_custom_deleter_count, 2);
	//
	{
		RC<TestCustomDeleterRC> rc{new TestCustomDeleterRC};
	}
	CHECK_EQ(rc_custom_deleter_count, 3);
	//
	{
		RCUnique<TestCustomDeleterRC> rc_unique{new TestCustomDeleterRC};
	}
	CHECK_EQ(rc_custom_deleter_count, 4);
}
