// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#include "catch2/catch.hpp"
#include "cx_test_util.hpp"
#include "ryu/ryu.hpp"

#include <cmath>
#include <cstdint>

struct test_case {
  double      value;
  uint32_t    fixed_precision;
  const char* fixed_string;
  uint32_t    exp_precision;
  const char* exp_string;
};

static constexpr bool test_d2fixed(double d, uint32_t precision, const char* expected) {
  char buf[2048]{0};
  auto size = ryu::cx::d2fixed_buffered_n(d, precision, buf);
  return cmp(buf, expected, size);
}

#define EXPECT_FIXED(d, precision, exp) STATIC_REQUIRE(test_d2fixed(d, precision, exp))

TEST_CASE("cx::D2fixedTest") {
  SECTION("Basic") {
    // TODO: #3
    // EXPECT_FIXED(ieeeParts2Double(false, 1234, 99999),
    //              0,
    //              "3291009114715486435425664845573426149758869524108446525879746560");
  }

  SECTION("Zero") {
    EXPECT_FIXED(0.0, 4, "0.0000");
    EXPECT_FIXED(0.0, 3, "0.000");
    EXPECT_FIXED(0.0, 2, "0.00");
    EXPECT_FIXED(0.0, 1, "0.0");
    EXPECT_FIXED(0.0, 0, "0");
  }

  SECTION("MinMax") {
    // TODO: #4 investigate MinMax constexpr double test
    // EXPECT_FIXED(
    //     ieeeParts2Double(false, 0, 1),
    //     1074,
    //     "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    //     "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    //     "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    //     "000000000000000000000000000000000000000000000000000000049406564584124654417656879286822137"
    //     "236505980261432476442558568250067550727020875186529983636163599237979656469544571773092665"
    //     "671035593979639877479601078187812630071319031140452784581716784898210368871863605699873072"
    //     "305000638740915356498438731247339727316961514003171538539807412623856559117102665855668676"
    //     "818703956031062493194527159149245532930545654440112748012970999954193198940908041656332452"
    //     "475714786901472678015935523861155013480352649347201937902681071074917033322268447533357208"
    //     "324319360923828934583680601060115061698097530783422773183292479049825247307763759272478746"
    //     "560847782037344696995336470179726777175851256605511991315048911014510378627381672509558373"
    //     "89733598993664809941164205702637090279242767544565229087538682506419718265533447265625");

    // EXPECT_FIXED(
    //     ieeeParts2Double(false, 2046, 0xFFFFFFFFFFFFFu),
    //     0,
    //     "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558"
    //     "632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245"
    //     "490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168"
    //     "738177180919299881250404026184124858368");
  }

  SECTION("RoundToEven") {
    EXPECT_FIXED(0.125, 3, "0.125");
    EXPECT_FIXED(0.125, 2, "0.12");
    EXPECT_FIXED(0.375, 3, "0.375");
    EXPECT_FIXED(0.375, 2, "0.38");
  }

  SECTION("RoundToEvenInteger") {
    EXPECT_FIXED(2.5, 1, "2.5");
    EXPECT_FIXED(2.5, 0, "2");
    EXPECT_FIXED(3.5, 1, "3.5");
    EXPECT_FIXED(3.5, 0, "4");
  }

  SECTION("NonRoundToEvenScenarios") {
    EXPECT_FIXED(0.748046875, 3, "0.748");
    EXPECT_FIXED(0.748046875, 2, "0.75");
    EXPECT_FIXED(0.748046875, 1, "0.7"); // 0.75 would round to "0.8", but this is smaller

    EXPECT_FIXED(0.2509765625, 3, "0.251");
    EXPECT_FIXED(0.2509765625, 2, "0.25");
    EXPECT_FIXED(0.2509765625, 1, "0.3"); // 0.25 would round to "0.2", but this is larger

    EXPECT_FIXED(ieeeParts2Double(false, 1021, 1),
                 54,
                 "0.250000000000000055511151231257827021181583404541015625");
    EXPECT_FIXED(ieeeParts2Double(false, 1021, 1), 3, "0.250");
    EXPECT_FIXED(ieeeParts2Double(false, 1021, 1), 2, "0.25");
    EXPECT_FIXED(ieeeParts2Double(false, 1021, 1),
                 1,
                 "0.3"); // 0.25 would round to "0.2", but this is larger (again)
  }

  SECTION("VaryingPrecision") {
    EXPECT_FIXED(1729.142857142857, 47, "1729.14285714285711037518922239542007446289062500000");
    EXPECT_FIXED(1729.142857142857, 46, "1729.1428571428571103751892223954200744628906250000");
    EXPECT_FIXED(1729.142857142857, 45, "1729.142857142857110375189222395420074462890625000");
    EXPECT_FIXED(1729.142857142857, 44, "1729.14285714285711037518922239542007446289062500");
    EXPECT_FIXED(1729.142857142857, 43, "1729.1428571428571103751892223954200744628906250");
    EXPECT_FIXED(1729.142857142857, 42, "1729.142857142857110375189222395420074462890625");
    EXPECT_FIXED(1729.142857142857, 41, "1729.14285714285711037518922239542007446289062");
    EXPECT_FIXED(1729.142857142857, 40, "1729.1428571428571103751892223954200744628906");
    EXPECT_FIXED(1729.142857142857, 39, "1729.142857142857110375189222395420074462891");
    EXPECT_FIXED(1729.142857142857, 38, "1729.14285714285711037518922239542007446289");
    EXPECT_FIXED(1729.142857142857, 37, "1729.1428571428571103751892223954200744629");
    EXPECT_FIXED(1729.142857142857, 36, "1729.142857142857110375189222395420074463");
    EXPECT_FIXED(1729.142857142857, 35, "1729.14285714285711037518922239542007446");
    EXPECT_FIXED(1729.142857142857, 34, "1729.1428571428571103751892223954200745");
    EXPECT_FIXED(1729.142857142857, 33, "1729.142857142857110375189222395420074");
    EXPECT_FIXED(1729.142857142857, 32, "1729.14285714285711037518922239542007");
    EXPECT_FIXED(1729.142857142857, 31, "1729.1428571428571103751892223954201");
    EXPECT_FIXED(1729.142857142857, 30, "1729.142857142857110375189222395420");
    EXPECT_FIXED(1729.142857142857, 29, "1729.14285714285711037518922239542");
    EXPECT_FIXED(1729.142857142857, 28, "1729.1428571428571103751892223954");
    EXPECT_FIXED(1729.142857142857, 27, "1729.142857142857110375189222395");
    EXPECT_FIXED(1729.142857142857, 26, "1729.14285714285711037518922240");
    EXPECT_FIXED(1729.142857142857, 25, "1729.1428571428571103751892224");
    EXPECT_FIXED(1729.142857142857, 24, "1729.142857142857110375189222");
    EXPECT_FIXED(1729.142857142857, 23, "1729.14285714285711037518922");
    EXPECT_FIXED(1729.142857142857, 22, "1729.1428571428571103751892");
    EXPECT_FIXED(1729.142857142857, 21, "1729.142857142857110375189");
    EXPECT_FIXED(1729.142857142857, 20, "1729.14285714285711037519");
    EXPECT_FIXED(1729.142857142857, 19, "1729.1428571428571103752");
    EXPECT_FIXED(1729.142857142857, 18, "1729.142857142857110375");
    EXPECT_FIXED(1729.142857142857, 17, "1729.14285714285711038");
    EXPECT_FIXED(1729.142857142857, 16, "1729.1428571428571104");
    EXPECT_FIXED(1729.142857142857, 15, "1729.142857142857110");
    EXPECT_FIXED(1729.142857142857, 14, "1729.14285714285711");
    EXPECT_FIXED(1729.142857142857, 13, "1729.1428571428571");
    EXPECT_FIXED(1729.142857142857, 12, "1729.142857142857");
    EXPECT_FIXED(1729.142857142857, 11, "1729.14285714286");
    EXPECT_FIXED(1729.142857142857, 10, "1729.1428571429");
    EXPECT_FIXED(1729.142857142857, 9, "1729.142857143");
    EXPECT_FIXED(1729.142857142857, 8, "1729.14285714");
    EXPECT_FIXED(1729.142857142857, 7, "1729.1428571");
    EXPECT_FIXED(1729.142857142857, 6, "1729.142857");
    EXPECT_FIXED(1729.142857142857, 5, "1729.14286");
    EXPECT_FIXED(1729.142857142857, 4, "1729.1429");
    EXPECT_FIXED(1729.142857142857, 3, "1729.143");
    EXPECT_FIXED(1729.142857142857, 2, "1729.14");
    EXPECT_FIXED(1729.142857142857, 1, "1729.1");
    EXPECT_FIXED(1729.142857142857, 0, "1729");
  }

  SECTION("Carrying") {
    EXPECT_FIXED(0.0009, 4, "0.0009");
    EXPECT_FIXED(0.0009, 3, "0.001");
    EXPECT_FIXED(0.0029, 4, "0.0029");
    EXPECT_FIXED(0.0029, 3, "0.003");
    EXPECT_FIXED(0.0099, 4, "0.0099");
    EXPECT_FIXED(0.0099, 3, "0.010");
    EXPECT_FIXED(0.0299, 4, "0.0299");
    EXPECT_FIXED(0.0299, 3, "0.030");
    EXPECT_FIXED(0.0999, 4, "0.0999");
    EXPECT_FIXED(0.0999, 3, "0.100");
    EXPECT_FIXED(0.2999, 4, "0.2999");
    EXPECT_FIXED(0.2999, 3, "0.300");
    EXPECT_FIXED(0.9999, 4, "0.9999");
    EXPECT_FIXED(0.9999, 3, "1.000");
    EXPECT_FIXED(2.9999, 4, "2.9999");
    EXPECT_FIXED(2.9999, 3, "3.000");
    EXPECT_FIXED(9.9999, 4, "9.9999");
    EXPECT_FIXED(9.9999, 3, "10.000");
    EXPECT_FIXED(29.9999, 4, "29.9999");
    EXPECT_FIXED(29.9999, 3, "30.000");
    EXPECT_FIXED(99.9999, 4, "99.9999");
    EXPECT_FIXED(99.9999, 3, "100.000");
    EXPECT_FIXED(299.9999, 4, "299.9999");
    EXPECT_FIXED(299.9999, 3, "300.000");

    EXPECT_FIXED(0.09, 2, "0.09");
    EXPECT_FIXED(0.09, 1, "0.1");
    EXPECT_FIXED(0.29, 2, "0.29");
    EXPECT_FIXED(0.29, 1, "0.3");
    EXPECT_FIXED(0.99, 2, "0.99");
    EXPECT_FIXED(0.99, 1, "1.0");
    EXPECT_FIXED(2.99, 2, "2.99");
    EXPECT_FIXED(2.99, 1, "3.0");
    EXPECT_FIXED(9.99, 2, "9.99");
    EXPECT_FIXED(9.99, 1, "10.0");
    EXPECT_FIXED(29.99, 2, "29.99");
    EXPECT_FIXED(29.99, 1, "30.0");
    EXPECT_FIXED(99.99, 2, "99.99");
    EXPECT_FIXED(99.99, 1, "100.0");
    EXPECT_FIXED(299.99, 2, "299.99");
    EXPECT_FIXED(299.99, 1, "300.0");

    EXPECT_FIXED(0.9, 1, "0.9");
    EXPECT_FIXED(0.9, 0, "1");
    EXPECT_FIXED(2.9, 1, "2.9");
    EXPECT_FIXED(2.9, 0, "3");
    EXPECT_FIXED(9.9, 1, "9.9");
    EXPECT_FIXED(9.9, 0, "10");
    EXPECT_FIXED(29.9, 1, "29.9");
    EXPECT_FIXED(29.9, 0, "30");
    EXPECT_FIXED(99.9, 1, "99.9");
    EXPECT_FIXED(99.9, 0, "100");
    EXPECT_FIXED(299.9, 1, "299.9");
    EXPECT_FIXED(299.9, 0, "300");
  }

  SECTION("RoundingResultZero") {
    EXPECT_FIXED(0.004, 3, "0.004");
    EXPECT_FIXED(0.004, 2, "0.00");
    EXPECT_FIXED(0.4, 1, "0.4");
    EXPECT_FIXED(0.4, 0, "0");
    EXPECT_FIXED(0.5, 1, "0.5");
    EXPECT_FIXED(0.5, 0, "0");
  }

  SECTION("Regression") { EXPECT_FIXED(7.018232e-82, 6, "0.000000"); }
}

static constexpr bool test_d2exp(double d, uint32_t precision, const char* expected) {
  char buf[2048]{0};
  auto size = ryu::cx::d2exp_buffered_n(d, precision, buf);
  return cmp(buf, expected, size);
}

#define EXPECT_EXP(a, b, c) STATIC_REQUIRE(test_d2exp(a, b, c))

TEST_CASE("cx::d2exp_buffered") {
  // TODO: #5 investigate constexpr ryu test d2exp_buffered::Basic
  SECTION("Basic") {
    // EXPECT_EXP(ieeeParts2Double(false, 1234, 99999),
    //            62,
    //            "3.29100911471548643542566484557342614975886952410844652587974656e+63");
  }

  SECTION("Zero") {
    EXPECT_EXP(0.0, 4, "0.0000e+00");
    EXPECT_EXP(0.0, 3, "0.000e+00");
    EXPECT_EXP(0.0, 2, "0.00e+00");
    EXPECT_EXP(0.0, 1, "0.0e+00");
    EXPECT_EXP(0.0, 0, "0e+00");
  }

  // TODO: #4
  SECTION("MinMax") {
    // EXPECT_EXP(
    //     ieeeParts2Double(false, 0, 1),
    //     750,
    //     "4.9406564584124654417656879286822137236505980261432476442558568250067550727020875186529983"
    //     "636163599237979656469544571773092665671035593979639877479601078187812630071319031140452784"
    //     "581716784898210368871863605699873072305000638740915356498438731247339727316961514003171538"
    //     "539807412623856559117102665855668676818703956031062493194527159149245532930545654440112748"
    //     "012970999954193198940908041656332452475714786901472678015935523861155013480352649347201937"
    //     "902681071074917033322268447533357208324319360923828934583680601060115061698097530783422773"
    //     "183292479049825247307763759272478746560847782037344696995336470179726777175851256605511991"
    //     "315048911014510378627381672509558373897335989936648099411642057026370902792427675445652290"
    //     "87538682506419718265533447265625e-324");

    // EXPECT_EXP(
    //     ieeeParts2Double(false, 2046, 0xFFFFFFFFFFFFFu),
    //     308,
    //     "1.7976931348623157081452742373170435679807056752584499659891747680315726078002853876058955"
    //     "863276687817154045895351438246423432132688946418276846754670353751698604991057655128207624"
    //     "549009038932894407586850845513394230458323690322294816580855933212334827479782620414472316"
    //     "8738177180919299881250404026184124858368e+308");
  }

  SECTION("RoundToEven") {
    EXPECT_EXP(0.125, 2, "1.25e-01");
    EXPECT_EXP(0.125, 1, "1.2e-01");
    EXPECT_EXP(0.375, 2, "3.75e-01");
    EXPECT_EXP(0.375, 1, "3.8e-01");
  }

  SECTION("RoundToEvenInteger") {
    EXPECT_EXP(2.5, 1, "2.5e+00");
    EXPECT_EXP(2.5, 0, "2e+00");
    EXPECT_EXP(3.5, 1, "3.5e+00");
    EXPECT_EXP(3.5, 0, "4e+00");
  }

  SECTION("NonRoundToEvenScenarios") {
    EXPECT_EXP(0.748046875, 2, "7.48e-01");
    EXPECT_EXP(0.748046875, 1, "7.5e-01");
    EXPECT_EXP(0.748046875, 0, "7e-01"); // 0.75 would round to "8e-01", but this is smaller

    EXPECT_EXP(0.2509765625, 2, "2.51e-01");
    EXPECT_EXP(0.2509765625, 1, "2.5e-01");
    EXPECT_EXP(0.2509765625, 0, "3e-01"); // 0.25 would round to "2e-01", but this is larger

    EXPECT_EXP(ieeeParts2Double(false, 1021, 1),
               53,
               "2.50000000000000055511151231257827021181583404541015625e-01");
    EXPECT_EXP(ieeeParts2Double(false, 1021, 1), 2, "2.50e-01");
    EXPECT_EXP(ieeeParts2Double(false, 1021, 1), 1, "2.5e-01");
    EXPECT_EXP(ieeeParts2Double(false, 1021, 1),
               0,
               "3e-01"); // 0.25 would round to "2e-01", but this is larger (again)
  }

  SECTION("VaryingPrecision") {
    EXPECT_EXP(1729.142857142857, 50, "1.72914285714285711037518922239542007446289062500000e+03");
    EXPECT_EXP(1729.142857142857, 49, "1.7291428571428571103751892223954200744628906250000e+03");
    EXPECT_EXP(1729.142857142857, 48, "1.729142857142857110375189222395420074462890625000e+03");
    EXPECT_EXP(1729.142857142857, 47, "1.72914285714285711037518922239542007446289062500e+03");
    EXPECT_EXP(1729.142857142857, 46, "1.7291428571428571103751892223954200744628906250e+03");
    EXPECT_EXP(1729.142857142857, 45, "1.729142857142857110375189222395420074462890625e+03");
    EXPECT_EXP(1729.142857142857, 44, "1.72914285714285711037518922239542007446289062e+03");
    EXPECT_EXP(1729.142857142857, 43, "1.7291428571428571103751892223954200744628906e+03");
    EXPECT_EXP(1729.142857142857, 42, "1.729142857142857110375189222395420074462891e+03");
    EXPECT_EXP(1729.142857142857, 41, "1.72914285714285711037518922239542007446289e+03");
    EXPECT_EXP(1729.142857142857, 40, "1.7291428571428571103751892223954200744629e+03");
    EXPECT_EXP(1729.142857142857, 39, "1.729142857142857110375189222395420074463e+03");
    EXPECT_EXP(1729.142857142857, 38, "1.72914285714285711037518922239542007446e+03");
    EXPECT_EXP(1729.142857142857, 37, "1.7291428571428571103751892223954200745e+03");
    EXPECT_EXP(1729.142857142857, 36, "1.729142857142857110375189222395420074e+03");
    EXPECT_EXP(1729.142857142857, 35, "1.72914285714285711037518922239542007e+03");
    EXPECT_EXP(1729.142857142857, 34, "1.7291428571428571103751892223954201e+03");
    EXPECT_EXP(1729.142857142857, 33, "1.729142857142857110375189222395420e+03");
    EXPECT_EXP(1729.142857142857, 32, "1.72914285714285711037518922239542e+03");
    EXPECT_EXP(1729.142857142857, 31, "1.7291428571428571103751892223954e+03");
    EXPECT_EXP(1729.142857142857, 30, "1.729142857142857110375189222395e+03");
    EXPECT_EXP(1729.142857142857, 29, "1.72914285714285711037518922240e+03");
    EXPECT_EXP(1729.142857142857, 28, "1.7291428571428571103751892224e+03");
    EXPECT_EXP(1729.142857142857, 27, "1.729142857142857110375189222e+03");
    EXPECT_EXP(1729.142857142857, 26, "1.72914285714285711037518922e+03");
    EXPECT_EXP(1729.142857142857, 25, "1.7291428571428571103751892e+03");
    EXPECT_EXP(1729.142857142857, 24, "1.729142857142857110375189e+03");
    EXPECT_EXP(1729.142857142857, 23, "1.72914285714285711037519e+03");
    EXPECT_EXP(1729.142857142857, 22, "1.7291428571428571103752e+03");
    EXPECT_EXP(1729.142857142857, 21, "1.729142857142857110375e+03");
    EXPECT_EXP(1729.142857142857, 20, "1.72914285714285711038e+03");
    EXPECT_EXP(1729.142857142857, 19, "1.7291428571428571104e+03");
    EXPECT_EXP(1729.142857142857, 18, "1.729142857142857110e+03");
    EXPECT_EXP(1729.142857142857, 17, "1.72914285714285711e+03");
    EXPECT_EXP(1729.142857142857, 16, "1.7291428571428571e+03");
    EXPECT_EXP(1729.142857142857, 15, "1.729142857142857e+03");
    EXPECT_EXP(1729.142857142857, 14, "1.72914285714286e+03");
    EXPECT_EXP(1729.142857142857, 13, "1.7291428571429e+03");
    EXPECT_EXP(1729.142857142857, 12, "1.729142857143e+03");
    EXPECT_EXP(1729.142857142857, 11, "1.72914285714e+03");
    EXPECT_EXP(1729.142857142857, 10, "1.7291428571e+03");
    EXPECT_EXP(1729.142857142857, 9, "1.729142857e+03");
    EXPECT_EXP(1729.142857142857, 8, "1.72914286e+03");
    EXPECT_EXP(1729.142857142857, 7, "1.7291429e+03");
    EXPECT_EXP(1729.142857142857, 6, "1.729143e+03");
    EXPECT_EXP(1729.142857142857, 5, "1.72914e+03");
    EXPECT_EXP(1729.142857142857, 4, "1.7291e+03");
    EXPECT_EXP(1729.142857142857, 3, "1.729e+03");
    EXPECT_EXP(1729.142857142857, 2, "1.73e+03");
    EXPECT_EXP(1729.142857142857, 1, "1.7e+03");
    EXPECT_EXP(1729.142857142857, 0, "2e+03");
  }

  SECTION("Carrying") {
    EXPECT_EXP(2.0009, 4, "2.0009e+00");
    EXPECT_EXP(2.0009, 3, "2.001e+00");
    EXPECT_EXP(2.0029, 4, "2.0029e+00");
    EXPECT_EXP(2.0029, 3, "2.003e+00");
    EXPECT_EXP(2.0099, 4, "2.0099e+00");
    EXPECT_EXP(2.0099, 3, "2.010e+00");
    EXPECT_EXP(2.0299, 4, "2.0299e+00");
    EXPECT_EXP(2.0299, 3, "2.030e+00");
    EXPECT_EXP(2.0999, 4, "2.0999e+00");
    EXPECT_EXP(2.0999, 3, "2.100e+00");
    EXPECT_EXP(2.2999, 4, "2.2999e+00");
    EXPECT_EXP(2.2999, 3, "2.300e+00");
    EXPECT_EXP(2.9999, 4, "2.9999e+00");
    EXPECT_EXP(2.9999, 3, "3.000e+00");
    EXPECT_EXP(9.9999, 4, "9.9999e+00");
    EXPECT_EXP(9.9999, 3, "1.000e+01");

    EXPECT_EXP(2.09, 2, "2.09e+00");
    EXPECT_EXP(2.09, 1, "2.1e+00");
    EXPECT_EXP(2.29, 2, "2.29e+00");
    EXPECT_EXP(2.29, 1, "2.3e+00");
    EXPECT_EXP(2.99, 2, "2.99e+00");
    EXPECT_EXP(2.99, 1, "3.0e+00");
    EXPECT_EXP(9.99, 2, "9.99e+00");
    EXPECT_EXP(9.99, 1, "1.0e+01");

    EXPECT_EXP(2.9, 1, "2.9e+00");
    EXPECT_EXP(2.9, 0, "3e+00");
    EXPECT_EXP(9.9, 1, "9.9e+00");
    EXPECT_EXP(9.9, 0, "1e+01");
  }

  SECTION("Exponents") {
    EXPECT_EXP(9.99e-100, 2, "9.99e-100");
    EXPECT_EXP(9.99e-99, 2, "9.99e-99");
    EXPECT_EXP(9.99e-10, 2, "9.99e-10");
    EXPECT_EXP(9.99e-09, 2, "9.99e-09");
    EXPECT_EXP(9.99e-01, 2, "9.99e-01");
    EXPECT_EXP(9.99e+00, 2, "9.99e+00");
    EXPECT_EXP(9.99e+01, 2, "9.99e+01");
    EXPECT_EXP(9.99e+09, 2, "9.99e+09");
    EXPECT_EXP(9.99e+10, 2, "9.99e+10");
    EXPECT_EXP(9.99e+99, 2, "9.99e+99");
    EXPECT_EXP(9.99e+100, 2, "9.99e+100");

    EXPECT_EXP(9.99e-100, 1, "1.0e-99");
    EXPECT_EXP(9.99e-99, 1, "1.0e-98");
    EXPECT_EXP(9.99e-10, 1, "1.0e-09");
    EXPECT_EXP(9.99e-09, 1, "1.0e-08");
    EXPECT_EXP(9.99e-01, 1, "1.0e+00");
    EXPECT_EXP(9.99e+00, 1, "1.0e+01");
    EXPECT_EXP(9.99e+01, 1, "1.0e+02");
    EXPECT_EXP(9.99e+09, 1, "1.0e+10");
    EXPECT_EXP(9.99e+10, 1, "1.0e+11");
    EXPECT_EXP(9.99e+99, 1, "1.0e+100");
    EXPECT_EXP(9.99e+100, 1, "1.0e+101");
  }

  SECTION("PrintDecimalPoint") {
    // These values exercise each codepath.
    EXPECT_EXP(1e+54, 0, "1e+54");
    EXPECT_EXP(1e+54, 1, "1.0e+54");
    EXPECT_EXP(1e-63, 0, "1e-63");
    EXPECT_EXP(1e-63, 1, "1.0e-63");
    EXPECT_EXP(1e+83, 0, "1e+83");
    EXPECT_EXP(1e+83, 1, "1.0e+83");
  }
}