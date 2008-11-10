#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace io = boost::iostreams;

int main()
{   
  io::filtering_ostream out;
  out.push(compressor());
  out.push(base64_encoder());
  out.push(file_sink("my_file.txt"));
  // write to out using std::ostream interface

  return 0;
}