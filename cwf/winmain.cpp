

#include "cwf/cwf.h"

int main() {
  using namespace cwf;

  Server server("0.0.0.0", "901", ".", 2);
  bool f = server.Init();
  if (!f)
    return -1;
  server.Run();
  return 0;
}