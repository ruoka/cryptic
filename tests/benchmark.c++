// SPDX-License-Identifier: MIT
// See the LICENSE file in the project root for full license text.
#include <sys/types.h>
#include <openssl/evp.h>
import cryptic;
import std;

using namespace std::literals;

constexpr auto loops = 100'000ul;

// Realistic test data: varied content that's less compressible
static auto test_case_medium()
{
    return "The quick brown fox jumps over the lazy dog. "
           "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
           "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
           "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris. "
           "Duis aute irure dolor in reprehenderit in voluptate velit esse. "
           "Excepteur sint occaecat cupidatat non proident, sunt in culpa. "
           "Integer posuere erat a ante venenatis dapibus posuere velit aliquet. "
           "Cras mattis consectetur purus sit amet fermentum. "
           "Aenean eu leo quam. Pellentesque ornare sem lacinia quam venenatis. "
           "Nullam id dolor id nibh ultricies vehicula ut id elit. "
           "Curabitur blandit tempus porttitor. Maecenas sed diam eget risus. "
           "Vestibulum id ligula porta felis euismod semper. "
           "Cum sociis natoque penatibus et magnis dis parturient montes."sv;
}

static auto test_case_small()
{
    return "The quick brown fox jumps over the lazy dog"sv;
}

static auto test_case_large()
{
    static auto result = std::string{};
    if (result.empty()) {
        result.reserve(8192 + 102); // Reserve space for chars + newlines
        // Generate varied content with some structure
        for (std::size_t i = 0; i < 8192; ++i) {
            result.push_back(static_cast<char>('A' + (i * 17 + i * 3) % 26));
            if (i % 80 == 79) result.push_back('\n');
        }
    }
    return std::string_view{result};
}

template<typename TestCase>
static auto cryptic_sha1_test(const TestCase& test)
{
    const auto t1 = std::chrono::steady_clock::now();
    for(auto i = loops; i; --i)
    {
        auto sha1 = cryptic::sha1{};
        sha1.hash(test);
        // Don't encode - we're only benchmarking hash computation
    }
    const auto t2 = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    return ms.count();
}

template<typename TestCase>
static auto cryptic_sha256_test(const TestCase& test)
{
    const auto t1 = std::chrono::steady_clock::now();
    for(auto i = loops; i; --i)
    {
        auto sha256 = cryptic::sha256{};
        sha256.hash(test);
        // Don't encode - we're only benchmarking hash computation
    }
    const auto t2 = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    return ms.count();
}

template<typename TestCase>
static auto crypto_sha1_test(const TestCase& test)
{
    const auto t1 = std::chrono::steady_clock::now();
    // Reuse context for better performance
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    unsigned char digest[20]; // SHA1 digest length
    unsigned int digest_len;
    
    for(auto i = loops; i; --i)
    {
        EVP_MD_CTX_reset(ctx);
        EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
        EVP_DigestUpdate(ctx, static_cast<const char*>(test.data()), test.size());
        EVP_DigestFinal_ex(ctx, digest, &digest_len);
    }
    EVP_MD_CTX_free(ctx);
    
    const auto t2 = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    return ms.count();
}

template<typename TestCase>
static auto crypto_sha256_test(const TestCase& test)
{
    const auto t1 = std::chrono::steady_clock::now();
    // Reuse context for better performance
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    unsigned char digest[32]; // SHA256 digest length
    unsigned int digest_len;
    
    for(auto i = loops; i; --i)
    {
        EVP_MD_CTX_reset(ctx);
        EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
        EVP_DigestUpdate(ctx, static_cast<const char*>(test.data()), test.size());
        EVP_DigestFinal_ex(ctx, digest, &digest_len);
    }
    EVP_MD_CTX_free(ctx);
    
    const auto t2 = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    return ms.count();
}

void benchmark_size(std::string_view name, std::string_view test_data)
{
    std::clog << "\n=== " << name << " (" << test_data.size() << " bytes) ===\n";
    
    // SHA1 benchmarks
    std::clog << "SHA1:\n";
    auto const t1 = cryptic_sha1_test(test_data);
    std::clog << "  cryptic:    " << t1 << " ms\n";
    auto const t2 = crypto_sha1_test(test_data);
    std::clog << "  openssl:    " << t2 << " ms\n";
    const auto ratio1 = static_cast<float>(t1) / t2;
    if (ratio1 > 1.0f)
        std::clog << "  openssl is " << std::fixed << std::setprecision(2) << ratio1 << "x faster\n";
    else
        std::clog << "  cryptic is " << std::fixed << std::setprecision(2) << (1.0f / ratio1) << "x faster\n";
    
    // SHA256 benchmarks
    std::clog << "SHA256:\n";
    auto const t3 = cryptic_sha256_test(test_data);
    std::clog << "  cryptic:    " << t3 << " ms\n";
    auto const t4 = crypto_sha256_test(test_data);
    std::clog << "  openssl:    " << t4 << " ms\n";
    const auto ratio2 = static_cast<float>(t3) / t4;
    if (ratio2 > 1.0f)
        std::clog << "  openssl is " << std::fixed << std::setprecision(2) << ratio2 << "x faster\n";
    else
        std::clog << "  cryptic is " << std::fixed << std::setprecision(2) << (1.0f / ratio2) << "x faster\n";
}

int main()
{
    std::clog << "SHA1 & SHA256 benchmark against OpenSSL crypto\n";
    std::clog << "Looping " << loops << " times per test\n";
    
    benchmark_size("Small message", test_case_small());
    benchmark_size("Medium message", test_case_medium());
    benchmark_size("Large message", test_case_large());
    
    return 0;
}

