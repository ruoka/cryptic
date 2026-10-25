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
    auto hash = cryptic::sha224{};
        require_eq(28ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha224::base64(test1), "0UoCjCo6K8lHYQK7KII0xBWisB+CjqYqxbPkLw=="s);

    auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha224::base64(test2), "cw4Qm9eooyscudmgmqIyXSQwWH3bwMOLrZEVJQ=="s);

    auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq(cryptic::sha224::base64(test3), "/udV9EpV8g+zNizcPEk2FbPLV07ZXOYQ7lsemw=="s);
    };

    test_case("CrypticSHA224::Hexadecimal, [cryptic]") = [] {
    auto hash = cryptic::sha224{};
        require_eq(28ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha224::hexadecimal(test1), "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f"s);

    auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha224::hexadecimal(test2), "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525"s);

    auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq(cryptic::sha224::hexadecimal(test3), "fee755f44a55f20fb3362cdc3c493615b3cb574ed95ce610ee5b1e9b"s);
    };

    test_case("CrypticSHA256::Base64, [cryptic]") = [] {
    auto hash = cryptic::sha256{};
        require_eq(32ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha256::base64(test1), "47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU="s);

    auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha256::base64(test2), "16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI="s);

    auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq(cryptic::sha256::base64(test3), "5MTY8792tpLeeRoXPgUyEVD3o0W0ZIT+Qn9qzH7Mgb4="s);
    };

    test_case("CrypticSHA256::Hexadecimal, [cryptic]") = [] {
    auto hash = cryptic::sha256{};
        require_eq(32ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha256::hexadecimal(test1), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"s);

    auto test21 = "The quick brown fox jumps over the lazy dog"s;
    auto test22 = std::as_bytes(std::span{test21});
        require_eq(cryptic::sha256::hexadecimal(test22), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"s);

    auto test31 = "The quick brown fox jumps over the lazy cog";
    auto test32 = std::as_bytes(std::span{test31,43});
        require_eq(cryptic::sha256::hexadecimal(test32), "e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be"s);
    };

    test_case("CrypticSHA256::Reset, [cryptic]") = [] {
    auto sha256 = cryptic::sha256{};
    sha256.hash(""s);

        require_eq(sha256.base64(), "47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU="s);
        require_eq(sha256.hexadecimal(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"s);

    sha256.hash("The quick brown fox jumps over the lazy dog"s);

        require_eq(sha256.base64(), "16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI="s);
        require_eq(sha256.hexadecimal(), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"s);

    sha256.hash("The quick brown fox jumps over the lazy dog"s);

        require_eq(sha256.base64(), "16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI="s);
        require_eq(sha256.hexadecimal(), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"s);
    };

    test_case("CrypticSHA256::Hash, [cryptic]") = [] {
    auto test1 = cryptic::sha256{}, test2 = cryptic::sha256{};
    test1.hash("The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog."s);
        require_neq(test1.base64(), test2.base64());
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

    test_case("CrypticSHA256::InputTypes, [cryptic]") = [] {
        // Test const char*
        const char* cstr = "abc";
        require_eq(cryptic::sha256::hexadecimal(cstr), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);

        // Test string_view
        std::string_view sv = "abc";
        require_eq(cryptic::sha256::hexadecimal(sv), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);

        // Test std::string
        std::string str = "abc";
        require_eq(cryptic::sha256::hexadecimal(str), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);
    };

    test_case("CrypticSHA256::LongMessage, [cryptic]") = [] {
        // NIST FIPS 180-4: one million 'a's
        std::string million(1'000'000, 'a');
        require_eq(cryptic::sha256::hexadecimal(million), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"s);
    };

    test_case("CrypticSHA2::ScalarMatchesNative, [cryptic]") = [] {
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
            const auto scalar256 = cryptic::details::sha2_paths::hexadecimal<cryptic::sha256, true>(message);
            const auto native256 = cryptic::details::sha2_paths::hexadecimal<cryptic::sha256, false>(message);
            require_eq(scalar256, native256);
            require_eq(cryptic::sha256::hexadecimal(message), native256);

            const auto scalar224 = cryptic::details::sha2_paths::hexadecimal<cryptic::sha224, true>(message);
            const auto native224 = cryptic::details::sha2_paths::hexadecimal<cryptic::sha224, false>(message);
            require_eq(scalar224, native224);
            require_eq(cryptic::sha224::hexadecimal(message), native224);
        };

        for (auto n = 0uz; n <= 300uz; ++n)
            check(fill(n));
        check(fill(1024));
        check(fill(65536));
    };

    test_case("CrypticSHA256::Comparison, [cryptic]") = [] {
        auto hash1 = cryptic::sha256{"abc"s};
        auto hash2 = cryptic::sha256{"def"s};
        auto hash3 = cryptic::sha256{"abc"s};

        require_neq(hash1.base64(), hash2.base64());
        require_eq(hash1.base64(), hash3.base64());
    };

    test_case("CrypticSHA2::EqualDigests, [cryptic]") = [] {
        const auto a = cryptic::sha256{"abc"s};
        const auto b = cryptic::sha256{"abc"s};
        require_false(a < b);
        require_false(b < a);

        auto digests = std::set<cryptic::sha224>{};
        require_true(digests.insert(cryptic::sha224{"abc"s}).second);
        require_false(digests.insert(cryptic::sha224{"abc"s}).second);
        require_eq(1uz, digests.size());
    };

    test_case("CrypticSHA2::CopyAndAssign, [cryptic]") = [] {
        auto original = cryptic::sha256{"abc"s};
        auto copy = original;
        require_eq(copy.hexadecimal(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);

        copy = cryptic::sha256{"The quick brown fox jumps over the lazy dog"s};
        require_eq(copy.hexadecimal(), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"s);

        copy = original;
        require_eq(copy.hexadecimal(), original.hexadecimal());

        auto digests = std::vector{cryptic::sha224{"c"s}, cryptic::sha224{"a"s}, cryptic::sha224{"b"s}};
        std::sort(digests.begin(), digests.end());
        require_true(std::is_sorted(digests.begin(), digests.end()));
    };

    test_case("CrypticSHA2::MessageIsNotCopied, [cryptic]") = [] {
        auto copies = 0;
        const auto message = counted_message{"abc", copies};
        require_eq(cryptic::sha256::hexadecimal(message), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);
        require_eq(cryptic::sha256::base64(message), "ungWv48Bz+pBQUDeXa4iI7ADYaOWF3qctBD/YfIAFa0="s);
        require_eq(cryptic::sha224::base64(message), "Iwl9IjQF2CKGQqR3vaJVsyqtvOS9oLP342ydpw=="s);
        auto hash = cryptic::sha256{message};
        hash.hash(message);
        hash.update(message);
        require_eq(0, copies);
    };

    test_case("CrypticSHA2::Update, [cryptic]") = [] {
        auto message = std::string(300, '\0');
        for (auto i = 0uz; i < message.size(); ++i)
            message[i] = static_cast<char>(i * 7 + 3);

        auto check = [&]<typename Hash>(std::type_identity<Hash>) {
            for (auto n = 0uz; n <= message.size(); ++n)
            {
                const auto whole = std::string_view{message}.substr(0, n);
                const auto expected = Hash{whole}.digest();

                for (const auto split : {0uz, 1uz, 55uz, 56uz, 63uz, 64uz, 65uz, 128uz, n / 2, n})
                {
                    if (split > n)
                        continue;
                    auto hash = Hash{};
                    hash.update(whole.substr(0, split)).update(whole.substr(split));
                    require_eq(expected, hash.finalize());
                }

                auto bytewise = Hash{};
                for (auto i = 0uz; i < n; ++i)
                    bytewise.update(whole.substr(i, 1));
                require_eq(expected, bytewise.finalize());
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
        million.finalize();
        require_eq(million.hexadecimal(), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"s);
    };

    test_case("CrypticSHA2::Finalize, [cryptic]") = [] {
        auto hash = cryptic::sha256{};
        hash.finalize();
        require_eq(hash.hexadecimal(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"s);

        hash.update("abc");
        const auto abc = hash.finalize();
        require_eq(hash.hexadecimal(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);
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

        auto sha224 = cryptic::sha224{};
        sha224.update("ab").update("c");
        sha224.finalize();
        require_eq(sha224.hexadecimal(), "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7"s);
    };

    test_case("CrypticSHA2::Digest, [cryptic]") = [] {
        static_assert(cryptic::sha256::digest_size == 32 and cryptic::sha256::block_size == 64);
        static_assert(cryptic::sha224::digest_size == 28 and cryptic::sha224::block_size == 64);
        static_assert(std::tuple_size_v<cryptic::sha256::buffer_type> == cryptic::sha256::digest_size);
        static_assert(std::tuple_size_v<cryptic::sha224::buffer_type> == cryptic::sha224::digest_size);

        const auto hash = cryptic::sha256{"abc"s};
        const auto digest = hash.digest();
        require_eq(0xba, std::to_integer<int>(digest.front()));
        require_eq(0xad, std::to_integer<int>(digest.back()));
        require_eq(hash.base64(), cryptic::base64::encode(digest));

        auto buffer = cryptic::sha256::buffer_type{};
        hash.encode(buffer);
        require_eq(digest, buffer);

        const auto sha224 = cryptic::sha224{"abc"s}.digest();
        require_eq(0x23, std::to_integer<int>(sha224.front()));
        require_eq(0xa7, std::to_integer<int>(sha224.back()));

        using digest_view = std::span<const std::byte, cryptic::sha256::digest_size>;
        auto larger = digest, smaller = digest;
        larger.back() = std::byte{0xae};
        smaller.front() = std::byte{0xb9};
        require_false(hash < digest_view{digest});
        require_true(hash < digest_view{larger});
        require_false(hash < digest_view{smaller});
    };

    test_case("CrypticSHA2::EqualityAndOrdering, [cryptic]") = [] {
        auto check = []<typename Hash>(std::type_identity<Hash>) {
            const auto abc = Hash{"abc"s};
            const auto same = Hash{"abc"s};
            const auto other = Hash{"abd"s};

            require_true(abc == same);
            require_false(abc != same);
            require_true(abc != other);
            require_true((abc <=> same) == std::strong_ordering::equal);
            require_eq(abc < other, (abc <=> other) < 0);
            require_eq(abc > other, other < abc);

            const auto abc_digest = abc.digest(), other_digest = other.digest();
            require_true((abc <=> other) == std::lexicographical_compare_three_way(abc_digest.begin(), abc_digest.end(),
                                                                                   other_digest.begin(), other_digest.end()));

            auto digests = std::unordered_set<Hash>{};
            require_true(digests.insert(abc).second);
            require_false(digests.insert(same).second);
            require_true(digests.insert(other).second);
            require_eq(std::hash<Hash>{}(abc), std::hash<Hash>{}(same));
        };
        check(std::type_identity<cryptic::sha224>{});
        check(std::type_identity<cryptic::sha256>{});
    };

    return 0;
}

} // namespace cryptic::sha2_test

consteval bool sha256_hashed_at_compile_time()
{
    constexpr auto abc = std::array{
        std::byte{'a'}, std::byte{'b'}, std::byte{'c'}
    };
    auto hash = cryptic::sha256{};
    hash.hash(abc);
    if (hash.hexadecimal() != "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
        return false;

    auto longer = std::array<std::byte, 70>{};
    for (auto i = 0uz; i < longer.size(); ++i)
        longer[i] = std::byte{static_cast<unsigned char>(i)};
    auto wide = cryptic::sha256{};
    wide.hash(longer);
    return wide.hexadecimal() == "5767d69a906d4860db9079eb7e90ab4a543e5cb032fce846554aef6ceb600e1d";
}

static_assert(sha256_hashed_at_compile_time());

consteval bool sha2_equal_digests_are_not_less()
{
    constexpr auto abc = std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    const auto a = cryptic::sha256{abc};
    const auto b = cryptic::sha256{abc};
    const auto c = cryptic::sha224{abc};
    const auto d = cryptic::sha224{abc};
    return not (a < b) and not (b < a) and not (c < d) and not (d < c);
}

static_assert(sha2_equal_digests_are_not_less());
static_assert(std::is_copy_assignable_v<cryptic::sha256> and std::is_move_assignable_v<cryptic::sha256>);
static_assert(std::is_copy_assignable_v<cryptic::sha224> and std::is_move_assignable_v<cryptic::sha224>);

consteval bool sha2_update_matches_hash()
{
    auto message = std::array<std::byte, 130>{};
    for (auto i = 0uz; i < message.size(); ++i)
        message[i] = std::byte{static_cast<unsigned char>(i)};

    const auto whole256 = cryptic::sha256{message};
    const auto whole224 = cryptic::sha224{message};
    for (const auto split : {0uz, 1uz, 63uz, 64uz, 65uz, 130uz})
    {
        auto hash256 = cryptic::sha256{};
        hash256.update(std::span{message}.first(split)).update(std::span{message}.subspan(split));
        auto hash224 = cryptic::sha224{};
        hash224.update(std::span{message}.first(split)).update(std::span{message}.subspan(split));
        if (hash256.finalize() != whole256.digest() or hash256 != whole256 or
            hash224.finalize() != whole224.digest() or hash224 != whole224)
            return false;
    }

    constexpr auto abc = std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
    const auto hash = cryptic::sha256{abc};
    const auto digest = hash.digest();
    return digest[0] == std::byte{0xba}
       and hash.base64() == "ungWv48Bz+pBQUDeXa4iI7ADYaOWF3qctBD/YfIAFa0="
       and not (hash < std::span<const std::byte, 32>{digest})
       and (hash <=> whole256) != std::strong_ordering::equal;
}

static_assert(sha2_update_matches_hash());

namespace {

template<typename Lhs, typename Rhs>
concept has_equality = requires(const Lhs& lhs, const Rhs& rhs) { lhs == rhs; };

template<typename Lhs, typename Rhs>
concept has_three_way = requires(const Lhs& lhs, const Rhs& rhs) { lhs <=> rhs; };

template<typename Message>
concept sha256_accepts = requires(const Message& message) { cryptic::sha256::hexadecimal(message); };

} // namespace

static_assert(std::totally_ordered<cryptic::sha256> and std::three_way_comparable<cryptic::sha256, std::strong_ordering>);
static_assert(std::totally_ordered<cryptic::sha224> and std::three_way_comparable<cryptic::sha224, std::strong_ordering>);
static_assert(not has_equality<cryptic::sha256, std::string> and not has_equality<std::string, cryptic::sha256>);
static_assert(not has_three_way<cryptic::sha256, std::string> and not has_three_way<std::string, cryptic::sha256>);
static_assert(not has_equality<cryptic::sha224, cryptic::sha256> and not has_three_way<cryptic::sha224, cryptic::sha256>);
static_assert(sha256_accepts<std::string> and sha256_accepts<const char*> and sha256_accepts<std::span<const std::byte>>);
static_assert(not sha256_accepts<std::list<char>> and not sha256_accepts<int>);

#if defined(__aarch64__) && defined(__ARM_FEATURE_SHA2) && !defined(CRYPTIC_SCALAR_ONLY)
static_assert(cryptic::details::sha2_paths::accelerated);
#else
static_assert(!cryptic::details::sha2_paths::accelerated);
#endif

const auto _ = cryptic::sha2_test::register_tests();
