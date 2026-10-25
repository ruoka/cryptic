# cryptic
SHA1, SHA2 and BASE64 algorithms implemented as C++23 modules.

## Requirements

- **Clang 21+** with libc++ modules (`std.cppm`)
- Nested [tester](https://github.com/ruoka/tester) submodule (pin must match sibling libraries — see [Versioning](#versioning--releases))

## Versioning & Releases

**Current release: [`v3.0.1`](https://github.com/ruoka/cryptic/releases/tag/v3.0.1)** — same `import cryptic;` surface as v3.0.0. The tests compare with OpenSSL only in builds that link libcrypto, so a project that builds them with its own doesn't need it. [`v3.0.0`](https://github.com/ruoka/cryptic/releases/tag/v3.0.0) was the major release: hashes return `cryptic::digest<N>` values, messages must be byte-like, and `base64::decode` reports errors instead of terminating. [`v2.0.1`](https://github.com/ruoka/cryptic/releases/tag/v2.0.1) is the last v2 release, and [`v2.0.0`](https://github.com/ruoka/cryptic/releases/tag/v2.0.0) was the first supported modules-era tag. Untagged header history is treated as v1.

See [`docs/interface.md`](docs/interface.md) for the interface and how to migrate from v2, and [`docs/release-policy.md`](docs/release-policy.md) for SemVer rules and release criteria. Ecosystem pin rule (all `deps/tester` gitlinks identical): [YarDB `docs/versioning.md`](https://github.com/ruoka/YarDB/blob/master/docs/versioning.md).

## Building

The project uses [tester](https://github.com/ruoka/tester) for testing and C++ Builder (CB.sh) for building. To build and run tests:

```bash
./tools/CB.sh test
```

### Additional Compiler Flags

You can add extra compiler flags with `--compile-flags`:

```bash
./tools/CB.sh release build --compile-flags "-march=native -funroll-loops"
./tools/CB.sh release test --compile-flags "-DCRYPTIC_SCALAR_ONLY"
```

### OpenSSL

The benchmark and a test that compares every hash with OpenSSL need OpenSSL's libcrypto, so `./tools/CB.sh` links it (from Homebrew on macOS) and defines `CRYPTIC_TEST_OPENSSL`, which turns the comparison on. A project that builds cryptic's tests as part of its own leaves `CRYPTIC_TEST_OPENSSL` undefined and needs no libcrypto, even where the OpenSSL headers are installed.

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

Examples #4 to #6 need v3.0.0; see [`docs/interface.md`](docs/interface.md#changes-in-v300) for what changed from v2.

Example #4 - Base64 Decode
```c++
import std;
import cryptic;

if (const auto test4 = cryptic::base64::decode(test3))
    std::cout << *test4 << std::endl;
else if (test4.error() == cryptic::base64::decode_error::invalid_character)
    std::cout << "not base64" << std::endl;
```

Example #5 - Incremental SHA256
```c++
import std;
import cryptic;

auto test5 = cryptic::sha256{};
test5.update("The quick brown fox "s).update("jumps over the lazy dog"s);
const auto digest = test5.digest(); // cryptic::digest<32>
std::cout << digest.hexadecimal() << std::endl;
```

Example #6 - Digests as Keys
```c++
import std;
import cryptic;

auto seen = std::unordered_set<cryptic::sha256::digest_type>{};
seen.insert(cryptic::sha256::hash(test2));
std::cout << seen.contains(cryptic::sha256::hash(test2)) << std::endl;
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

The benchmark hashes three message sizes (small: 43 bytes, medium: 782 bytes, large: 8294 bytes), 100,000 times each. On Apple Silicon, `transform` uses the CPU SHA instructions. Against the scalar code that shipped in v2.0.0, SHA-1 is about 3× faster and SHA-256 about 11× faster on multi-block messages. Large messages land next to OpenSSL; medium messages are faster than OpenSSL. The comparison is in [CPU SHA instructions](#cpu-sha-instructions).

### Notes

- The benchmark uses OpenSSL's EVP API
- Times are milliseconds, so a 3 ms result is the resolution of the clock
- The benchmark uses realistic, varied test data rather than repetitive patterns

### Optimizations

Recent optimizations include:
- **Compiler hints**: Hot function attributes and always-inline hints for critical paths
- **Memory access patterns**: Prefetching, restrict pointers, and cache-aligned data structures
- **Release flags**: Optional `-funroll-loops` for transform hot paths without manual loop duplication

## CPU SHA instructions

The scalar `transform` remains the reference implementation. On AArch64, when the compiler defines `__ARM_FEATURE_SHA2` (CB's Clang does: `__aarch64__`, `__ARM_FEATURE_CRYPTO`, `__ARM_FEATURE_SHA2`, `__ARM_FEATURE_SHA3`), the runtime path is one `asm` block per block:

- SHA-256 and SHA-224 share `sha256h`, `sha256h2`, `sha256su0` and `sha256su1`. Message words are big-endian, so each 16-byte lane is `rev32`'d after the load. Round constants are the existing `k` table.
- SHA-1 uses `sha1c`, `sha1p`, `sha1m`, `sha1h`, `sha1su0` and `sha1su1`.
- `if consteval` keeps the scalar rounds, so a compile-time hash does not touch the assembly.
- Every other target, including the Linux x86-64 CI with Clang 21, builds the scalar code unchanged.

`CRYPTIC_SCALAR_ONLY` forces that scalar path, which is how both paths are tested on one Mac:

```bash
./tools/CB.sh release test --jsonl=failures --compile-flags "-DCRYPTIC_SCALAR_ONLY"
```

`CRYPTIC_UNROLL_SCALAR` adds `#pragma clang loop unroll(full)` on the scalar schedule and round loops. It is a measurement switch, not the default. At `-O3`, without the pragma, Clang leaves both the SHA-256 message schedule and the 64 rounds as counted loops (`b.ne`); it does not unroll them.

The table is three release runs on this machine, 100,000 iterations, `cryptic` milliseconds then OpenSSL milliseconds. Small-message times sit on the 1 ms clock.

| Path | SHA-1 small | SHA-1 medium | SHA-1 large | SHA-256 small | SHA-256 medium | SHA-256 large |
| --- | --- | --- | --- | --- | --- | --- |
| master scalar | 9 / 29–39 | 123–130 / 53–57 | 1267–1453 / 369–379 | 32–35 / 22–24 | 412–424 / 52–54 | 4138–4250 / 362–376 |
| scalar, `unroll(full)` | 8–15 / 24–40 | 118 / 53–54 | 1195–1229 / 368–413 | 21–23 / 23 | 304–306 / 52–53 | 3087–3368 / 362–363 |
| AArch64 `asm` | 3 / 24–38 | 38–40 / 53–55 | 389–410 / 368–380 | 3 / 23–24 | 37–38 / 52–53 | 381–383 / 366–373 |

Full unrolling is a fraction of the instruction speedup and inflates `benchmark.o` `__text` from about 8 KB to about 78 KB. The `asm` build is smaller than the scalar one (about 5 KB of `__text` in that object) because the round loops collapse into the crypto instructions.

## Future enhancements

- x86 SHA-NI (`SHA256RNDS2` / `SHA256MSG1` / `SHA256MSG2`, and `SHA1RNDS4` / `SHA1NEXTE` / `SHA1MSG1` / `SHA1MSG2`), gated on `__SHA__`, once an x86-64 machine can test it.
- Scalar rounds written as C++26 `template for (constexpr auto i : std::views::iota(0uz, 64uz))` once Clang implements the iterating form of [P1306](https://clang.llvm.org/cxx_status.html).
