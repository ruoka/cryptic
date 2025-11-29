# cryptic
SHA1, SHA2 and BASE64 algorithms implemented as C++23 modules.

## Building

The project uses [tester](https://github.com/ruoka/tester) for testing and C++ Builder (CB.sh) for building. To build and run tests:

```bash
./tools/CB.sh test
```

## Usage

Import the `cryptic` module and use the algorithms:

Example #1 - SHA1 Base64
```c++
import std;
import cryptic;

auto test1 = "The quick brown fox jumps over the lazy dog"s;
std::cout << cryptic::sha1::base64(test1) << std::endl;
```

Example #2 - SHA256 Hexadecimal
```c++
import std;
import cryptic;

auto test2 = "The quick brown fox jumps over the lazy dog"s;
std::cout << cryptic::sha256::hexadecimal(test2) << std::endl;
```

Example #3 - Base64 Encode
```c++
import std;
import cryptic;

auto test3 = cryptic::base64::encode("Man"s);
std::cout << test3 << std::endl;
```

Example #4 - Base64 Decode
```c++
import std;
import cryptic;

auto test4 = cryptic::base64::decode(test3);
std::cout << test4 << std::endl;
```

## Benchmark

The project includes a performance benchmark comparing `cryptic` implementations against OpenSSL's EVP API. The benchmark tests SHA1 and SHA256 algorithms across different message sizes.

### Running the Benchmark

Build and run the benchmark in release mode:

```bash
./tools/CB.sh release test
./build-darwin-release/bin/benchmark
```

### Benchmark Results

The benchmark tests three message sizes (small: 43 bytes, medium: 782 bytes, large: 8294 bytes) with 100,000 iterations per test. Results show:

**SHA1 Performance:**
- **Small messages**: `cryptic` is ~2.4x faster than OpenSSL
- **Medium messages**: OpenSSL is ~2.4x faster than `cryptic`
- **Large messages**: OpenSSL is ~3.6x faster than `cryptic`

**SHA256 Performance:**
- **Small messages**: `cryptic` is ~1.1x faster than OpenSSL
- **Medium messages**: OpenSSL is ~5.2x faster than `cryptic`
- **Large messages**: OpenSSL is ~8.0x faster than `cryptic`

### Notes

- The benchmark uses OpenSSL 3.0's modern EVP API for fair comparison
- `cryptic` performs best on small messages, making it suitable for applications that primarily hash short strings
- For large message hashing, OpenSSL's optimized implementations show significant performance advantages
- The benchmark uses realistic, varied test data rather than repetitive patterns
- Recent optimizations include extensive loop unrolling, compiler hints, and special-casing for small messages
