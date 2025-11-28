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
        require_eq("0UoCjCo6K8lHYQK7KII0xBWisB+CjqYqxbPkLw==", cryptic::sha224::base64(test1));

        auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq("cw4Qm9eooyscudmgmqIyXSQwWH3bwMOLrZEVJQ==", cryptic::sha224::base64(test2));

        auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq("/udV9EpV8g+zNizcPEk2FbPLV07ZXOYQ7lsemw==", cryptic::sha224::base64(test3));
    };

    test_case("CrypticSHA224::Hexadecimal") = [] {
        auto hash = cryptic::sha224{};
        require_eq(28ul, hash.size());

        auto test1 = ""s;
        require_eq("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f", cryptic::sha224::hexadecimal(test1));

        auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq("730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525", cryptic::sha224::hexadecimal(test2));

        auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq("fee755f44a55f20fb3362cdc3c493615b3cb574ed95ce610ee5b1e9b", cryptic::sha224::hexadecimal(test3));
    };

    test_case("CrypticSHA256::Base64") = [] {
        auto hash = cryptic::sha256{};
        require_eq(32ul, hash.size());

        auto test1 = ""s;
        require_eq("47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=", cryptic::sha256::base64(test1));

        auto test2 = "The quick brown fox jumps over the lazy dog"s;
        require_eq("16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI=", cryptic::sha256::base64(test2));

        auto test3 = "The quick brown fox jumps over the lazy cog"s;
        require_eq("5MTY8792tpLeeRoXPgUyEVD3o0W0ZIT+Qn9qzH7Mgb4=", cryptic::sha256::base64(test3));
    };

    test_case("CrypticSHA256::Hexadecimal") = [] {
        auto hash = cryptic::sha256{};
        require_eq(32ul, hash.size());

        auto test1 = ""s;
        require_eq("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", cryptic::sha256::hexadecimal(test1));

        auto test21 = "The quick brown fox jumps over the lazy dog"s;
        auto test22 = std::as_bytes(std::span{test21});
        require_eq("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592", cryptic::sha256::hexadecimal(test22));

        auto test31 = "The quick brown fox jumps over the lazy cog";
        auto test32 = std::as_bytes(std::span{test31,43});
        require_eq("e4c4d8f3bf76b692de791a173e05321150f7a345b46484fe427f6acc7ecc81be", cryptic::sha256::hexadecimal(test32));
    };

    test_case("CrypticSHA256::Reset") = [] {
        auto sha256 = cryptic::sha256{};
        sha256.hash(""s);

        require_eq("47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=", sha256.base64());
        require_eq("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", sha256.hexadecimal());

        sha256.hash("The quick brown fox jumps over the lazy dog"s);

        require_eq("16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI=", sha256.base64());
        require_eq("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592", sha256.hexadecimal());

        sha256.hash("The quick brown fox jumps over the lazy dog"s);

        require_eq("16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI=", sha256.base64());
        require_eq("d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592", sha256.hexadecimal());
    };

    test_case("CrypticSHA256::Hash") = [] {
        auto test1 = cryptic::sha256{}, test2 = cryptic::sha256{};
        test1.hash("The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog."s);
        require_neq(test1.base64(), test2.base64());
    };

    return 0;
}

const auto _ = register_tests();

} // namespace cryptic::sha2_test
