#include "AveragingBuffer.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <stdexcept>

using namespace warden;
using Catch::Approx;

TEST_CASE("AveragingBuffer — throws on zero capacity", "[averaging]") {
    REQUIRE_THROWS_AS(AveragingBuffer<float>{0}, std::invalid_argument);
}

TEST_CASE("AveragingBuffer — empty buffer returns nullopt", "[averaging]") {
    AveragingBuffer<float> buf{5};
    REQUIRE_FALSE(buf.average().has_value());
}

TEST_CASE("AveragingBuffer — single value average returns that value", "[averaging]") {
    AveragingBuffer<float> buf{5};
    buf.push(23.4F);
    REQUIRE(buf.average().value() == Approx(23.4F));
}

TEST_CASE("AveragingBuffer — average of multiple values is correct", "[averaging]") {
    AveragingBuffer<float> buf{4};
    buf.push(10.0F);
    buf.push(20.0F);
    buf.push(30.0F);
    buf.push(40.0F);
    REQUIRE(buf.average().value() == Approx(25.0F));
}

TEST_CASE("AveragingBuffer — old values are evicted once full", "[averaging]") {
    AveragingBuffer<float> buf{3};
    buf.push(10.0F);
    buf.push(10.0F);
    buf.push(10.0F);
    buf.push(40.0F);  // evicts first 10.0
    // window: [10, 10, 40] → average = 20
    REQUIRE(buf.average().value() == Approx(20.0F));
}

TEST_CASE("AveragingBuffer — size and full flags are correct", "[averaging]") {
    AveragingBuffer<float> buf{3};
    REQUIRE(buf.empty());
    REQUIRE(buf.empty());

    buf.push(1.0F);
    REQUIRE_FALSE(buf.empty());
    REQUIRE_FALSE(buf.full());
    REQUIRE(buf.size() == 1);

    buf.push(2.0F);
    buf.push(3.0F);
    REQUIRE(buf.full());
    REQUIRE(buf.size() == 3);

    buf.push(4.0F);  // buffer wraps — still full, still size 3
    REQUIRE(buf.full());
    REQUIRE(buf.size() == 3);
}

TEST_CASE("AveragingBuffer — capacity of 1 always returns last pushed value", "[averaging]") {
    AveragingBuffer<float> buf{1};
    buf.push(10.0F);
    REQUIRE(buf.average().value() == Approx(10.0F));
    buf.push(99.0F);
    REQUIRE(buf.average().value() == Approx(99.0F));
}

TEST_CASE("AveragingBuffer — works with integer types", "[averaging]") {
    AveragingBuffer<int> buf{4};
    buf.push(10);
    buf.push(20);
    buf.push(30);
    buf.push(40);
    REQUIRE(buf.average().value() == 25);
}
