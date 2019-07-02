#pragma once
#pragma pack(push,4)
//////////////////////////////////////////////////////////////////////////
using str_c = const char*;

//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable : 4200)
struct XRCORE_API str_value
{
	u32			dwReference;
	u32			dwLength;
	u32			dwCRC;
	str_value*  next;
	char		value[];
};

struct XRCORE_API str_value_cmp	// less
{ 
	inline bool operator() (const str_value* A, const str_value* B) const { return A->dwCRC<B->dwCRC; };
};
struct XRCORE_API str_hash_function 
{
	inline u32 operator() (str_value const* const value) const { return value->dwCRC;	};
};
#pragma warning(pop)

struct str_container_impl;
class IWriter;
//////////////////////////////////////////////////////////////////////////
class XRCORE_API str_container
{
private:
    xrCriticalSection	cs;
	str_container_impl* impl;
public:
						str_container	();
						~str_container  ();

	str_value*			dock			(str_c value);
	void				clean			();
	void				dump			();
	void				dump			(IWriter* W);
	void				verify			();
	u32					stat_economy	();
};
XRCORE_API extern str_container g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class shared_str
{
	str_value*			p_;
protected:
	// ref-counting
	void				_dec		()								{	if (p_== nullptr) return;	p_->dwReference--; 	if (0==p_->dwReference)	p_ = nullptr;		}
public:
	void				_set		(str_c rhs) 					{	str_value* v = g_pStringContainer.dock(rhs); if (nullptr!=v) v->dwReference++; _dec(); p_ = v;	}
	void				_set		(shared_str const &rhs)			{	str_value* v = rhs.p_; if (nullptr!=v) v->dwReference++; _dec(); p_ = v;							}

	const str_value*	_get		()	const						{	return p_;	}
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// construction
			constexpr	shared_str	() : p_(nullptr)						{ }
						shared_str	(str_c rhs) : p_(nullptr)				{	_set(rhs); }
						shared_str	(shared_str const &rhs) : p_(nullptr)	{	_set(rhs); }
						~shared_str	()										{	_dec();	   }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// assignment & accessors
	shared_str&			operator=	(str_c rhs)						{	_set(rhs);	return (shared_str&)*this;		}
	shared_str&			operator=	(shared_str const &rhs)			{	_set(rhs);	return (shared_str&)*this;		}
	str_c				operator*	() const						{	return p_ ? p_->value : nullptr;			}
	bool				operator!	() const						{	return p_ == nullptr;						}
	char				operator[]	(size_t id)						{	return p_->value[id];						}
	bool				operator==	(shared_str const &rhs)			{	return _get() == rhs._get();				}

	str_c				c_str		() const						{	return p_ ? p_->value : nullptr;			}
	char*				data		() const						{	return p_ ? p_->value : nullptr;			}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// misc func
	u32					size		()						 const	{	if (nullptr==p_) return 0; else return p_->dwLength; }
	void				swap		(shared_str & rhs)				{	str_value* tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
	bool				equal		(const shared_str & rhs) const	{	return (p_ == rhs.p_);							}

	shared_str& __cdecl	printf		(const char* format, ...)		
	{
		string4096 	buf;
		va_list		p;
		va_start	(p,format);
		int vs_sz	= _vsnprintf(buf,sizeof(buf)-1,format,p); buf[sizeof(buf)-1]=0;
		va_end		(p);
		if (vs_sz)	_set(buf);	
		return 		(shared_str&)*this;
	}
};

class XRCORE_API xr_string : public std::basic_string<char, std::char_traits<char>, xalloc<char>>
{
public:
	typedef std::basic_string<char, std::char_traits<char>, xalloc<char>> Super;

	xr_string(LPCSTR Str);
	xr_string(LPCSTR Str, u32 Size);
	xr_string(const xr_string& other);
	xr_string(const xr_string&& other);
	xr_string(const Super&& other);
	xr_string();

	xr_string& operator=(LPCSTR Str);
	xr_string& operator=(const xr_string& other);
	xr_string& operator=(const Super& other);

	template <size_t ArrayLenght> xr_string(char* (&InArray)[ArrayLenght])
	{
		assign(InArray, ArrayLenght);
	}

	xr_vector<xr_string> Split(char splitCh);
	xr_vector<xr_string> Split(u32 NumberOfSplits, ...);

	bool StartWith(const xr_string& Other) const;
	bool StartWith(LPCSTR Str) const;
	bool StartWith(LPCSTR Str, size_t Size) const;
	xr_string RemoveWhitespaces() const;

	static xr_string ToString(int Value);
	static xr_string ToString(unsigned int Value);
	static xr_string ToString(float Value);
	static xr_string ToString(double Value);
	static xr_string ToString(const Fvector& Value);
	static xr_string ToString(const Dvector& Value);

	using xrStringVector = xr_vector<xr_string>;
	static xr_string Join(xrStringVector::iterator beginIter, xrStringVector::iterator endIter, const char delimeter = '\0');

	template<typename StringType> static void FixSlashes(StringType& str)
	{
		// Should be array of chars
		static_assert(std::is_same<std::remove_extent<StringType>::type, char>::value);

		constexpr size_t sizeArray = sizeof(str);

		for (size_t i = 0; i < sizeArray; ++i)
		{
			if (str[i] == '/')
			{
				str[i] = '\\';
			}
		}
	}

	template<> static void FixSlashes<xr_string>(xr_string& InStr)
	{
		for (size_t i = 0; i < InStr.size(); ++i)
		{
			if (InStr[i] == '/')
			{
				InStr[i] = '\\';
			}
		}
	}
};

using SStringVec = xr_vector<xr_string>;

// warning
// this function can be used for debug purposes only
template <typename... Args>
const char* make_string(const char* format, const Args&... args)
{
	static string4096 temp;
	snprintf(temp, sizeof(temp), format, args...);
	return temp;
}

IC bool operator==	(shared_str const & a, shared_str const & b) { return a._get() == b._get(); }
IC bool operator!=	(shared_str const & a, shared_str const & b) { return a._get() != b._get(); }
IC bool operator<	(shared_str const & a, shared_str const & b) { return a._get() <  b._get(); }
IC bool operator>	(shared_str const & a, shared_str const & b) { return a._get() >  b._get(); }

// externally visible standart functionality
IC void swap	 (shared_str & lhs, shared_str & rhs)	      { lhs.swap(rhs); }
IC u32	xr_strlen(const shared_str & a)						  { return a.size(); }
IC int	xr_strcmp(const shared_str & a, const char* b)		  { return xr_strcmp(*a,b); }
IC int	xr_strcmp(const char* a, const shared_str & b)		  { return xr_strcmp(a,*b); }
IC int	xr_strcmp(const shared_str & a, const shared_str & b) { return a.equal(b) ? 0 : xr_strcmp(*a, *b);}
IC void	xr_strlwr(xr_string& src)							  { for(char & it : src) it=xr_string::value_type(tolower(it));}
IC void	xr_strlwr(shared_str& src)							  { if (*src){char* lp=xr_strdup(*src); xr_strlwr(lp); src=lp; xr_free(lp);} }

namespace std
{
	template<> struct hash<xr_string>
	{
		size_t operator()(const xr_string& s) const
		{
			std::hash<xr_string::Super> hashFn;
			return hashFn(s);
		}
	};

	template<> struct hash<shared_str> 
	{
		std::size_t operator() (const shared_str &s) const 
		{
			return std::hash<xr_string>{}(s.c_str());
		}
	};
}
#pragma pack(pop)