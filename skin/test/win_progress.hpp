#ifndef __WIN_PROGRESS_H__
#define __WIN_PROGRESS_H__

#include <boost/config.hpp>

#ifdef BOOST_WINDOWS

#include <iostream>
#include <windows.h>
#include <boost/utility.hpp>  // for noncopyable

namespace boost {

class win_progress_timer : /*public timer, */ private noncopyable
{
 public:
  explicit win_progress_timer( std::ostream & os = std::cout )
     // os is hint; implementation may ignore, particularly in embedded systems
     : m_os(os) 
  {
      QueryPerformanceCounter(&_start);
  }
  
  ~win_progress_timer()
  {
  //  A) Throwing an exception from a destructor is a Bad Thing.
  //  B) The progress_timer destructor does output which may throw.
  //  C) A progress_timer is usually not critical to the application.
  //  Therefore, wrap the I/O in a try block, catch and ignore all exceptions.
    try
    {
      // use istream instead of ios_base to workaround GNU problem (Greg Chicares)
      std::istream::fmtflags old_flags = m_os.setf( std::istream::fixed,
                                                   std::istream::floatfield );
      std::streamsize old_prec = m_os.precision( 2 );
      m_os << elapsed() << " ms\n"
                        << std::endl;
      m_os.flags( old_flags );
      m_os.precision( old_prec );
    }

    catch (...) {} // eat any exceptions
  } // ~progress_timer

  void restart()
  {
      QueryPerformanceCounter(&_start);
  }
  double elapsed() const
  {
      LARGE_INTEGER end, freq;
      QueryPerformanceCounter(&end);
        
      QueryPerformanceFrequency(&freq);

      return (double)(end.QuadPart - _start.QuadPart) * 1000 / freq.LowPart;
  }

 private:
  std::ostream & m_os;
  LARGE_INTEGER _start;
};

typedef win_progress_timer progress_timer;

} // namespace boost

#else // if not BOOST_WINDOWS

  #include <boost/progress.hpp>

#endif // #ifdef BOOST_WINDOWS

#endif // __WIN_PROGRESS_H__