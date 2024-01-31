#ifndef VFS_API_DETAIL_WRAPPED_POINTER_HH
#define VFS_API_DETAIL_WRAPPED_POINTER_HH

namespace vfs::core {
	template <typename T>
	class wrapped_pointer {
	 public:
		wrapped_pointer ();
		explicit wrapped_pointer (T* ptr);

		wrapped_pointer (const wrapped_pointer&) = default;
		wrapped_pointer& operator= (const wrapped_pointer&) = default;

		explicit operator bool () const noexcept;

		T* operator-> ();
		const T* operator-> () const;

		T& operator* ();
		const T& operator* () const;

		T* get ();
		const T* get () const;
	 private:
		T* _ptr;
	};

	template <typename T>
	wrapped_pointer<T>::wrapped_pointer ()
		: _ptr (nullptr) {

	}

	// ----------------------------------------------------------------------
	template <typename T>
	wrapped_pointer<T>::wrapped_pointer (T* ptr)
		: _ptr (ptr) {

	}

	// ----------------------------------------------------------------------
	template <typename T>
	wrapped_pointer<T>::operator bool () const noexcept {
		return _ptr != nullptr;
	}

	// ----------------------------------------------------------------------
	template <typename T>
	T* wrapped_pointer<T>::operator-> () {
		return _ptr;
	}

	// ----------------------------------------------------------------------
	template <typename T>
	const T* wrapped_pointer<T>::operator-> () const {
		return _ptr;
	}

	// ----------------------------------------------------------------------
	template <typename T>
	T& wrapped_pointer<T>::operator* () {
		return *_ptr;
	}

	// ----------------------------------------------------------------------
	template <typename T>
	const T& wrapped_pointer<T>::operator* () const {
		return _ptr;
	}

	// ----------------------------------------------------------------------
	template <typename T>
	T* wrapped_pointer<T>::get () {
		return _ptr;
	}

	// ----------------------------------------------------------------------
	template <typename T>
	const T* wrapped_pointer<T>::get () const {
		return _ptr;
	}
}

#endif
