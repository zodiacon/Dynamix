#pragma once

#include <type_traits>

namespace Dynamix {
	template<typename T = RuntimeObject>
	class ObjectPtr {
		//static_assert(std::is_base_of_v<RuntimeObject, T>, "T must derive from RuntimeObject");
	public:
		ObjectPtr(T* obj = nullptr) : p(obj) {
			if (p)
				p->AddRef();
		}

		ObjectPtr(ObjectPtr const& other) : p(other.p) {
			if(p)
				p->AddRef();
		}

		ObjectPtr& operator=(ObjectPtr const& other) {
			if (this != &other) {
				Release();
				p = other.p;
				if (p)
					p->AddRef();
			}
			return *this;
		}

		ObjectPtr(ObjectPtr&& other) noexcept : p(other.p) {
			other.p = nullptr;
		}

		ObjectPtr& operator=(ObjectPtr&& other) noexcept {
			if (this != &other) {
				Release();
				p = other.p;
				other.p = nullptr;
			}
			return *this;
		}

		~ObjectPtr() {
			Release();
		}

		void Release() {
			if (p) {
				p->Release();
				p = nullptr;
			}
		}

		operator T* () {
			return p;
		}

		T* operator->() const {
			return p;
		}

		T* Get() {
			return p;
		}

		T const* Get() const {
			return p;
		}

	private:
		T* p;
	};
}
