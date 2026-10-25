// Copyright (c) 2025-2026 Kaius Ruokonen. All rights reserved.
// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

namespace cryptic::digest_test {

constexpr auto deadbeef = std::array{std::byte{0xde}, std::byte{0xad}, std::byte{0xbe}, std::byte{0xef}};

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticDigest::Construction, [cryptic]") = [] {
        const auto zero = cryptic::digest<4>{};
        require_eq(4uz, zero.size());
        require_true(std::ranges::all_of(zero, [](std::byte octet) { return octet == std::byte{0}; }));

        const auto digest = cryptic::digest<4>{deadbeef};
        require_true(std::ranges::equal(digest.bytes(), deadbeef));
        require_true(std::ranges::equal(digest, deadbeef));
        require_eq(digest.data(), digest.bytes().data());
    };

    test_case("CrypticDigest::Formatting, [cryptic]") = [] {
        const auto digest = cryptic::digest<4>{deadbeef};
        require_eq(digest.hexadecimal(), "deadbeef"s);
        require_eq(cryptic::digest<4>{}.hexadecimal(), "00000000"s);
        require_eq(digest.base64(), "3q2+7w=="s);
        require_eq(digest.base64(), cryptic::base64::encode(digest));

        auto every_byte = std::array<std::byte, 256>{};
        for (auto i = 0uz; i < every_byte.size(); ++i)
            every_byte[i] = std::byte{static_cast<unsigned char>(i)};
        const auto hexadecimal = cryptic::digest<256>{every_byte}.hexadecimal();
        require_eq(512uz, hexadecimal.size());
        require_eq(hexadecimal.substr(0, 8), "00010203"s);
        require_eq(hexadecimal.substr(504), "fcfdfeff"s);
    };

    test_case("CrypticDigest::Comparison, [cryptic]") = [] {
        const auto digest = cryptic::digest<4>{deadbeef};
        auto bytes = deadbeef;
        bytes.back() = std::byte{0xf0};
        const auto larger_last = cryptic::digest<4>{bytes};
        bytes = deadbeef;
        bytes.front() = std::byte{0x00};
        const auto smaller_first = cryptic::digest<4>{bytes};

        require_true(digest == cryptic::digest<4>{deadbeef});
        require_true(digest != larger_last);
        require_true(digest < larger_last);
        require_true(smaller_first < digest);
        require_true(smaller_first < larger_last);
        require_true((digest <=> cryptic::digest<4>{deadbeef}) == std::strong_ordering::equal);
        require_true((larger_last <=> digest) == std::strong_ordering::greater);
    };

    test_case("CrypticDigest::Hash, [cryptic]") = [] {
        require_eq(std::size_t{0xdeadbeef}, std::hash<cryptic::digest<4>>{}(cryptic::digest<4>{deadbeef}));

        // Only the leading sizeof(std::size_t) bytes count.
        auto bytes = std::array<std::byte, 32>{};
        bytes.front() = std::byte{0x01};
        bytes.back() = std::byte{0xff};
        require_eq(std::size_t{1} << (8 * (sizeof(std::size_t) - 1)), std::hash<cryptic::digest<32>>{}(cryptic::digest<32>{bytes}));
    };

    return 0;
}

} // namespace cryptic::digest_test

consteval bool digest_at_compile_time()
{
    constexpr auto digest = cryptic::digest<4>{cryptic::digest_test::deadbeef};
    return digest.hexadecimal() == "deadbeef"
       and digest.base64() == "3q2+7w=="
       and digest == cryptic::digest<4>{cryptic::digest_test::deadbeef}
       and cryptic::digest<4>{} < digest
       and std::hash<cryptic::digest<4>>{}(digest) == 0xdeadbeef;
}

static_assert(digest_at_compile_time());

static_assert(std::regular<cryptic::digest<32>> and std::totally_ordered<cryptic::digest<32>>);
static_assert(std::three_way_comparable<cryptic::digest<32>, std::strong_ordering>);
static_assert(std::is_trivially_copyable_v<cryptic::digest<32>> and sizeof(cryptic::digest<32>) == 32);
static_assert(std::ranges::contiguous_range<cryptic::digest<32>> and std::ranges::sized_range<cryptic::digest<32>>);
static_assert(std::is_nothrow_constructible_v<cryptic::digest<4>, std::span<const std::byte, 4>>);
static_assert(not std::is_convertible_v<std::span<const std::byte, 4>, cryptic::digest<4>>);
static_assert(not std::is_constructible_v<cryptic::digest<4>, std::span<const std::byte, 3>>);
static_assert(not std::equality_comparable_with<cryptic::digest<20>, cryptic::digest<32>>);

const auto _ = cryptic::digest_test::register_tests();
