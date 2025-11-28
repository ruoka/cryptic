// Copyright (c) 2017 Kaius Ruokonen
// Distributed under the MIT software license
// See LICENCE file or https://opensource.org/licenses/MIT

#include <string_view>
#include "cryptic/sha1.hpp"
import tester;

using namespace std::string_literals;

namespace cryptic::sha1_test {

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticSHA1::Base64") = [] {
        constexpr auto hash = cryptic::sha1{};
        require_eq(20ul, hash.size());

        auto test1 = ""s;
        require_eq("2jmj7l5rSw0yVb/vlWAYkK/YBwk=", cryptic::sha1::base64(test1));

        auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq("L9ThxnotKPzthJ7hu3bnORuT6xI=", cryptic::sha1::base64(test2));

        auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq("3p8sf9JeGzr60+haC9F9mxANtLM=", cryptic::sha1::base64(test3));
    };

    test_case("CrypticSHA1::Hexadecimal") = [] {
        auto hash = cryptic::sha1{};
        require_eq(20ul, hash.size());

        auto test1 = ""s;
        require_eq("da39a3ee5e6b4b0d3255bfef95601890afd80709", cryptic::sha1::hexadecimal(test1));

        auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12", cryptic::sha1::hexadecimal(test2));

        auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq("de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3", cryptic::sha1::hexadecimal(test3));
    };

    return 0;
}

const auto _ = register_tests();

} // namespace cryptic::sha1_test
