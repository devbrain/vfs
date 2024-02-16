#ifndef VFS_DETAIL_WRAPPED_ITERATOR_HH
#define VFS_DETAIL_WRAPPED_ITERATOR_HH

#include <iterator>

namespace vfs::detail
{
	template <typename Data, typename Wrapper>
	class iterator
	{
	public:
		typedef Data value_type;
		typedef std::ptrdiff_t difference_type;
		typedef Data* pointer;
		typedef Data& reference;
		typedef std::input_iterator_tag iterator_category;
	public:
		~iterator() = default;

		[[nodiscard]] Data operator*() const;
		[[nodiscard]] Data operator -> () const;
		bool operator==(const iterator& other) const noexcept ;
		bool operator!=(const iterator& other) const noexcept ;

		Data operator++(int);
		iterator& operator++();
	protected:
		Wrapper& _value;
		explicit iterator(Wrapper& itr);

		virtual bool _equals(const Wrapper& a, const Wrapper& b) const noexcept = 0;
		virtual void _inc(Wrapper& a) = 0;
	};
	// =====================================================================
	// Implementation
	// =====================================================================
	template <typename Data, typename Wrapper>
	Data iterator<Data, Wrapper>::operator*() const
	{
		return Data(_value);
	}
	template <typename Data, typename Wrapper>
	Data iterator<Data, Wrapper>::operator -> () const
	{
		return Data(_value);
	}
	// ---------------------------------------------------------------------
	template <typename Data, typename Wrapper>
	bool iterator<Data, Wrapper>::operator==(const iterator& other) const noexcept
	{
		return _equals(_value, other._value);
	}
	// ---------------------------------------------------------------------
	template <typename Data, typename Wrapper>
	bool iterator<Data, Wrapper>::operator!=(const iterator& other) const noexcept
	{
		return !_equals(_value, other._value);
	}
	// ---------------------------------------------------------------------
	template <typename Data, typename Wrapper>
	Data iterator<Data, Wrapper>::operator++(int)
	{
		Data ret(_value);
		(void) ++*this;
		return ret;
	}
	// ---------------------------------------------------------------------
	template <typename Data, typename Wrapper>
	iterator<Data, Wrapper>& iterator<Data, Wrapper>::operator++()
	{
		_inc (_value);
		return *this;
	}
	// ---------------------------------------------------------------------
	template <typename Data, typename Wrapper>
	iterator<Data, Wrapper>::iterator(Wrapper& itr)
		: _value(itr)
	{
	}
} // ns vfs::detail


#endif
