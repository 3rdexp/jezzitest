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

// using namespace boost::iostreams;
namespace bio = boost::iostreams;


template<typename Container>
class container_sink {
public:
  typedef typename Container::value_type  char_type;
  typedef bio::sink_tag                        category;
  container_sink(Container& container) : container_(container) { }
  std::streamsize write(const char_type* s, std::streamsize n)
  {
    container_.insert(container_.end(), s, s + n);
    return n;
  }
  Container& container() { return container_; }
private:
  Container& container_;
};

const char *data = "Reach at that Arrogance which needs thy harm,\n"
  "And beat it down before its sins grow worse.\n";

void test() {
  typedef container_sink<std::string> string_sink;
  std::string result;
  
  {
    bio::filtering_ostream chain;
    chain.push(bio::counter());
    chain.push(bio::gzip_compressor());
    chain.push(bio::counter());
    chain.push(string_sink(result));

    bio::counter*  first_counter = BOOST_IOSTREAMS_COMPONENT(chain, 0, bio::counter);
    bio::counter*  second_counter = BOOST_IOSTREAMS_COMPONENT(chain, 2, bio::counter);

    chain << data;
    chain.flush();

    std::cout << "line:" << first_counter->lines() << " character:" << first_counter->characters() << std::endl;
    std::cout << "line:" << second_counter->lines() << " character:" << second_counter->characters() << std::endl;
  }

  std::cout << "result: " << result.size() << std::endl;
  __asm int 3;
}

int main() {
//  test();
//  return 0;

  bio::stream_buffer<bio::file_sink> output_buffer;
  output_buffer.open(bio::file_sink("sink.out"));

  // output_buffer << data;

  bio::counter c1, c2;
  {
    bio::filtering_ostream out;
    //out.push(bio::counter());
    out.push(bio::gzip_compressor());
    out.push(bio::counter());
    out.push(bio::file_sink("txt.gz"));

    //bio::counter*  first_counter = BOOST_IOSTREAMS_COMPONENT(out, 0, bio::counter);
    bio::counter*  second_counter = BOOST_IOSTREAMS_COMPONENT(out, 1, bio::counter);

    int i=0;
    while(i++ < 10)
      out << data;

    // out.flush();
    out.strict_sync();

    //std::cout << "line:" << first_counter->lines() << " character:" << first_counter->characters() << std::endl;
    std::cout << "line:" << second_counter->lines() << " character:" << second_counter->characters() << std::endl;
  }
  return 0;
}
