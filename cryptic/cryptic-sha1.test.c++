// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;
    test_case("CrypticSHA1::Base64") = [] {
        require_eq(cryptic::sha1::base64(""),  "2jmj7l5rSw0yVb/vlWAYkK/YBwk="s);
        require_eq(cryptic::sha1::base64("a"), "hvfkN/qlp/zhXR3cuerq6jd2Z7g="s);
        require_eq(cryptic::sha1::base64("abc"), "qZk+NkcGgWq6PiVxeFDCbJzQ2J0="s);
    };

    test_case("CrypticSHA1::Hexadecimal") = [] {
        require_eq(cryptic::sha1::hexadecimal(""), "da39a3ee5e6b4b0d3255bfef95601890afd80709"s);
        require_eq(cryptic::sha1::hexadecimal("a"), "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"s);
        require_eq(cryptic::sha1::hexadecimal("abc"), "a9993e364706816aba3e25717850c26c9cd0d89d"s);
    };

    return 0;
}

const auto _ = register_tests();
