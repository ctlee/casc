#include <algorithm>
#include <array>
#include <cstring>
#include <future>
#include <initializer_list>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <queue>
#include <set>
#include <tuple>
#include <vector>

#include <casc/casc>
#include <experimental/optional>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Wrong arguments passed" << std::endl;
    return -1;
  }

  auto pF = readOFF(argv[1]);

  auto rval = compute_orientation(*pF);

  std::cout << std::get<0>(rval) << " " << std::get<1>(rval) << " "
            << std::get<2>(rval) << std::endl;

  if (strcmp(argv[2], "true") == 0) {
    if (std::get<1>(rval)) {
      return 0;
    } else {
      std::cerr << "False negative. Algorithm decided that \"" << argv[1]
                << "\" was not orientable, but should have decided that it was."
                << std::endl;
      return -1;
    }
  } else {
    if (std::get<1>(rval)) {
      std::cerr << "False positive. Algorithm decided that \"" << argv[1]
                << "\" was orientable, but should have decided that it was not."
                << std::endl;
      return -1;
    } else {
      return 0;
    }
  }
}
