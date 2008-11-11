#if 0
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace io = boost::iostreams;

int main() {

  bio::filtering_ostream out;
  out.push(bio::gzip_compressor());
  // out.push(base64_encoder());
  // out.push(file_sink("my_file.txt"));
  // write to out using std::ostream interface

  out << "";

  return 0;
}
// bjam release debug link=static runtime-link=static stage --with-system
#endif

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace boost::iostreams;

int main() {
  typedef stream<file_sink> ofstream;

  ofstream out("HeavyArtillery.txt"); // Wilfred Owen
  out << "Reach at that Arrogance which needs thy harm,\n"
  "And beat it down before its sins grow worse.\n";
  out.close();
  return 0;
}
