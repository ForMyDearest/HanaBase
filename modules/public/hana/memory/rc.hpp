#pragma once

#include <concepts>

namespace hana
{
	class RCInterface {
		uint32_t rc_count() const noexcept {
			return counter.load(std::memory_order_relaxed);
		}

		uint32_t rc_weak_count() const noexcept {
			return weak_counter.load(std::memory_order_relaxed);
		}

		void rc_set_unique() noexcept {
			counter.store(1, std::memory_order_relaxed);
		}

		void rc_add_ref() noexcept {
			counter.fetch_add(1, std::memory_order_relaxed);
		}

		void rc_weak_add_ref() noexcept {
			weak_counter.fetch_add(1, std::memory_order_relaxed);
		}

		bool rc_release() noexcept {
			return counter.fetch_sub(1, std::memory_order_release) == 1 && rc_weak_count() == 0;
		}

		bool rc_weak_release() noexcept {
			return weak_counter.fetch_sub(1, std::memory_order_release) == 1 && rc_count() == 0;
		}

		std::atomic<uint32_t> counter = 0;
		std::atomic<uint32_t> weak_counter = 0;

		template<typename T> requires(std::is_base_of_v<RCInterface, T>)
		friend class RC;
		template<typename T> requires(std::is_base_of_v<RCInterface, T>)
		friend class RCWeak;
		template<typename T> requires(std::is_base_of_v<RCInterface, T>)
		friend class RCUnique;
	};

	template<typename From, typename To>
	concept RCConvertible = requires
	{
		std::convertible_to<From*, To*>;
		std::is_same_v<From, To> || std::has_virtual_destructor_v<To>;
	};

	template<typename T> requires(std::is_base_of_v<RCInterface, T>)
	class RCUnique {
	public:
		RCUnique() noexcept = default;
		explicit RCUnique(std::nullptr_t) noexcept {}
		RCUnique(RCUnique&& rhs) noexcept: RCUnique(rhs.release()) {}
		template<RCConvertible<T> U> explicit RCUnique(RCUnique<U>&& rhs) noexcept : RCUnique(rhs.release()) {}
		template<RCConvertible<T> U> explicit RCUnique(U* ptr) noexcept: ptr_(static_cast<T*>(ptr)) { if (ptr_) ptr_->rc_set_unique(); }
		~RCUnique() noexcept { reset(); }

		RCUnique& operator=(std::nullptr_t) noexcept { return reset(), *this; }
		RCUnique& operator=(RCUnique&& rhs) noexcept { return this->reset(rhs.release()), *this; }
		template<RCConvertible<T> U> RCUnique& operator=(U* ptr) noexcept { return this->reset(ptr), *this; }
		template<RCConvertible<T> U> RCUnique& operator=(RCUnique<U>&& rhs) noexcept { return this->reset(rhs.release()), *this; }

		RCUnique(const RCUnique& rhs) noexcept = delete;
		RCUnique& operator=(const RCUnique& rhs) noexcept = delete;

		void swap(RCUnique& rhs) noexcept { std::swap(ptr_, rhs.ptr_); }

		T* get() const noexcept { return ptr_; }
		T& operator*() const noexcept { return *ptr_; }
		T* operator->() const noexcept { return ptr_; }
		explicit operator bool() const noexcept { return ptr_; }
		uint32_t weak_ref_count() const noexcept { return ptr_ ? ptr_->rc_weak_count() : 0; }

		bool operator==(std::nullptr_t) const noexcept { return ptr_ == nullptr; }
		template<typename U> bool operator==(U* rhs) const noexcept { return ptr_ == rhs; }
		template<typename U> bool operator==(const RCUnique<U>& rhs) const noexcept { return ptr_ == rhs.get(); }
		template<typename U> std::strong_ordering operator<=>(U* rhs) const noexcept { return ptr_ <=> rhs; }
		template<typename U> std::strong_ordering operator<=>(const RCUnique<U>& rhs) const noexcept { return ptr_ <=> rhs.get(); }

		friend bool operator==(std::nullptr_t, const RCUnique& rhs) noexcept { return nullptr == rhs.ptr_; }
		template<typename U> friend bool operator==(U* lhs, const RCUnique& rhs) noexcept { return lhs == rhs.ptr_; }
		template<typename U> friend std::strong_ordering operator<=>(U* lhs, const RCUnique& rhs) noexcept { return lhs <=> rhs.ptr_; }

		void reset(T* ptr = nullptr) noexcept {
			if (T* old = std::exchange(ptr_, ptr)) {
				if (old->rc_release()) {
					if constexpr (requires(T* obj) { { obj->rc_delete() } -> std::same_as<void>; }) {
						old->rc_delete();
					} else {
						delete old;
					}
				}
			}
			if (ptr_) ptr_->rc_set_unique();
		}

		template<RCConvertible<T> U>
		void reset(U* ptr) noexcept {
			this->reset(static_cast<T*>(ptr));
		}

		T* release() noexcept {
			if (ptr_) ptr_->rc_release();
			return std::exchange(ptr_, nullptr);
		}

	private:
		T* ptr_ = nullptr;
	};

	template<typename T> requires(std::is_base_of_v<RCInterface, T>)
	class RC {
	public:
		RC() noexcept = default;
		explicit RC(std::nullptr_t) noexcept {}
		RC(const RC& rhs) noexcept: RC(rhs.ptr_) {}
		RC(RC&& rhs) noexcept: ptr_(rhs.ptr_) { rhs.ptr_ = nullptr; }
		template<RCConvertible<T> U> explicit RC(const RC<U>& rhs) noexcept: RC(rhs.get()) {}
		template<RCConvertible<T> U> explicit RC(RC<U>&& rhs) noexcept: RC(rhs.get()) { rhs.reset(); }
		template<RCConvertible<T> U> explicit RC(RCUnique<U>&& rhs) noexcept: RC(rhs.release()) {}
		template<RCConvertible<T> U> explicit RC(U* ptr) noexcept: ptr_(static_cast<T*>(ptr)) { if (ptr_) ptr_->rc_add_ref(); }
		~RC() noexcept { reset(); }

		RC& operator=(std::nullptr_t) noexcept { return this->reset(), *this; }
		RC& operator=(const RC& rhs) noexcept { return this->operator=(rhs.ptr_); }
		RC& operator=(RC&& rhs) noexcept { return this->reset(rhs.ptr_), rhs.reset(), *this; }
		template<RCConvertible<T> U> RC& operator=(U* ptr) noexcept { return this->reset(ptr), *this; }
		template<RCConvertible<T> U> RC& operator=(const RC<U>& rhs) noexcept { return this->operator=(rhs.get()); }
		template<RCConvertible<T> U> RC& operator=(RC<U>&& rhs) noexcept { return this->reset(rhs.get()), rhs.reset(), *this; }
		template<RCConvertible<T> U> RC& operator=(RCUnique<U>&& rhs) noexcept { return this->operator=(rhs.release()); }

		T* get() const noexcept { return ptr_; }
		T& operator*() const noexcept { return *ptr_; }
		T* operator->() const noexcept { return ptr_; }
		explicit operator bool() const noexcept { return ptr_; }
		void swap(RC& rhs) noexcept { std::swap(rhs.ptr_, ptr_); }
		template<RCConvertible<T> U> RC<U> cast_static() const noexcept { return RC<U>(ptr_); }
		uint32_t ref_count() const noexcept { return ptr_ ? ptr_->rc_count() : 0; }
		uint32_t weak_ref_count() const noexcept { return ptr_ ? ptr_->rc_weak_count() : 0; }

		bool operator==(std::nullptr_t) const noexcept { return ptr_ == nullptr; }
		template<typename U> bool operator==(U* rhs) const noexcept { return ptr_ == rhs; }
		template<typename U> bool operator==(const RC<U>& rhs) const noexcept { return ptr_ == rhs.get(); }
		template<typename U> std::strong_ordering operator<=>(U* rhs) const noexcept { return ptr_ <=> rhs; }
		template<typename U> std::strong_ordering operator<=>(const RC<U>& rhs) const noexcept { return ptr_ <=> rhs.get(); }

		friend bool operator==(std::nullptr_t, const RC& rhs) noexcept { return nullptr == rhs.ptr_; }
		template<typename U> friend bool operator==(U* lhs, const RC& rhs) noexcept { return lhs == rhs.ptr_; }
		template<typename U> friend std::strong_ordering operator<=>(U* lhs, const RC& rhs) noexcept { return lhs <=> rhs.ptr_; }

		void reset(T* ptr = nullptr) noexcept {
			if (T* old = std::exchange(ptr_, ptr)) {
				if (old->rc_release()) {
					if constexpr (requires(T* obj) { { obj->rc_delete() } -> std::same_as<void>; }) {
						old->rc_delete();
					} else {
						delete old;
					}
				}
			}
			if (ptr_) ptr_->rc_add_ref();
		}

		template<RCConvertible<T> U>
		void reset(U* ptr) noexcept {
			this->reset(static_cast<T*>(ptr));
		}

	private:
		T* ptr_ = nullptr;
	};

	template<typename T> requires(std::is_base_of_v<RCInterface, T>)
	class RCWeak {
		template<typename U> requires(std::is_base_of_v<RCInterface, U>)
		friend class RCWeak;

	public:
		RCWeak() noexcept = default;
		explicit RCWeak(std::nullptr_t) noexcept {}
		RCWeak(const RCWeak& rhs) noexcept: RCWeak(rhs.ptr_) {}
		RCWeak(RCWeak&& rhs) noexcept: ptr_(rhs.ptr_) { rhs.ptr_ = nullptr; }
		template<typename U> explicit RCWeak(U* ptr) noexcept: ptr_(static_cast<T*>(ptr)) { if (ptr_) ptr_->rc_weak_add_ref(); }
		template<typename U> explicit RCWeak(RCWeak<U>&& rhs) noexcept: ptr_(static_cast<T*>(rhs.ptr_)) { rhs.ptr_ = nullptr; }
		template<typename U> explicit RCWeak(const RCWeak<U>& rhs) noexcept: RCWeak(rhs.get_unsafe()) {}
		template<typename U> explicit RCWeak(const RC<U>& rhs) noexcept: RCWeak(rhs.get()) {}
		template<typename U> explicit RCWeak(const RCUnique<U>& rhs) noexcept: RCWeak(rhs.get()) {}
		~RCWeak() noexcept { reset(); }

		RCWeak& operator=(std::nullptr_t) noexcept { return reset(), *this; }
		RCWeak& operator=(const RCWeak& rhs) noexcept { return this->operator=(rhs.ptr_); }
		RCWeak& operator=(RCWeak&& rhs) noexcept { return this->reset(rhs.get_unsafe()), rhs.reset(), *this; }
		template<typename U> RCWeak& operator=(U* ptr) noexcept { return this->reset(ptr), *this; }
		template<typename U> RCWeak& operator=(RCWeak<U>&& rhs) noexcept { return this->reset(rhs.get_unsafe()), rhs.reset(), *this; }
		template<typename U> RCWeak& operator=(const RCWeak<U>& rhs) noexcept { return this->operator=(rhs.get_unsafe()); }
		template<typename U> RCWeak& operator=(const RC<U>& rhs) noexcept { return this->operator=(rhs.get()); }
		template<typename U> RCWeak& operator=(const RCUnique<U>& rhs) noexcept { return this->operator=(rhs.get()); }

		explicit operator bool() const noexcept { return !is_expired(); }
		bool is_expired() const noexcept { return !ptr_ || !ptr_->rc_count(); }
		void swap(RCWeak& rhs) noexcept { std::swap(ptr_, rhs.ptr_); }
		uint32_t weak_ref_count() const noexcept { return ptr_ ? ptr_->rc_weak_count() : 0; }
		template<typename U> RCWeak<U> cast_static() const noexcept { return ptr_ && ptr_->rc_count() ? RCWeak<U>{ptr_} : RCWeak<U>{}; }

		bool operator==(std::nullptr_t) const noexcept { return ptr_ == nullptr; }
		friend bool operator==(std::nullptr_t, const RCWeak& rhs) noexcept { return nullptr == rhs.get_unsafe(); }
		template<typename U> bool operator==(const RCWeak<U>& rhs) const noexcept { return ptr_ == rhs.get_unsafe(); }
		template<typename U> std::strong_ordering operator<=>(const RCWeak<U>& rhs) const noexcept { return ptr_ <=> rhs.get_unsafe(); }

		T* get_unsafe() const noexcept { return ptr_; }
		bool is_empty() const noexcept { return !ptr_; }

		RC<T> lock() const noexcept {
			return ptr_ && ptr_->rc_count() ? RC<T>{ptr_} : RC<T>{};
		}

		void reset(T* ptr = nullptr) noexcept {
			if (T* old = std::exchange(ptr_, ptr)) {
				if (old->rc_weak_release()) {
					if constexpr (requires(T* obj) { { obj->rc_delete() } -> std::same_as<void>; }) {
						old->rc_delete();
					} else {
						delete old;
					}
				}
			}
			if (ptr_) ptr_->rc_weak_add_ref();
		}

		template<typename U>
		void reset(U* ptr) noexcept {
			this->reset(static_cast<T*>(ptr));
		}

		template<typename U>
		void reset(const RC<U>& ptr) noexcept {
			this->reset(ptr.get());
		}

		template<typename U>
		void reset(const RCUnique<U>& ptr) noexcept {
			this->reset(ptr.get());
		}

	private:
		T* ptr_ = nullptr;
	};
}
