# cryptic
SHA1, SHA2 and BASE64 algorithms implemented as C++23 modules.

## Requirements

- **Clang 21+** with libc++ modules (`std.cppm`)
- Nested [tester](https://github.com/ruoka/tester) submodule (pin must match sibling libraries — see [Versioning](#versioning--releases))

## Versioning & Releases

**Current release: [`v2.0.0`](https://github.com/ruoka/cryptic/releases/tag/v2.0.0)** — first supported modules-era SemVer tag. Untagged header history is treated as v1.

See [`docs/release-policy.md`](docs/release-policy.md) for the public surface, SemVer rules, and release criteria. Ecosystem pin rule (all `deps/tester` gitlinks identical): [YarDB `docs/versioning.md`](https://github.com/ruoka/YarDB/blob/master/docs/versioning.md).

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

The benchmark hashes three message sizes (small: 43 bytes, medium: 782 bytes, large: 8294 bytes), 100,000 times each. On Apple Silicon the release build uses the AArch64 SHA instructions. The scalar baseline, the fully unrolled scalar loops, and the `asm` path are compared in [Future enhancements](#future-enhancements).

### Notes

- The benchmark uses OpenSSL's EVP API
- Times are milliseconds, so a 3 ms result is the resolution of the clock
- The benchmark uses realistic, varied test data rather than repetitive patterns

### Optimizations

Recent optimizations include:
- **Compiler hints**: Hot function attributes and always-inline hints for critical paths
- **Small message optimization**: Special-casing for single-chunk messages to avoid loop overhead
- **Memory access patterns**: Prefetching, restrict pointers, and cache-aligned data structures
- **Template-based unrolling**: Compile-time unrolling of hexadecimal encoding loops
- **Release flags**: Optional `-funroll-loops` for transform hot paths without manual loop duplication

## Future enhancements

`transform` is still the scalar reference implementation. On AArch64, when the compiler defines `__ARM_FEATURE_SHA2` (CB's Clang does: `__aarch64__`, `__ARM_FEATURE_CRYPTO`, `__ARM_FEATURE_SHA2`, `__ARM_FEATURE_SHA3`), the runtime path is one `asm` block per block:

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

Still open:

- x86 SHA-NI (`SHA256RNDS2` / `SHA256MSG1` / `SHA256MSG2`, and `SHA1RNDS4` / `SHA1NEXTE` / `SHA1MSG1` / `SHA1MSG2`), gated on `__SHA__`, once an x86-64 machine can test it.
- Scalar rounds written as C++26 `template for (constexpr auto i : std::views::iota(0uz, 64uz))` once Clang implements the iterating form of [P1306](https://clang.llvm.org/cxx_status.html).
