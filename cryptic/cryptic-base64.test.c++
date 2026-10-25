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

    test_case("CrypticBase64::Decode, [cryptic]") = [] {
        // RFC 4648, section 10
        require_eq(""s, cryptic::base64::decode("").value());
        require_eq("f"s, cryptic::base64::decode("Zg==").value());
        require_eq("fo"s, cryptic::base64::decode("Zm8=").value());
        require_eq("foo"s, cryptic::base64::decode("Zm9v").value());
        require_eq("foob"s, cryptic::base64::decode("Zm9vYg==").value());
        require_eq("fooba"s, cryptic::base64::decode("Zm9vYmE=").value());
        require_eq("foobar"s, cryptic::base64::decode("Zm9vYmFy").value());

        require_eq("pleasure."s, cryptic::base64::decode("cGxlYXN1cmUu").value());
        require_eq("leasure."s, cryptic::base64::decode("bGVhc3VyZS4=").value());
        require_eq("easure."s, cryptic::base64::decode("ZWFzdXJlLg==").value());
        require_eq("asure."s, cryptic::base64::decode("YXN1cmUu").value());
        require_eq("sure."s, cryptic::base64::decode("c3VyZS4=").value());
    };

    test_case("CrypticBase64::Alphabet, [cryptic]") = [] {
        const auto alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"s;
        const auto decoded = cryptic::base64::decode(alphabet).value();
        require_eq(48uz, decoded.size());
        require_eq(alphabet, cryptic::base64::encode(decoded));

        auto every_byte = std::string(256, '\0');
        for (auto i = 0uz; i < every_byte.size(); ++i)
            every_byte[i] = static_cast<char>(i);
        require_eq(every_byte, cryptic::base64::decode(cryptic::base64::encode(every_byte)).value());
    };

    test_case("CrypticBase64::DecodeErrors, [cryptic]") = [] {
        using enum cryptic::base64::decode_error;
        require_true(cryptic::base64::decode("Zg=") == std::unexpected{invalid_length});
        require_true(cryptic::base64::decode("Zm9vY") == std::unexpected{invalid_length});
        require_true(cryptic::base64::decode("Zm9v!A==") == std::unexpected{invalid_character});
        require_true(cryptic::base64::decode("Zm 9") == std::unexpected{invalid_character});
        require_true(cryptic::base64::decode("Zm9\n") == std::unexpected{invalid_character});
        require_true(cryptic::base64::decode("Zm9v-_==") == std::unexpected{invalid_character});
        require_true(cryptic::base64::decode("====") == std::unexpected{invalid_padding});
        require_true(cryptic::base64::decode("Z===") == std::unexpected{invalid_padding});
        require_true(cryptic::base64::decode("Z=g=") == std::unexpected{invalid_padding});
        require_true(cryptic::base64::decode("Zg==Zm8=") == std::unexpected{invalid_padding});
    };

    test_case("CrypticBase64::TryDecode, [cryptic]") = [] {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        require_eq("foobar"s, cryptic::base64::try_decode("Zm9vYmFy").value());
        require_true(cryptic::base64::try_decode("Zm9v!A==") == std::unexpected{cryptic::base64::decode_error::invalid_character});
#pragma clang diagnostic pop
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
       and cryptic::base64::decode("TWFu").value() == "Man"
       and cryptic::base64::decode("TQ==").value() == "M"
       and cryptic::base64::decode("TW=u") == std::unexpected{cryptic::base64::decode_error::invalid_padding};
}

static_assert(base64_at_compile_time());

static_assert(std::same_as<decltype(cryptic::base64::decode("")),
                           std::expected<std::string, cryptic::base64::decode_error>>);

namespace {

template<typename Source>
concept base64_encodes = requires(const Source& source) { cryptic::base64::encode(source); };

} // namespace

static_assert(base64_encodes<std::string> and base64_encodes<const char*> and base64_encodes<std::u8string_view>
          and base64_encodes<std::span<const std::byte>> and base64_encodes<cryptic::sha256::digest_type>);
static_assert(not base64_encodes<std::vector<int>> and not base64_encodes<std::wstring>
          and not base64_encodes<std::list<char>> and not base64_encodes<int>);
static_assert(not base64_encodes<volatile char*> and not base64_encodes<std::span<volatile char>>);
