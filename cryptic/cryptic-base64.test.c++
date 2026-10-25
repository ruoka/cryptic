// Copyright (c) 2025-2026 Kaius Ruokonen. All rights reserved.
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

    test_case("CrypticBase64::ToCharacterSet, [cryptic]") = [] {
        require_eq('A', cryptic::base64::to_character_set(std::byte{0}));
        require_eq('B', cryptic::base64::to_character_set(std::byte{1}));
        require_eq('a', cryptic::base64::to_character_set(std::byte{26}));
        require_eq('b', cryptic::base64::to_character_set(std::byte{27}));
        require_eq('0', cryptic::base64::to_character_set(std::byte{52}));
        require_eq('+', cryptic::base64::to_character_set(std::byte{62}));
        require_eq('/', cryptic::base64::to_character_set(std::byte{63}));
        require_eq('=', cryptic::base64::to_character_set(std::byte{64}));
    };

    test_case("CrypticBase64::Encode, [cryptic]") = [] {
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

    test_case("CrypticBase64::ToIndex, [cryptic]") = [] {
        require_eq(0, cryptic::base64::to_index('A'));
        require_eq(1, cryptic::base64::to_index('B'));
        require_eq(26, cryptic::base64::to_index('a'));
        require_eq(27, cryptic::base64::to_index('b'));
        require_eq(52, cryptic::base64::to_index('0'));
        require_eq(62, cryptic::base64::to_index('+'));
        require_eq(63, cryptic::base64::to_index('/'));
        require_eq(64, cryptic::base64::to_index('='));
    };

    test_case("CrypticBase64::Decode, [cryptic]") = [] {
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

    test_case("CrypticBase64::EncodeInputs, [cryptic]") = [] {
        require_eq("TWFu"s, cryptic::base64::encode("Man"));
        const char* cstr = "Ma";
        require_eq("TWE="s, cryptic::base64::encode(cstr));
        require_eq("TQ=="s, cryptic::base64::encode(std::string_view{"M"}));
        require_eq("YQBi"s, cryptic::base64::encode("a\0b"s));
        require_eq("TWFu"s, cryptic::base64::encode(u8"Man"));
        const char unterminated[] = {'M', 'a', 'n'};
        require_eq("TWFu"s, cryptic::base64::encode(unterminated));

        const auto bytes = std::array{std::byte{0x00}, std::byte{0xff}, std::byte{0x10}};
        require_eq("AP8Q"s, cryptic::base64::encode(bytes));
        require_eq("AP8="s, cryptic::base64::encode(std::span{bytes}.first(2)));
        require_eq("AP8Q"s, cryptic::base64::encode(std::vector(bytes.begin(), bytes.end())));
    };

    test_case("CrypticBase64::TryDecode, [cryptic]") = [] {
        // RFC 4648, section 10
        require_eq(""s, cryptic::base64::try_decode("").value());
        require_eq("f"s, cryptic::base64::try_decode("Zg==").value());
        require_eq("fo"s, cryptic::base64::try_decode("Zm8=").value());
        require_eq("foo"s, cryptic::base64::try_decode("Zm9v").value());
        require_eq("foob"s, cryptic::base64::try_decode("Zm9vYg==").value());
        require_eq("fooba"s, cryptic::base64::try_decode("Zm9vYmE=").value());
        require_eq("foobar"s, cryptic::base64::try_decode("Zm9vYmFy").value());

        auto every_byte = std::string(256, '\0');
        for (auto i = 0uz; i < every_byte.size(); ++i)
            every_byte[i] = static_cast<char>(i);
        require_eq(every_byte, cryptic::base64::try_decode(cryptic::base64::encode(every_byte)).value());

        using enum cryptic::base64::decode_error;
        require_true(cryptic::base64::try_decode("Zg=") == std::unexpected{invalid_length});
        require_true(cryptic::base64::try_decode("Zm9vY") == std::unexpected{invalid_length});
        require_true(cryptic::base64::try_decode("Zm9v!A==") == std::unexpected{invalid_character});
        require_true(cryptic::base64::try_decode("Zm 9") == std::unexpected{invalid_character});
        require_true(cryptic::base64::try_decode("Zm9\n") == std::unexpected{invalid_character});
        require_true(cryptic::base64::try_decode("====") == std::unexpected{invalid_padding});
        require_true(cryptic::base64::try_decode("Z===") == std::unexpected{invalid_padding});
        require_true(cryptic::base64::try_decode("Z=g=") == std::unexpected{invalid_padding});
        require_true(cryptic::base64::try_decode("Zg==Zm8=") == std::unexpected{invalid_padding});
    };

    return 0;
}

const auto _ = register_tests();

} // namespace cryptic::base64_test

consteval bool base64_at_compile_time()
{
    const auto bytes = std::array{std::byte{'M'}, std::byte{'a'}, std::byte{'n'}};
    return cryptic::base64::encode(bytes) == "TWFu"
       and cryptic::base64::encode(std::span{bytes}.first(1)) == "TQ=="
       and cryptic::base64::try_decode("TWFu").value() == "Man"
       and cryptic::base64::try_decode("TQ==").value() == "M"
       and cryptic::base64::try_decode("TW=u") == std::unexpected{cryptic::base64::decode_error::invalid_padding};
}

static_assert(base64_at_compile_time());
