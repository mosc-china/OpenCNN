/*  ---------------------------------------------------------------------
  Copyright 2018-2019 Fangjun Kuang
  email: csukuangfj at gmail dot com
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a COPYING file of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>
  -----------------------------------------------------------------  */
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "cnn/jet.hpp"

namespace cnn {

template <typename Dtype>
class JetTest : public ::testing::Test {};

using MyTypes = ::testing::Types<float, double>;
TYPED_TEST_CASE(JetTest, MyTypes);

// x = x + 1
template <typename Dtype>
Dtype add_scalar1(const Dtype& x) {
  Dtype y;
  y = x + Dtype(1);
  return y;
}

// x = 1 + x
template <typename Dtype>
Dtype add_scalar2(const Dtype& x) {
  Dtype y;
  y = Dtype(1) + x;
  return y;
}

// x += 1
template <typename Dtype>
Dtype add_scalar3(const Dtype& x) {
  Dtype y(x);
  y += Dtype(1);
  return y;
}

// x = x - 1
template <typename Dtype>
Dtype sub_scalar1(const Dtype& x) {
  Dtype y;
  y = x - Dtype(1);
  return y;
}

// x = 1 - x
template <typename Dtype>
Dtype sub_scalar2(const Dtype& x) {
  Dtype y;
  y = Dtype(1) - x;
  return y;
}

// x -= 1
template <typename Dtype>
Dtype sub_scalar3(const Dtype& x) {
  Dtype y(x);
  y -= Dtype(1);
  return y;
}

// x = x * 2
template <typename Dtype>
Dtype mul_scalar1(const Dtype& x) {
  Dtype y;
  y = x * Dtype(2);
  return y;
}

// x = 2 * x
template <typename Dtype>
Dtype mul_scalar2(const Dtype& x) {
  Dtype y;
  y = Dtype(2) * x;
  return y;
}

// x *= 2
template <typename Dtype>
Dtype mul_scalar3(const Dtype& x) {
  Dtype y(x);
  y *= Dtype(2);
  return y;
}

// x = x / 2
template <typename Dtype>
Dtype div_scalar1(const Dtype& x) {
  Dtype y;
  y = x / Dtype(2);
  return y;
}

// x = 2 / x
template <typename Dtype>
Dtype div_scalar2(const Dtype& x) {
  Dtype y;
  y = Dtype(2) / x;
  return y;
}

// x /= 2
template <typename Dtype>
Dtype div_scalar3(const Dtype& x) {
  Dtype y(x);
  y /= Dtype(2);
  return y;
}

template <typename Dtype>
Dtype jet_mul_div_add(const Dtype& x, const Dtype& y) {
  Dtype z;
  z = x * y / (x * x + y);
  return z;
  // from https://www.whitman.edu/mathematics/calculus_online/section14.03.html
  /*
   * dz/dx = (y*y - x*x*y)/((x*x+y)*(x*x+y))
   * dz/dy = x*x*x/((x*x+y)*(x*x+y))
   */
}

template <typename Dtype>
Dtype jet_sub_div(const Dtype& x, const Dtype& y) {
  Dtype z;
  z = (x - y) / (x + y);
  return z;
  // http://math.gmu.edu/~memelian/teaching/Fall08/partDerivExamples.pdf
  // 1 (e)
  /*
   * dz/dx = 2y/((x+y)*(x+y))
   * dz/dy = -2x/((x+y)*(x+y))
   */
}

TYPED_TEST(JetTest, scalar_add) {
  Jet<TypeParam, 1> f(10, 0);

  // x = x + 1
  f = add_scalar1(f);
  EXPECT_EQ(f.a_, 11);
  EXPECT_EQ(f.v_[0], 1);

  // x = 1 + x
  f = add_scalar2(f);
  EXPECT_EQ(f.a_, 12);
  EXPECT_EQ(f.v_[0], 1);

  // x += 1
  f = add_scalar2(f);
  EXPECT_EQ(f.a_, 13);
  EXPECT_EQ(f.v_[0], 1);
}

TYPED_TEST(JetTest, scalar_sub) {
  Jet<TypeParam, 1> f(10, 0);

  // x = x - 1
  f = sub_scalar1(f);
  EXPECT_EQ(f.a_, 9);
  EXPECT_EQ(f.v_[0], 1);

  // x = 1 - x
  f = sub_scalar2(f);
  EXPECT_EQ(f.a_, -8);
  EXPECT_EQ(f.v_[0], -1);

  // x -= 1
  f = sub_scalar3(f);
  EXPECT_EQ(f.a_, -9);
  EXPECT_EQ(f.v_[0], -1);
}

TYPED_TEST(JetTest, negate) {
  Jet<TypeParam, 1> f(10, 0);

  // x = -x
  f = -f;
  EXPECT_EQ(f.a_, -10);
  EXPECT_EQ(f.v_[0], -1);
}

TYPED_TEST(JetTest, scalar_mul) {
  Jet<TypeParam, 1> f(10, 0);

  // x = x * 2
  f = mul_scalar1(f);
  EXPECT_EQ(f.a_, 20);
  EXPECT_EQ(f.v_[0], 2);

  // x = 2 * x
  f = mul_scalar2(f);
  EXPECT_EQ(f.a_, 40);
  EXPECT_EQ(f.v_[0], 4);

  // x *= 2
  f = mul_scalar3(f);
  EXPECT_EQ(f.a_, 80);
  EXPECT_EQ(f.v_[0], 8);
}

TYPED_TEST(JetTest, scalar_div) {
  Jet<TypeParam, 1> f(10, 0);

  // x = x / 2
  f = div_scalar1(f);
  EXPECT_EQ(f.a_, 5);
  EXPECT_EQ(f.v_[0], 0.5);

  // x = 2 / x
  f = div_scalar2(f);
  EXPECT_NEAR(f.a_, 0.4, 1e-7);
  EXPECT_EQ(f.v_[0], -TypeParam(2) / 25 * 0.5);

  // x /= 2
  f = div_scalar3(f);
  EXPECT_NEAR(f.a_, 0.2, 1e-7);
  EXPECT_EQ(f.v_[0], -TypeParam(2) / 25 * 0.5 * 0.5);
}

TYPED_TEST(JetTest, jet_mul_div_add_test) {
  TypeParam x = 1;
  TypeParam y = 2;
  Jet<TypeParam, 2> a(x, 0);
  Jet<TypeParam, 2> b(y, 1);
  auto z = jet_mul_div_add(a, b);

  EXPECT_NEAR(z.a_, jet_mul_div_add(x, y), 1e-6);
  TypeParam dz_dx;
  TypeParam dz_dy;

  dz_dx = (y * y - x * x * y) / ((x * x + y) * (x * x + y));
  dz_dy = x * x * x / ((x * x + y) * (x * x + y));
  EXPECT_NEAR(z.v_[0], dz_dx, 1e-7);
  EXPECT_NEAR(z.v_[1], dz_dy, 1e-7);
}

TYPED_TEST(JetTest, jet_sub_div) {
  TypeParam x = 1;
  TypeParam y = 2;
  Jet<TypeParam, 2> a(x, 0);
  Jet<TypeParam, 2> b(y, 1);
  auto z = jet_sub_div(a, b);

  EXPECT_NEAR(z.a_, jet_sub_div(x, y), 1e-7);
  TypeParam dz_dx;
  TypeParam dz_dy;

  dz_dx = 2 * y / ((x + y) * (x + y));
  dz_dy = -2 * x / ((x + y) * (x + y));
  EXPECT_NEAR(z.v_[0], dz_dx, 1e-7);
  EXPECT_NEAR(z.v_[1], dz_dy, 1e-7);
}

TYPED_TEST(JetTest, jet_exp_log) {
  using Type = Jet<TypeParam, 1>;
  Type x(exp(TypeParam(2)), 0, 10);

  {
    auto f = exp(x);
    auto g = log(f);
    EXPECT_NEAR(g.a_, x.a_, 1e-6);
    EXPECT_NEAR(g.v_[0], x.v_[0], 1e-6);
  }

  {
    auto f = log(x);
    auto g = exp(f);
    EXPECT_NEAR(g.a_, x.a_, 1e-6);
    EXPECT_NEAR(g.v_[0], x.v_[0], 1e-6);
  }
}

TYPED_TEST(JetTest, jet_sqrt) {
  using Type = Jet<TypeParam, 1>;
  Type x(exp(TypeParam(2)), 0, 10);

  {
    auto f = sqrt(x);
    auto g = f * f;
    EXPECT_NEAR(g.a_, x.a_, 1e-6);
    EXPECT_NEAR(g.v_[0], x.v_[0], 1e-6);
  }

  {
    auto f = x * x;
    auto g = sqrt(f);
    EXPECT_NEAR(g.a_, x.a_, 1e-6);
    EXPECT_NEAR(g.v_[0], x.v_[0], 1e-6);
  }
}

}  // namespace cnn
