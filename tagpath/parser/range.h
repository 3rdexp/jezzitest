#ifndef __RANGE_H__
#define __RANGE_H__

#include <cctype>
#include <string>

namespace kong {

//////////////////////////////////////////////////////////////////////////
// Range for c string
//////////////////////////////////////////////////////////////////////////
template<typename T>
class range_base
{
public:
    typedef T* iterator;
    typedef const T* const_iterator;

    const T* begin() const { return begin_; }
    const T* end() const { return end_; }
    size_t size() const { return std::distance(begin_, end_); }
    bool empty() const { return (begin_ == end_); }

    // why push_back:
    // 操作和 std::string 比较类似
    void push_back(char) { assert(begin_); end_ ++; }
    void clear() { begin_ = 0; end_ = 0; }
    void begin(const T* b) { begin_ = b; }
    void end(const T* e) { end_ = e; }

    std::string to_string() const
    {
        return std::string(begin(), size());
    }

    explicit range(const T* b = 0, const T* e = 0) : begin_(b), end_(e) {}
private:
    const T* begin_;
    const T* end_;
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const range<T> & r)
{
#pragma warning (disable : 4267)
    stream.write(r.begin(), r.size());
#pragma warning (default : 4267)
    return stream;
}

#if 1
  typedef range_base<char> range_t;
#else
  typedef std::string range_t;
#endif

typedef std::vector<std::pair<range_t, range_t> > attrarray_t;

} // kong

#endif // __RANGE_H__
