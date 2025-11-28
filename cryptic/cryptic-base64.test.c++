// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

namespace cryptic::base64_test {

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticBase64::ToCharacterSet") = [] {
        require_eq('A', cryptic::base64::to_character_set(std::byte{0}));
        require_eq('B', cryptic::base64::to_character_set(std::byte{1}));
        require_eq('a', cryptic::base64::to_character_set(std::byte{26}));
        require_eq('b', cryptic::base64::to_character_set(std::byte{27}));
        require_eq('0', cryptic::base64::to_character_set(std::byte{52}));
        require_eq('+', cryptic::base64::to_character_set(std::byte{62}));
        require_eq('/', cryptic::base64::to_character_set(std::byte{63}));
        require_eq('=', cryptic::base64::to_character_set(std::byte{64}));
    };

    test_case("CrypticBase64::Encode") = [] {
        require_eq(""s, cryptic::base64::encode(""s));
        require_eq("TQ=="s, cryptic::base64::encode("M"s));
        require_eq("TWE="s, cryptic::base64::encode("Ma"s));
        require_eq("TWFu"s, cryptic::base64::encode("Man"s));

        require_eq("cGxlYXN1cmUu"s, cryptic::base64::encode("pleasure."s));
        require_eq("bGVhc3VyZS4="s, cryptic::base64::encode("leasure."s));
        require_eq("ZWFzdXJlLg=="s, cryptic::base64::encode("easure."s));
        require_eq("YXN1cmUu"s, cryptic::base64::encode("asure."s));
        require_eq("c3VyZS4="s, cryptic::base64::encode("sure."s));
    };

    test_case("CrypticBase64::ToIndex") = [] {
        require_eq(0, cryptic::base64::to_index('A'));
        require_eq(1, cryptic::base64::to_index('B'));
        require_eq(26, cryptic::base64::to_index('a'));
        require_eq(27, cryptic::base64::to_index('b'));
        require_eq(52, cryptic::base64::to_index('0'));
        require_eq(62, cryptic::base64::to_index('+'));
        require_eq(63, cryptic::base64::to_index('/'));
        require_eq(64, cryptic::base64::to_index('='));
    };

    test_case("CrypticBase64::Decode") = [] {
        require_eq(""s, cryptic::base64::decode(""));
        require_eq("M"s, cryptic::base64::decode("TQ=="));
        require_eq("Ma"s, cryptic::base64::decode("TWE="));
        require_eq("Man"s, cryptic::base64::decode("TWFu"));

        require_eq("pleasure."s, cryptic::base64::decode("cGxlYXN1cmUu"));
        require_eq("leasure."s, cryptic::base64::decode("bGVhc3VyZS4="));
        require_eq("easure."s, cryptic::base64::decode("ZWFzdXJlLg=="));
        require_eq("asure."s, cryptic::base64::decode("YXN1cmUu"));
        require_eq("sure."s, cryptic::base64::decode("c3VyZS4="));
    };

    return 0;
}

const auto _ = register_tests();

} // namespace cryptic::base64_test
