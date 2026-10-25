// Copyright (c) 2025-2026 Kaius Ruokonen. All rights reserved.
// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

namespace cryptic::sha1_test {

struct counted_message
{
    std::string text;
    int* copies;
    counted_message(std::string_view t, int& counter) : text{t}, copies{&counter} {}
    counted_message(const counted_message& other) : text{other.text}, copies{other.copies} { ++*copies; }
    auto begin() const { return text.begin(); }
    auto end() const { return text.end(); }
    auto data() const { return text.data(); }
    auto size() const { return text.size(); }
};

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticSHA1::Size, [cryptic]") = [] {
        auto hash = cryptic::sha1{};
        require_eq(20ul, hash.size());
    };

    test_case("CrypticSHA1::Base64, [cryptic]") = [] {
        // Standard test vectors
        require_eq(cryptic::sha1::base64(""), "2jmj7l5rSw0yVb/vlWAYkK/YBwk="s);
        require_eq(cryptic::sha1::base64("a"), "hvfkN/qlp/zhXR3cuerq6jd2Z7g="s);
        require_eq(cryptic::sha1::base64("abc"), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
        require_eq(cryptic::sha1::base64("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), "hJg+RBw70m66rkqh+VEp5eVGcPE="s);
        require_eq(cryptic::sha1::base64("The quick brown fox jumps over the lazy dog"), "L9ThxnotKPzthJ7hu3bnORuT6xI="s);
        require_eq(cryptic::sha1::base64("The quick brown fox jumps over the lazy cog"), "3p8sf9JeGzr60+haC9F9mxANtLM="s);
    };

    test_case("CrypticSHA1::Hexadecimal, [cryptic]") = [] {
        // Standard test vectors
        require_eq(cryptic::sha1::hexadecimal(""), "da39a3ee5e6b4b0d3255bfef95601890afd80709"s);
        require_eq(cryptic::sha1::hexadecimal("a"), "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"s);
        require_eq(cryptic::sha1::hexadecimal("abc"), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
        require_eq(cryptic::sha1::hexadecimal("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), "84983e441c3bd26ebaae4aa1f95129e5e54670f1"s);
        require_eq(cryptic::sha1::hexadecimal("The quick brown fox jumps over the lazy dog"), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"s);
        require_eq(cryptic::sha1::hexadecimal("The quick brown fox jumps over the lazy cog"), "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3"s);
    };

    test_case("CrypticSHA1::Reset, [cryptic]") = [] {
        auto sha1 = cryptic::sha1{};
        sha1.hash(""s);

        require_eq(sha1.base64(), "2jmj7l5rSw0yVb/vlWAYkK/YBwk="s);
        require_eq(sha1.hexadecimal(), "da39a3ee5e6b4b0d3255bfef95601890afd80709"s);

        sha1.hash("abc"s);

        require_eq(sha1.base64(), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
        require_eq(sha1.hexadecimal(), "a9993e364706816aba3e25717850c26c9cd0d89d"s);

        sha1.hash("The quick brown fox jumps over the lazy dog"s);

        require_eq(sha1.base64(), "L9ThxnotKPzthJ7hu3bnORuT6xI="s);
        require_eq(sha1.hexadecimal(), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"s);

        // Hash same message twice - should produce same result
        sha1.hash("The quick brown fox jumps over the lazy dog"s);

        require_eq(sha1.base64(), "L9ThxnotKPzthJ7hu3bnORuT6xI="s);
        require_eq(sha1.hexadecimal(), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"s);
    };

    test_case("CrypticSHA1::Hash, [cryptic]") = [] {
        auto test1 = cryptic::sha1{}, test2 = cryptic::sha1{};
        test1.hash("The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog."s);
        require_neq(test1.base64(), test2.base64());
        require_neq(test1.hexadecimal(), test2.hexadecimal());
    };

    test_case("CrypticSHA1::InputTypes, [cryptic]") = [] {
        // Test const char*
        const char* cstr = "abc";
        require_eq(cryptic::sha1::hexadecimal(cstr), "a9993e364706816aba3e25717850c26c9cd0d89d"s);

        // Test string_view
        std::string_view sv = "abc";
        require_eq(cryptic::sha1::hexadecimal(sv), "a9993e364706816aba3e25717850c26c9cd0d89d"s);

        // Test std::string
        std::string str = "abc";
        require_eq(cryptic::sha1::hexadecimal(str), "a9993e364706816aba3e25717850c26c9cd0d89d"s);

        // Test byte span
        auto test_str = "abc"s;
        auto bytes = std::as_bytes(std::span{test_str});
        require_eq(cryptic::sha1::hexadecimal(bytes), "a9993e364706816aba3e25717850c26c9cd0d89d"s);

        // Test byte array
        const std::byte array[] = {std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
        require_eq(cryptic::sha1::hexadecimal(array), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
    };

    test_case("CrypticSHA1::LongMessage, [cryptic]") = [] {
        // NIST FIPS 180-4: one million 'a's
        std::string million(1'000'000, 'a');
        require_eq(cryptic::sha1::hexadecimal(million), "34aa973cd4c4daa4f61eeb2bdbad27316534016f"s);
    };

    test_case("CrypticSHA1::ScalarMatchesNative, [cryptic]") = [] {
        auto fill = [](std::size_t n) {
            auto bytes = std::vector<std::byte>(n);
            auto state = 0xA5A5A5A5u;
            for (auto& byte : bytes)
            {
                state = state * 1664525u + 1013904223u;
                byte = std::byte{static_cast<unsigned char>(state >> 24)};
            }
            return bytes;
        };

        auto check = [](std::span<const std::byte> message) {
            const auto scalar = cryptic::details::sha1_paths::hexadecimal<true>(message);
            const auto native = cryptic::details::sha1_paths::hexadecimal<false>(message);
            require_eq(scalar, native);
            require_eq(cryptic::sha1::hexadecimal(message), native);
        };

        for (auto n = 0uz; n <= 300uz; ++n)
            check(fill(n));
        check(fill(1024));
        check(fill(65536));
    };

    test_case("CrypticSHA1::Comparison, [cryptic]") = [] {
        auto hash1 = cryptic::sha1{"abc"s};
        auto hash2 = cryptic::sha1{"def"s};
        auto hash3 = cryptic::sha1{"abc"s};

        require_neq(hash1.base64(), hash2.base64());
        require_eq(hash1.base64(), hash3.base64());
        require_neq(hash1 < hash2, hash2 < hash1); // One must be true
    };

    test_case("CrypticSHA1::EqualDigests, [cryptic]") = [] {
        const auto a = cryptic::sha1{"abc"s};
        const auto b = cryptic::sha1{"abc"s};
        require_false(a < b);
        require_false(b < a);

        auto digests = std::set<cryptic::sha1>{};
        require_true(digests.insert(a).second);
        require_false(digests.insert(b).second);
        require_eq(1uz, digests.size());
    };

    test_case("CrypticSHA1::CopyAndAssign, [cryptic]") = [] {
        auto original = cryptic::sha1{"abc"s};
        auto copy = original;
        require_eq(copy.hexadecimal(), "a9993e364706816aba3e25717850c26c9cd0d89d"s);

        copy = cryptic::sha1{"The quick brown fox jumps over the lazy dog"s};
        require_eq(copy.hexadecimal(), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"s);

        copy = original;
        require_eq(copy.hexadecimal(), original.hexadecimal());
    };

    test_case("CrypticSHA1::MessageIsNotCopied, [cryptic]") = [] {
        auto copies = 0;
        const auto message = counted_message{"abc", copies};
        require_eq(cryptic::sha1::hexadecimal(message), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
        require_eq(cryptic::sha1::base64(message), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
        auto hash = cryptic::sha1{message};
        hash.hash(message);
        hash.update(message);
        require_eq(0, copies);
    };

    test_case("CrypticSHA1::Update, [cryptic]") = [] {
        auto message = std::string(300, '\0');
        for (auto i = 0uz; i < message.size(); ++i)
            message[i] = static_cast<char>(i * 7 + 3);

        for (auto n = 0uz; n <= message.size(); ++n)
        {
            const auto whole = std::string_view{message}.substr(0, n);
            const auto expected = cryptic::sha1{whole}.digest();

            for (const auto split : {0uz, 1uz, 55uz, 56uz, 63uz, 64uz, 65uz, 128uz, n / 2, n})
            {
                if (split > n)
                    continue;
                auto hash = cryptic::sha1{};
                hash.update(whole.substr(0, split)).update(whole.substr(split));
                require_eq(expected, hash.finalize());
            }

            auto bytewise = cryptic::sha1{};
            for (auto i = 0uz; i < n; ++i)
                bytewise.update(whole.substr(i, 1));
            require_eq(expected, bytewise.finalize());
        }

        // NIST FIPS 180-4: one million 'a's, in pieces that don't line up with blocks
        auto million = cryptic::sha1{};
        const auto piece = std::string(997, 'a');
        auto fed = 0uz;
        for (; fed + piece.size() <= 1'000'000; fed += piece.size())
            million.update(piece);
        million.update(std::string(1'000'000 - fed, 'a'));
        million.finalize();
        require_eq(million.hexadecimal(), "34aa973cd4c4daa4f61eeb2bdbad27316534016f"s);
    };

    test_case("CrypticSHA1::Finalize, [cryptic]") = [] {
        auto hash = cryptic::sha1{};
        hash.finalize();
        require_eq(hash.hexadecimal(), "da39a3ee5e6b4b0d3255bfef95601890afd80709"s);

        hash.update("abc");
        const auto abc = hash.finalize();
        require_eq(hash.hexadecimal(), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
        require_eq(abc, hash.finalize());
        require_eq(abc, hash.digest());

        // update() after finalize() or hash() starts a new message
        hash.update("a").update("bc");
        require_eq(abc, hash.finalize());
        hash.hash("The quick brown fox jumps over the lazy dog"s);
        hash.update("abc");
        require_eq(abc, hash.finalize());

        // hash() discards bytes from an unfinished update()
        hash.update("discarded");
        hash.hash("abc");
        require_eq(abc, hash.digest());
    };

    test_case("CrypticSHA1::Digest, [cryptic]") = [] {
        static_assert(cryptic::sha1::digest_size == 20 and cryptic::sha1::block_size == 64);
        static_assert(std::tuple_size_v<cryptic::sha1::buffer_type> == cryptic::sha1::digest_size);

        const auto hash = cryptic::sha1{"abc"s};
        const auto digest = hash.digest();
        require_eq(0xa9, std::to_integer<int>(digest.front()));
        require_eq(0x9d, std::to_integer<int>(digest.back()));
        require_eq(hash.base64(), cryptic::base64::encode(digest));

        auto buffer = cryptic::sha1::buffer_type{};
        hash.encode(buffer);
        require_eq(digest, buffer);

        using digest_view = std::span<const std::byte, cryptic::sha1::digest_size>;
        auto larger = digest, smaller = digest;
        larger.back() = std::byte{0x9e};
        smaller.front() = std::byte{0xa8};
        require_false(hash < digest_view{digest});
        require_true(hash < digest_view{larger});
        require_false(hash < digest_view{smaller});
    };

    test_case("CrypticSHA1::EqualityAndOrdering, [cryptic]") = [] {
        const auto abc = cryptic::sha1{"abc"s};
        const auto same = cryptic::sha1{"abc"s};
        const auto other = cryptic::sha1{"abd"s};

        require_true(abc == same);
        require_false(abc != same);
        require_true(abc != other);
        require_true((abc <=> same) == std::strong_ordering::equal);
        require_eq(abc < other, (abc <=> other) < 0);
        require_eq(abc > other, other < abc);

        const auto abc_digest = abc.digest(), other_digest = other.digest();
        require_true((abc <=> other) == std::lexicographical_compare_three_way(abc_digest.begin(), abc_digest.end(),
                                                                               other_digest.begin(), other_digest.end()));

        auto digests = std::unordered_set<cryptic::sha1>{};
        require_true(digests.insert(abc).second);
        require_false(digests.insert(same).second);
        require_true(digests.insert(other).second);
        require_eq(std::hash<cryptic::sha1>{}(abc), std::hash<cryptic::sha1>{}(same));
    };

    return 0;
}

} // namespace cryptic::sha1_test

consteval bool sha1_hashed_at_compile_time()
{
    constexpr auto abc = std::array{
        std::byte{'a'}, std::byte{'b'}, std::byte{'c'}
    };
    auto hash = cryptic::sha1{};
    hash.hash(abc);
    if (hash.hexadecimal() != "a9993e364706816aba3e25717850c26c9cd0d89d")
        return false;

    auto longer = std::array<std::byte, 70>{};
    for (auto i = 0uz; i < longer.size(); ++i)
        longer[i] = std::byte{static_cast<unsigned char>(i)};
    auto wide = cryptic::sha1{};
    wide.hash(longer);
    return wide.hexadecimal() == "c24887924f92adac5ae367995d12691c662b7362";
}

static_assert(sha1_hashed_at_compile_time());

consteval bool sha1_equal_digests_are_not_less()
{
    constexpr auto abc = std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    const auto a = cryptic::sha1{abc};
    const auto b = cryptic::sha1{abc};
    return not (a < b) and not (b < a);
}

static_assert(sha1_equal_digests_are_not_less());
static_assert(std::is_copy_assignable_v<cryptic::sha1> and std::is_move_assignable_v<cryptic::sha1>);

consteval bool sha1_update_matches_hash()
{
    auto message = std::array<std::byte, 130>{};
    for (auto i = 0uz; i < message.size(); ++i)
        message[i] = std::byte{static_cast<unsigned char>(i)};

    const auto whole = cryptic::sha1{message};
    for (const auto split : {0uz, 1uz, 63uz, 64uz, 65uz, 130uz})
    {
        auto hash = cryptic::sha1{};
        hash.update(std::span{message}.first(split)).update(std::span{message}.subspan(split));
        if (hash.finalize() != whole.digest() or hash != whole)
            return false;
    }

    constexpr auto abc = std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    const auto hash = cryptic::sha1{abc};
    const auto digest = hash.digest();
    return digest[0] == std::byte{0xa9}
       and hash.base64() == "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="
       and not (hash < std::span<const std::byte, 20>{digest})
       and (hash <=> whole) != std::strong_ordering::equal;
}

static_assert(sha1_update_matches_hash());

namespace {

template<typename Lhs, typename Rhs>
concept has_equality = requires(const Lhs& lhs, const Rhs& rhs) { lhs == rhs; };

template<typename Lhs, typename Rhs>
concept has_three_way = requires(const Lhs& lhs, const Rhs& rhs) { lhs <=> rhs; };

template<typename Message>
concept sha1_accepts = requires(const Message& message) { cryptic::sha1::hexadecimal(message); };

} // namespace

static_assert(std::totally_ordered<cryptic::sha1> and std::three_way_comparable<cryptic::sha1, std::strong_ordering>);
static_assert(not has_equality<cryptic::sha1, std::string> and not has_equality<std::string, cryptic::sha1>);
static_assert(not has_three_way<cryptic::sha1, std::string> and not has_three_way<std::string, cryptic::sha1>);
static_assert(sha1_accepts<std::string> and sha1_accepts<const char*> and sha1_accepts<std::span<const std::byte>>);
static_assert(not sha1_accepts<std::list<char>> and not sha1_accepts<int>);

#if defined(__aarch64__) && defined(__ARM_FEATURE_SHA2) && !defined(CRYPTIC_SCALAR_ONLY)
static_assert(cryptic::details::sha1_paths::accelerated);
#else
static_assert(!cryptic::details::sha1_paths::accelerated);
#endif

const auto _ = cryptic::sha1_test::register_tests();
