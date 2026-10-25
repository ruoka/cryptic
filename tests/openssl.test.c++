// Copyright (c) 2025-2026 Kaius Ruokonen. All rights reserved.
// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.

// Compares every hash with OpenSSL, using whichever transform the build
// selected. Keep it in tests/ with the benchmark: a project that depends on
// cryptic builds the tests in cryptic/ with its own, and links no OpenSSL.
#include <sys/types.h>
#include <openssl/evp.h>

import std;
import cryptic;
import tester;

namespace cryptic::openssl_test {

template<typename Hash>
auto openssl_hexadecimal(const EVP_MD* algorithm, std::span<const std::byte> message)
{
    auto bytes = std::array<std::byte, Hash::digest_size>{};
    auto size = 0u;
    if (EVP_Digest(message.data(), message.size(), reinterpret_cast<unsigned char*>(bytes.data()), &size, algorithm, nullptr) != 1
        or size != bytes.size())
        return std::string{"EVP_Digest failed"};
    return typename Hash::digest_type{bytes}.hexadecimal();
}

auto register_tests()
{
    using tester::basic::test_case;
    using namespace tester::assertions;

    test_case("CrypticOpenSSL::SameDigests, [cryptic]") = [] {
        auto random = std::mt19937{20261025u};
        auto message = std::vector<std::byte>(70'000);
        for (auto& octet : message)
            octet = std::byte{static_cast<unsigned char>(random())};

        auto sizes = std::vector<std::size_t>{};
        for (auto size = 0uz; size <= 300uz; ++size)
            sizes.push_back(size);
        sizes.insert(sizes.end(), {511uz, 512uz, 513uz, 1000uz, 4095uz, 4096uz, 4097uz, 65536uz, message.size()});

        auto check = [&]<typename Hash>(std::type_identity<Hash>, const EVP_MD* algorithm, std::span<const std::byte> bytes) {
            const auto expected = openssl_hexadecimal<Hash>(algorithm, bytes);
            require_eq(expected, Hash::hash(bytes).hexadecimal());

            const auto split = bytes.empty() ? 0uz : random() % bytes.size();
            auto hasher = Hash{};
            hasher.update(bytes.first(split)).update(bytes.subspan(split));
            require_eq(expected, hasher.digest().hexadecimal());
        };

        for (const auto size : sizes)
        {
            const auto bytes = std::span<const std::byte>{message}.first(size);
            check(std::type_identity<cryptic::sha1>{}, EVP_sha1(), bytes);
            check(std::type_identity<cryptic::sha224>{}, EVP_sha224(), bytes);
            check(std::type_identity<cryptic::sha256>{}, EVP_sha256(), bytes);
        }
    };

    return 0;
}

} // namespace cryptic::openssl_test

const auto _ = cryptic::openssl_test::register_tests();
