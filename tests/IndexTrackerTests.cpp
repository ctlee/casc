// This file is part of the Colored Abstract Simplicial Complex library.
// Copyright (C) 2016-2021
// by Christopher T. Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
//    and Michael Holst
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, see <http://www.gnu.org/licenses/>
// or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301 USA

#include "gtest/gtest.h"
#include <casc/casc>

// Tests factorial of positive numbers.
TEST(IntervalTest, GeneralOperation) {
  index_tracker::index_tracker<int> idx;
  for (int i = 0; i < 1000; ++i) {
    idx.remove(i);
  }
  for (int i = 0; i < 500; ++i) {
    idx.insert(2 * i);
  }
}
