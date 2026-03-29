#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace header0 {

#include "src/bin-to-header/test/header.0.hpp"

} // namespace header0

namespace header1 {

#include "src/bin-to-header/test/header.1.hpp"

} // namespace header1

namespace header100 {

#include "src/bin-to-header/test/header.100.hpp"

} // namespace header100

namespace {

std::vector<std::byte> readFile(const std::filesystem::path& path)
{
    auto stream = std::ifstream{path, std::ios::ate | std::ios::binary};
    if (!stream.is_open()) {
        throw std::runtime_error{
            std::format("failed to open file: \"{}\"", path.string())};
    }
    stream.exceptions(std::ios::badbit | std::ios::failbit);

    auto fileSize = stream.tellg();
    auto buffer = std::vector<std::byte>(fileSize);
    stream.seekg(0);

    stream.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    stream.close();

    return buffer;
}

} // namespace

TEST_CASE("0 bytes round-trip", "[round-trip]")
{
    auto reference = readFile("src/bin-to-header/test/data.0");
    REQUIRE(std::ranges::equal(reference, header0::bytes));
}

TEST_CASE("1 byte round-trip", "[round-trip]")
{
    auto reference = readFile("src/bin-to-header/test/data.1");
    REQUIRE(std::ranges::equal(reference, header1::bytes));
}

TEST_CASE("100 bytes round-trip", "[round-trip]")
{
    auto reference = readFile("src/bin-to-header/test/data.100");
    REQUIRE(std::ranges::equal(reference, header100::bytes));
}
