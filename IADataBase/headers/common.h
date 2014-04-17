#if !defined(__COMMON_UTILS_H)
#define __COMMON_UTILS_H	1

// Helper functor to delete pointers in the containers
struct delete_ptr
{
	template<typename T>
	void operator()(const T* p) const
	{
		delete p;
	}
};

template <typename T>
class auto_set
{
	T& m_val;
	T m_destroy;
public:
	auto_set(T& val, T init, T destroy) : m_val(val), m_destroy(destroy)
	{
		m_val = init;
	}
	~auto_set()
	{
		m_val = m_destroy;
	}
};

template <typename T>
struct equal_ptr_obj
{
	const T* m_ref;
	equal_ptr_obj(const T* ref) : m_ref(ref) {}
	equal_ptr_obj(const equal_ptr_obj& src) : m_ref(src.m_ref) {}
	bool operator()(const T* val) {return *m_ref == *val;}
};

#endif //__COMMON_UTILS_H