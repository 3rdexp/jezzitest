// set ZLIB_INCLUDE
// set ZLIB_SOURCE
// bjam release debug link=static runtime-link=static stage --with-system

#include <iostream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/counter.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <boost/iostreams/stream_buffer.hpp>

using namespace boost::iostreams;

int main() {
  typedef stream<file_sink> ofstream;

  stream_buffer<file_sink> output_buffer;
  output_buffer.open(file_sink("sink.out"));

  const char *data = "Reach at that Arrogance which needs thy harm,\n"
    "And beat it down before its sins grow worse.\n";

  // output_buffer << data;

  counter c1, c2;
  {
    filtering_ostream out;
    out.push(counter());
    out.push(gzip_compressor());
    // out.push(counter());
    out.push(file_sink("txt.gz"));

    counter*  first_counter = BOOST_IOSTREAMS_COMPONENT(out, 0, counter);
    counter*  second_counter = BOOST_IOSTREAMS_COMPONENT(out, 2, counter);

    out << data;

    out.flush();

    std::cout << "line:" << first_counter->lines() << " character:" << first_counter->characters() << std::endl;
    std::cout << "line:" << second_counter->lines() << " character:" << second_counter->characters() << std::endl;
  }
  return 0;
}
