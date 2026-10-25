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

    test_case("CrypticSHA1::Hash, [cryptic]") = [] {
        const auto digest = cryptic::sha1::hash("abc");
        require_eq(digest.hexadecimal(), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
        require_eq(digest.base64(), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
        require_eq(0xa9, std::to_integer<int>(digest.bytes().front()));
        require_eq(0x9d, std::to_integer<int>(digest.bytes().back()));
        require_eq(digest, cryptic::sha1::hash("abc"s));
        require_neq(digest, cryptic::sha1::hash("abd"s));
    };

    test_case("CrypticSHA1::InputTypes, [cryptic]") = [] {
        const auto abc = cryptic::sha1::hash(std::string_view{"abc"});

        const char* cstr = "abc";
        require_eq(abc, cryptic::sha1::hash(cstr));
        require_eq(abc, cryptic::sha1::hash("abc"));
        require_eq(abc, cryptic::sha1::hash("abc"s));
        require_eq(abc, cryptic::sha1::hash(u8"abc"));
        const char8_t* u8cstr = u8"abc";
        require_eq(abc, cryptic::sha1::hash(u8cstr));
        require_eq(abc, cryptic::sha1::hash(std::u8string{u8"abc"}));

        const auto text = "abc"s;
        require_eq(abc, cryptic::sha1::hash(std::as_bytes(std::span{text})));
        const std::byte bytes[] = {std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
        require_eq(abc, cryptic::sha1::hash(bytes));
        require_eq(abc, cryptic::sha1::hash(std::vector(std::begin(bytes), std::end(bytes))));
        const unsigned char octets[] = {'a', 'b', 'c'};
        require_eq(abc, cryptic::sha1::hash(octets));
        require_eq(abc, cryptic::sha1::hash(std::array{'a', 'b', 'c'}));

        // A char array is a C string: read up to its first NUL and never past its end.
        const char unterminated[] = {'a', 'b', 'c'};
        require_eq(abc, cryptic::sha1::hash(unterminated));
        const char embedded[] = "abc\0def";
        require_eq(abc, cryptic::sha1::hash(embedded));

        // Any other range is hashed whole, NULs included.
        const auto with_nul = "abc\0"s;
        require_eq(cryptic::sha1::hash(with_nul), cryptic::sha1::hash(std::array{'a', 'b', 'c', '\0'}));
        require_neq(abc, cryptic::sha1::hash(with_nul));
    };

    test_case("CrypticSHA1::LongMessage, [cryptic]") = [] {
        // NIST FIPS 180-4: one million 'a's
        std::string million(1'000'000, 'a');
        require_eq(cryptic::sha1::hexadecimal(million), "34aa973cd4c4daa4f61eeb2bdbad27316534016f"s);
    };

    test_case("CrypticSHA1::Update, [cryptic]") = [] {
        auto message = std::string(300, '\0');
        for (auto i = 0uz; i < message.size(); ++i)
            message[i] = static_cast<char>(i * 7 + 3);

        for (auto n = 0uz; n <= message.size(); ++n)
        {
            const auto whole = std::string_view{message}.substr(0, n);
            const auto expected = cryptic::sha1::hash(whole);

            for (const auto split : {0uz, 1uz, 55uz, 56uz, 63uz, 64uz, 65uz, 128uz, n / 2, n})
            {
                if (split > n)
                    continue;
                auto hasher = cryptic::sha1{};
                hasher.update(whole.substr(0, split)).update(whole.substr(split));
                require_eq(expected, hasher.digest());
            }

            auto bytewise = cryptic::sha1{};
            for (auto i = 0uz; i < n; ++i)
                bytewise.update(whole.substr(i, 1));
            require_eq(expected, bytewise.digest());
        }

        // NIST FIPS 180-4: one million 'a's, in pieces that don't line up with blocks
        auto million = cryptic::sha1{};
        const auto piece = std::string(997, 'a');
        auto fed = 0uz;
        for (; fed + piece.size() <= 1'000'000; fed += piece.size())
            million.update(piece);
        million.update(std::string(1'000'000 - fed, 'a'));
        require_eq(million.digest().hexadecimal(), "34aa973cd4c4daa4f61eeb2bdbad27316534016f"s);
    };

    test_case("CrypticSHA1::Digest, [cryptic]") = [] {
        auto hasher = cryptic::sha1{};
        require_eq(hasher.digest(), cryptic::sha1::hash(""));

        // digest() leaves the hasher as it was, so the message can go on.
        hasher.update("ab");
        const auto ab = hasher.digest();
        require_eq(ab, hasher.digest());
        require_eq(ab, cryptic::sha1::hash("ab"));
        hasher.update("c");
        require_eq(hasher.digest(), cryptic::sha1::hash("abc"));

        auto copy = hasher;
        copy.update("d");
        hasher.update("e");
        require_eq(copy.digest(), cryptic::sha1::hash("abcd"));
        require_eq(hasher.digest(), cryptic::sha1::hash("abce"));

        hasher = {};
        require_eq(hasher.digest(), cryptic::sha1::hash(""));
    };

    test_case("CrypticSHA1::MessageIsNotCopied, [cryptic]") = [] {
        auto copies = 0;
        const auto message = counted_message{"abc", copies};
        require_eq(cryptic::sha1::hexadecimal(message), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
        require_eq(cryptic::sha1::base64(message), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
        require_eq(cryptic::sha1::hash(message), cryptic::sha1::hash("abc"));
        auto hasher = cryptic::sha1{};
        hasher.update(message);
        require_eq(0, copies);
    };

    test_case("CrypticSHA1::DigestsAsKeys, [cryptic]") = [] {
        const auto abc = cryptic::sha1::hash("abc");
        const auto same = cryptic::sha1::hash("abc"s);
        const auto other = cryptic::sha1::hash("abd");

        auto ordered = std::set{abc, other};
        require_false(ordered.insert(same).second);
        require_eq(2uz, ordered.size());

        auto unordered = std::unordered_set{abc, other};
        require_false(unordered.insert(same).second);
        require_eq(2uz, unordered.size());

        auto sorted = std::vector{other, abc, same};
        std::ranges::sort(sorted);
        require_true(std::ranges::is_sorted(sorted));
        require_eq(2, std::ranges::count(sorted, abc));
    };

    return 0;
}

} // namespace cryptic::sha1_test

consteval bool sha1_hashed_at_compile_time()
{
    constexpr auto abc = std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    if (cryptic::sha1::hexadecimal(abc) != "a9993e364706816aba3e25717850c26c9cd0d89d"
        or cryptic::sha1::base64(abc) != "qZk+NkcGgWq6PiVxeFDCbJzQ2J0=")
        return false;

    auto longer = std::array<std::byte, 70>{};
    for (auto i = 0uz; i < longer.size(); ++i)
        longer[i] = std::byte{static_cast<unsigned char>(i)};
    if (cryptic::sha1::hexadecimal(longer) != "c24887924f92adac5ae367995d12691c662b7362")
        return false;

    auto message = std::array<std::byte, 130>{};
    for (auto i = 0uz; i < message.size(); ++i)
        message[i] = std::byte{static_cast<unsigned char>(i)};
    const auto whole = cryptic::sha1::hash(message);
    for (const auto split : {0uz, 1uz, 63uz, 64uz, 65uz, 130uz})
    {
        auto hasher = cryptic::sha1{};
        hasher.update(std::span{message}.first(split)).update(std::span{message}.subspan(split));
        if (hasher.digest() != whole)
            return false;
    }
    return true;
}

static_assert(sha1_hashed_at_compile_time());

constexpr auto empty_sha1 = cryptic::sha1::hash(std::span<const std::byte>{});
static_assert(empty_sha1.bytes().front() == std::byte{0xda} and empty_sha1.bytes().back() == std::byte{0x09});

namespace {

template<typename Lhs, typename Rhs>
concept has_equality = requires(const Lhs& lhs, const Rhs& rhs) { lhs == rhs; };

template<typename Lhs, typename Rhs>
concept has_less = requires(const Lhs& lhs, const Rhs& rhs) { lhs < rhs; };

template<typename Message>
concept sha1_accepts = requires(const Message& message) { cryptic::sha1::hash(message); };

} // namespace

static_assert(cryptic::sha1::digest_size == 20 and cryptic::sha1::block_size == 64);
static_assert(std::same_as<cryptic::sha1::digest_type, cryptic::digest<20>>);

// A hasher is not a digest: it has no message constructor and no comparisons.
static_assert(not std::constructible_from<cryptic::sha1, std::string> and not std::convertible_to<const char*, cryptic::sha1>);
static_assert(not has_equality<cryptic::sha1, cryptic::sha1> and not has_less<cryptic::sha1, cryptic::sha1>);
static_assert(std::is_nothrow_default_constructible_v<cryptic::sha1> and std::is_trivially_copyable_v<cryptic::sha1>);

// Only byte-like elements, so a digest doesn't depend on the platform.
static_assert(sha1_accepts<std::string> and sha1_accepts<const char*> and sha1_accepts<std::u8string_view>
              and sha1_accepts<std::span<const std::byte>> and sha1_accepts<std::array<unsigned char, 4>>
              and sha1_accepts<cryptic::sha1::digest_type>);
static_assert(not sha1_accepts<std::vector<int>> and not sha1_accepts<std::wstring> and not sha1_accepts<std::u16string>
              and not sha1_accepts<std::list<char>> and not sha1_accepts<const unsigned char*> and not sha1_accepts<int>);
static_assert(not sha1_accepts<volatile char*> and not sha1_accepts<volatile char[4]> and not sha1_accepts<std::span<volatile char>>);

const auto _ = cryptic::sha1_test::register_tests();
