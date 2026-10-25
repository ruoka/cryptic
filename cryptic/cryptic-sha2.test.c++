// Copyright (c) 2025-2026 Kaius Ruokonen. All rights reserved.
// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

namespace cryptic::sha2_test {

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

    test_case("CrypticSHA224::Base64, [cryptic]") = [] {
        require_eq(cryptic::sha224::base64(""s), "0UoCjCo6K8lHYQK7KII0xBWisB+CjqYqxbPkLw=="s);
        require_eq(cryptic::sha224::base64("The quick brown fox jumps over the lazy dog"s), "cw4Qm9eooyscudmgmqIyXSQwWH3bwMOLrZEVJQ=="s);
        require_eq(cryptic::sha224::base64("The quick brown fox jumps over the lazy cog"s), "/udV9EpV8g+zNizcPEk2FbPLV07ZXOYQ7lsemw=="s);
    };

    test_case("CrypticSHA224::Hexadecimal, [cryptic]") = [] {
        require_eq(cryptic::sha224::hexadecimal(""s), "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f"s);
        require_eq(cryptic::sha224::hexadecimal("The quick brown fox jumps over the lazy dog"s), "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525"s);
        require_eq(cryptic::sha224::hexadecimal("The quick brown fox jumps over the lazy cog"s), "fee755f44a55f20fb3362cdc3c493615b3cb574ed95ce610ee5b1e9b"s);
    };

    test_case("CrypticSHA256::Base64, [cryptic]") = [] {
        require_eq(cryptic::sha256::base64(""s), "47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU="s);
        require_eq(cryptic::sha256::base64("The quick brown fox jumps over the lazy dog"s), "16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI="s);
        require_eq(cryptic::sha256::base64("The quick brown fox jumps over the lazy cog"s), "5MTY8792tpLeeRoXPgUyEVD3o0W0ZIT+Qn9qzH7Mgb4="s);
    };

    test_case("CrypticSHA256::Hexadecimal, [cryptic]") = [] {
        require_eq(cryptic::sha256::hexadecimal(""s), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"s);

        const auto dog = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha256::hexadecimal(std::as_bytes(std::span{dog})), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"s);

        const auto* cog = "The quick brown fox jumps over the lazy cog";
        require_eq(cryptic::sha256::hexadecimal(std::as_bytes(std::span{cog, 43})), "e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be"s);
    };

    test_case("CrypticSHA256::StandardVectors, [cryptic]") = [] {
        // Additional standard test vectors
        require_eq(cryptic::sha256::hexadecimal("a"), "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb"s);
        require_eq(cryptic::sha256::hexadecimal("abc"), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);
        require_eq(cryptic::sha256::hexadecimal("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"s);
    };

    test_case("CrypticSHA224::StandardVectors, [cryptic]") = [] {
        // Additional standard test vectors
        require_eq(cryptic::sha224::hexadecimal("a"), "abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5"s);
        require_eq(cryptic::sha224::hexadecimal("abc"), "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7"s);
    };

    test_case("CrypticSHA2::Hash, [cryptic]") = [] {
        const auto sha256 = cryptic::sha256::hash("abc");
        require_eq(32uz, sha256.size());
        require_eq(0xba, std::to_integer<int>(sha256.bytes().front()));
        require_eq(0xad, std::to_integer<int>(sha256.bytes().back()));
        require_eq(sha256.base64(), "ungWv48Bz+pBQUDeXa4iI7ADYaOWF3qctBD/YfIAFa0="s);

        const auto sha224 = cryptic::sha224::hash("abc");
        require_eq(28uz, sha224.size());
        require_eq(0x23, std::to_integer<int>(sha224.bytes().front()));
        require_eq(0xa7, std::to_integer<int>(sha224.bytes().back()));
        require_eq(sha224.base64(), "Iwl9IjQF2CKGQqR3vaJVsyqtvOS9oLP342ydpw=="s);
    };

    test_case("CrypticSHA256::InputTypes, [cryptic]") = [] {
        const auto abc = cryptic::sha256::hash(std::string_view{"abc"});

        const char* cstr = "abc";
        require_eq(abc, cryptic::sha256::hash(cstr));
        require_eq(abc, cryptic::sha256::hash("abc"));
        require_eq(abc, cryptic::sha256::hash("abc"s));
        require_eq(abc, cryptic::sha256::hash(u8"abc"));

        const unsigned char octets[] = {'a', 'b', 'c'};
        require_eq(abc, cryptic::sha256::hash(octets));
        const char unterminated[] = {'a', 'b', 'c'};
        require_eq(abc, cryptic::sha256::hash(unterminated));
        require_eq(cryptic::sha224::hash("abc"s), cryptic::sha224::hash("abc\0def"));
    };

    test_case("CrypticSHA256::LongMessage, [cryptic]") = [] {
        // NIST FIPS 180-4: one million 'a's
        std::string million(1'000'000, 'a');
        require_eq(cryptic::sha256::hexadecimal(million), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"s);
    };

    test_case("CrypticSHA2::Update, [cryptic]") = [] {
        auto message = std::string(300, '\0');
        for (auto i = 0uz; i < message.size(); ++i)
            message[i] = static_cast<char>(i * 7 + 3);

        auto check = [&]<typename Hash>(std::type_identity<Hash>) {
            for (auto n = 0uz; n <= message.size(); ++n)
            {
                const auto whole = std::string_view{message}.substr(0, n);
                const auto expected = Hash::hash(whole);

                for (const auto split : {0uz, 1uz, 55uz, 56uz, 63uz, 64uz, 65uz, 128uz, n / 2, n})
                {
                    if (split > n)
                        continue;
                    auto hasher = Hash{};
                    hasher.update(whole.substr(0, split)).update(whole.substr(split));
                    require_eq(expected, hasher.digest());
                }

                auto bytewise = Hash{};
                for (auto i = 0uz; i < n; ++i)
                    bytewise.update(whole.substr(i, 1));
                require_eq(expected, bytewise.digest());
            }
        };
        check(std::type_identity<cryptic::sha224>{});
        check(std::type_identity<cryptic::sha256>{});

        // NIST FIPS 180-4: one million 'a's, in pieces that don't line up with blocks
        auto million = cryptic::sha256{};
        const auto piece = std::string(997, 'a');
        auto fed = 0uz;
        for (; fed + piece.size() <= 1'000'000; fed += piece.size())
            million.update(piece);
        million.update(std::string(1'000'000 - fed, 'a'));
        require_eq(million.digest().hexadecimal(), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"s);
    };

    test_case("CrypticSHA2::Digest, [cryptic]") = [] {
        auto check = []<typename Hash>(std::type_identity<Hash>) {
            auto hasher = Hash{};
            require_eq(hasher.digest(), Hash::hash(""));

            // digest() leaves the hasher as it was, so the message can go on.
            hasher.update("ab");
            const auto ab = hasher.digest();
            require_eq(ab, hasher.digest());
            require_eq(ab, Hash::hash("ab"));
            hasher.update("c");
            require_eq(hasher.digest(), Hash::hash("abc"));

            auto copy = hasher;
            copy.update("d");
            hasher.update("e");
            require_eq(copy.digest(), Hash::hash("abcd"));
            require_eq(hasher.digest(), Hash::hash("abce"));

            hasher = {};
            require_eq(hasher.digest(), Hash::hash(""));
        };
        check(std::type_identity<cryptic::sha224>{});
        check(std::type_identity<cryptic::sha256>{});
    };

    test_case("CrypticSHA2::MessageIsNotCopied, [cryptic]") = [] {
        auto copies = 0;
        const auto message = counted_message{"abc", copies};
        require_eq(cryptic::sha256::hexadecimal(message), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);
        require_eq(cryptic::sha256::base64(message), "ungWv48Bz+pBQUDeXa4iI7ADYaOWF3qctBD/YfIAFa0="s);
        require_eq(cryptic::sha224::base64(message), "Iwl9IjQF2CKGQqR3vaJVsyqtvOS9oLP342ydpw=="s);
        require_eq(cryptic::sha256::hash(message), cryptic::sha256::hash("abc"));
        auto hasher = cryptic::sha256{};
        hasher.update(message);
        require_eq(0, copies);
    };

    test_case("CrypticSHA2::DigestsAsKeys, [cryptic]") = [] {
        auto check = []<typename Hash>(std::type_identity<Hash>) {
            const auto abc = Hash::hash("abc");
            const auto same = Hash::hash("abc"s);
            const auto other = Hash::hash("abd");

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
        check(std::type_identity<cryptic::sha224>{});
        check(std::type_identity<cryptic::sha256>{});
    };

    return 0;
}

} // namespace cryptic::sha2_test

consteval bool sha2_hashed_at_compile_time()
{
    constexpr auto abc = std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    if (cryptic::sha256::hexadecimal(abc) != "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
        or cryptic::sha224::hexadecimal(abc) != "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7")
        return false;

    auto longer = std::array<std::byte, 70>{};
    for (auto i = 0uz; i < longer.size(); ++i)
        longer[i] = std::byte{static_cast<unsigned char>(i)};
    if (cryptic::sha256::hexadecimal(longer) != "5767d69a906d4860db9079eb7e90ab4a543e5cb032fce846554aef6ceb600e1d")
        return false;

    auto message = std::array<std::byte, 130>{};
    for (auto i = 0uz; i < message.size(); ++i)
        message[i] = std::byte{static_cast<unsigned char>(i)};
    const auto whole256 = cryptic::sha256::hash(message);
    const auto whole224 = cryptic::sha224::hash(message);
    for (const auto split : {0uz, 1uz, 63uz, 64uz, 65uz, 130uz})
    {
        auto sha256 = cryptic::sha256{};
        sha256.update(std::span{message}.first(split)).update(std::span{message}.subspan(split));
        auto sha224 = cryptic::sha224{};
        sha224.update(std::span{message}.first(split)).update(std::span{message}.subspan(split));
        if (sha256.digest() != whole256 or sha224.digest() != whole224)
            return false;
    }
    return true;
}

static_assert(sha2_hashed_at_compile_time());

constexpr auto empty_sha256 = cryptic::sha256::hash(std::span<const std::byte>{});
static_assert(empty_sha256.bytes().front() == std::byte{0xe3} and empty_sha256.bytes().back() == std::byte{0x55});

namespace {

template<typename Lhs, typename Rhs>
concept has_equality = requires(const Lhs& lhs, const Rhs& rhs) { lhs == rhs; };

template<typename Lhs, typename Rhs>
concept has_less = requires(const Lhs& lhs, const Rhs& rhs) { lhs < rhs; };

template<typename Message>
concept sha256_accepts = requires(const Message& message) { cryptic::sha256::hash(message); };

} // namespace

static_assert(cryptic::sha256::digest_size == 32 and cryptic::sha256::block_size == 64);
static_assert(cryptic::sha224::digest_size == 28 and cryptic::sha224::block_size == 64);
static_assert(std::same_as<cryptic::sha256::digest_type, cryptic::digest<32>>);
static_assert(std::same_as<cryptic::sha224::digest_type, cryptic::digest<28>>);
static_assert(not has_equality<cryptic::sha224::digest_type, cryptic::sha256::digest_type>);

// A hasher is not a digest: it has no message constructor and no comparisons.
static_assert(not std::constructible_from<cryptic::sha256, std::string> and not std::convertible_to<const char*, cryptic::sha256>);
static_assert(not std::constructible_from<cryptic::sha224, std::string> and not std::convertible_to<const char*, cryptic::sha224>);
static_assert(not has_equality<cryptic::sha256, cryptic::sha256> and not has_less<cryptic::sha256, cryptic::sha256>);
static_assert(std::is_nothrow_default_constructible_v<cryptic::sha256> and std::is_trivially_copyable_v<cryptic::sha256>);

// Only byte-like elements, so a digest doesn't depend on the platform.
static_assert(sha256_accepts<std::string> and sha256_accepts<const char*> and sha256_accepts<std::u8string_view>
              and sha256_accepts<std::span<const std::byte>> and sha256_accepts<std::array<unsigned char, 4>>
              and sha256_accepts<cryptic::sha256::digest_type>);
static_assert(not sha256_accepts<std::vector<int>> and not sha256_accepts<std::wstring> and not sha256_accepts<std::u16string>
              and not sha256_accepts<std::list<char>> and not sha256_accepts<const unsigned char*> and not sha256_accepts<int>);
static_assert(not sha256_accepts<volatile char*> and not sha256_accepts<volatile char[4]> and not sha256_accepts<std::span<volatile char>>);

const auto _ = cryptic::sha2_test::register_tests();
