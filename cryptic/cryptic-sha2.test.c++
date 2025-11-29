// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

import std;
import cryptic;
import tester;

using namespace std::string_literals;

namespace cryptic::sha2_test {

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticSHA224::Base64") = [] {
    auto hash = cryptic::sha224{};
        require_eq(28ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha224::base64(test1), "0UoCjCo6K8lHYQK7KII0xBWisB+CjqYqxbPkLw=="s);

    auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha224::base64(test2), "cw4Qm9eooyscudmgmqIyXSQwWH3bwMOLrZEVJQ=="s);

    auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq(cryptic::sha224::base64(test3), "/udV9EpV8g+zNizcPEk2FbPLV07ZXOYQ7lsemw=="s);
    };

    test_case("CrypticSHA224::Hexadecimal") = [] {
    auto hash = cryptic::sha224{};
        require_eq(28ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha224::hexadecimal(test1), "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f"s);

    auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha224::hexadecimal(test2), "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525"s);

    auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq(cryptic::sha224::hexadecimal(test3), "fee755f44a55f20fb3362cdc3c493615b3cb574ed95ce610ee5b1e9b"s);
    };

    test_case("CrypticSHA256::Base64") = [] {
    auto hash = cryptic::sha256{};
        require_eq(32ul, hash.size());

    auto test1 = ""s;
        require_eq(cryptic::sha256::base64(test1), "47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU="s);

    auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq(cryptic::sha256::base64(test2), "16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI="s);

    auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq(cryptic::sha256::base64(test3), "5MTY8792tpLeeRoXPgUyEVD3o0W0ZIT+Qn9qzH7Mgb4="s);
    };

    test_case("CrypticSHA256::Hexadecimal") = [] {
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

    test_case("CrypticSHA256::Reset") = [] {
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

    test_case("CrypticSHA256::Hash") = [] {
    auto test1 = cryptic::sha256{}, test2 = cryptic::sha256{};
    test1.hash("The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog."s);
        require_neq(test1.base64(), test2.base64());
    };

    test_case("CrypticSHA256::StandardVectors") = [] {
        // Additional standard test vectors
        require_eq(cryptic::sha256::hexadecimal("a"), "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb"s);
        require_eq(cryptic::sha256::hexadecimal("abc"), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"s);
        require_eq(cryptic::sha256::hexadecimal("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"s);
    };

    test_case("CrypticSHA224::StandardVectors") = [] {
        // Additional standard test vectors
        require_eq(cryptic::sha224::hexadecimal("a"), "abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5"s);
        require_eq(cryptic::sha224::hexadecimal("abc"), "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7"s);
    };

    test_case("CrypticSHA256::InputTypes") = [] {
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

    test_case("CrypticSHA256::LongMessage") = [] {
        // One million 'a's - known test vector
        std::string million(1'000'000, 'a');
        require_eq(cryptic::sha256::hexadecimal(million), "7ada70eb68068a222f2f02c77f9376c270501a1fd25beac6280325fc2950ab39"s);
    };

    test_case("CrypticSHA256::Comparison") = [] {
        auto hash1 = cryptic::sha256{"abc"s};
        auto hash2 = cryptic::sha256{"def"s};
        auto hash3 = cryptic::sha256{"abc"s};

        require_neq(hash1.base64(), hash2.base64());
        require_eq(hash1.base64(), hash3.base64());
    };

    return 0;
}

} // namespace cryptic::sha2_test

const auto _ = cryptic::sha2_test::register_tests();
