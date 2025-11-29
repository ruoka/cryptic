// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

namespace cryptic::sha1_test {

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticSHA1::Size") = [] {
        auto hash = cryptic::sha1{};
        require_eq(20ul, hash.size());
    };

    test_case("CrypticSHA1::Base64") = [] {
        // Standard test vectors
        require_eq(cryptic::sha1::base64(""), "2jmj7l5rSw0yVb/vlWAYkK/YBwk="s);
        require_eq(cryptic::sha1::base64("a"), "hvfkN/qlp/zhXR3cuerq6jd2Z7g="s);
        require_eq(cryptic::sha1::base64("abc"), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
        require_eq(cryptic::sha1::base64("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), "hJg+RBw70m66rkqh+VEp5eVGcPE="s);
        require_eq(cryptic::sha1::base64("The quick brown fox jumps over the lazy dog"), "L9ThxnotKPzthJ7hu3bnORuT6xI="s);
        require_eq(cryptic::sha1::base64("The quick brown fox jumps over the lazy cog"), "3p8sf9JeGzr60+haC9F9mxANtLM="s);
    };

    test_case("CrypticSHA1::Hexadecimal") = [] {
        // Standard test vectors
        require_eq(cryptic::sha1::hexadecimal(""), "da39a3ee5e6b4b0d3255bfef95601890afd80709"s);
        require_eq(cryptic::sha1::hexadecimal("a"), "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"s);
        require_eq(cryptic::sha1::hexadecimal("abc"), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
        require_eq(cryptic::sha1::hexadecimal("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), "84983e441c3bd26ebaae4aa1f95129e5e54670f1"s);
        require_eq(cryptic::sha1::hexadecimal("The quick brown fox jumps over the lazy dog"), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"s);
        require_eq(cryptic::sha1::hexadecimal("The quick brown fox jumps over the lazy cog"), "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3"s);
    };

    test_case("CrypticSHA1::Reset") = [] {
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

    test_case("CrypticSHA1::Hash") = [] {
        auto test1 = cryptic::sha1{}, test2 = cryptic::sha1{};
        test1.hash("The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog."s);
        require_neq(test1.base64(), test2.base64());
        require_neq(test1.hexadecimal(), test2.hexadecimal());
    };

    test_case("CrypticSHA1::InputTypes") = [] {
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
    };

    test_case("CrypticSHA1::LongMessage") = [] {
        // One million 'a's - known test vector
        std::string million(1'000'000, 'a');
        require_eq(cryptic::sha1::hexadecimal(million), "a048fad61afde325fdf0cdd04e6302cf6a2344dd"s);
    };

    test_case("CrypticSHA1::Comparison") = [] {
        auto hash1 = cryptic::sha1{"abc"s};
        auto hash2 = cryptic::sha1{"def"s};
        auto hash3 = cryptic::sha1{"abc"s};

        require_neq(hash1.base64(), hash2.base64());
        require_eq(hash1.base64(), hash3.base64());
        require_neq(hash1 < hash2, hash2 < hash1); // One must be true
    };

    return 0;
}

} // namespace cryptic::sha1_test

const auto _ = cryptic::sha1_test::register_tests();
