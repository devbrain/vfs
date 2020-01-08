#ifndef VFS_API_REFCOUNT_HH
#define VFS_API_REFCOUNT_HH
#include <type_traits>

namespace vfs::detail
{
	class referenced
	{
	public:
		referenced();
		void inc_ref() const noexcept;
		int dec_ref() const noexcept;
	private:
		mutable int _ref;
	};

	template<class T>
	class refcounted_ptr
	{
		static_assert(std::is_base_of_v<referenced, T>, "Type should be derived from referenced");
	public:
		class null_dereference
		{
		};

		refcounted_ptr ();

		explicit refcounted_ptr(T* obj) noexcept;

		refcounted_ptr(refcounted_ptr<T>&& orig) noexcept ;
		refcounted_ptr<T>& operator=(refcounted_ptr<T>&& orig) noexcept ;

		refcounted_ptr(const refcounted_ptr<T>& orig) noexcept;
		refcounted_ptr<T>& operator=(const refcounted_ptr<T>& right);

		~refcounted_ptr<T>() noexcept;

		T& operator*() noexcept(false);
		T* operator->() noexcept(false);

		bool operator==(const refcounted_ptr<T>& right) const;
		explicit operator bool() const noexcept;

	private:
		T* _obj;
		void _dec() noexcept;

	};

	// ========================================================================
	// Implementation
	// ========================================================================
	inline
	referenced::referenced()
		: _ref(1)
	{

	}
	// -------------------------------------------------------------------------
	inline
	void referenced::inc_ref() const noexcept
	{
		_ref++;
	}
	// -------------------------------------------------------------------------
	inline
	int referenced::dec_ref() const noexcept
	{
		return --_ref;
	}
	// =========================================================================
	template<typename T>
	refcounted_ptr<T>::refcounted_ptr ()
	: _obj (nullptr)
	{

	}
	// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>::refcounted_ptr(T* obj) noexcept
		: _obj(_obj)
	{
	}
	// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>::refcounted_ptr(refcounted_ptr<T>&& orig) noexcept
	{
		_obj = orig._obj;
		orig._obj = nullptr;
	}
	// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>& refcounted_ptr<T>::operator=(refcounted_ptr<T>&& orig) noexcept
	{
		_obj = orig._obj;
		orig._obj = nullptr;
		return *this;
	}

// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>::refcounted_ptr(const refcounted_ptr<T>& orig) noexcept
	{
		_obj = orig._obj;
		if (_obj)
		{
			_obj->inc_ref();
		}
	}
// -------------------------------------------------------------------------
	template<typename T>
	T& refcounted_ptr<T>::operator*() noexcept(false)
	{
		if (!_obj)
		{
			throw null_dereference();
		}
		return *_obj;
	}
// -------------------------------------------------------------------------
	template<typename T>
	T* refcounted_ptr<T>::operator->() noexcept(false)
	{
		if (!_obj)
		{
			throw null_dereference();
		}
		return _obj;
	}
// -------------------------------------------------------------------------
	template<typename T>
	bool refcounted_ptr<T>::operator==(const refcounted_ptr<T>& right) const
	{
		return _obj == right._obj;
	}
// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>& refcounted_ptr<T>::operator=(const refcounted_ptr<T>& right)
	{
		if (this == &right)
			return *this;

		if (right._obj)
			right._obj->inc_ref();

		_dec();

		_obj = right._obj;

		return *this;
	}
// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>::operator bool() const noexcept
	{
		return _obj != nullptr;
	}
	// -------------------------------------------------------------------------
	template<typename T>
	refcounted_ptr<T>::~refcounted_ptr<T>() noexcept
	{
		_dec();
	}
	// -------------------------------------------------------------------------
	template<typename T>
	void refcounted_ptr<T>::_dec() noexcept
	{
		if (_obj && _obj->dec_ref == 0)
		{
			delete _obj;
			_obj = nullptr;
		}
	}
}


#endif
