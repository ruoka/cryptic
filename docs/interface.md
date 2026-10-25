# Public interface

This page lists what `import cryptic;` promises under the [release policy](release-policy.md), what it exports without promising, and the changes that have to wait for v3.0.0 because they would break callers. It started as a review of v2.0.1 in October 2026.

## Hashes

`cryptic::sha1`, `cryptic::sha224` and `cryptic::sha256` share one interface.

```c++
cryptic::sha256::hexadecimal(message);   // std::string, lower-case hex
cryptic::sha256::base64(message);        // std::string, padded base64

auto hash = cryptic::sha256{message};    // hashes on construction
hash.hash(other);                        // hashes again, replacing the previous message

auto stream = cryptic::sha256{};
stream.update(header).update(body);
const auto digest = stream.finalize();   // std::array<std::byte, 32>
```

`message` can be a `std::string`, a `std::string_view`, a string literal or other C string, a span or array of bytes, or any other contiguous range. A pointer to `char` or `char8_t` is read up to its terminating NUL. So is an array of them, a string literal included, but never past the array's end. Any other range is hashed as its object representation, NULs included. Messages are read in place, never copied, and anything else is rejected at the call.

- `update()` appends to the message. After `hash()` or `finalize()`, it starts a new one.
- `finalize()` pads the message, keeps the digest and returns it. Calling it again returns the same digest.
- `hash()` replaces the message, including any bytes from an unfinished `update()`.
- The results below are the digest after `hash()` or `finalize()`. Before that they are the internal state, so a default-constructed `cryptic::sha1{}` reports `67452301…`, not the hash of the empty message.

| Member | Result |
| --- | --- |
| `hexadecimal()` | lower-case hex `std::string` |
| `base64()` | padded base64 `std::string` |
| `digest()` | the big-endian digest bytes as `buffer_type`, a `std::array<std::byte, digest_size>` |
| `encode(std::span<std::byte, digest_size>)` | writes the same bytes into a caller buffer |
| `digest_size`, `size()` | 20, 28 or 32 |
| `block_size` | 64 |

`==` compares the whole digest without stopping at the first difference. `<=>` and `<` order digests by their bytes, so a hash works as a key in `std::set` and `std::map`, and through the `std::hash` specializations in `std::unordered_set` and `std::unordered_map`. `<` also accepts a `std::span<const std::byte, digest_size>`. Comparing a hash with a string, or a `sha224` with a `sha256`, doesn't compile.

With `std::byte` input, everything above also works in a constant expression. Text can only be hashed at run time, because viewing characters as bytes takes a `reinterpret_cast`.

## Base64

```c++
cryptic::base64::encode(source);     // padded, standard alphabet
cryptic::base64::try_decode(text);   // std::expected<std::string, cryptic::base64::decode_error>
cryptic::base64::decode(text);
```

`encode` takes the same inputs as the hashes and works in a constant expression for `std::byte` input.

`try_decode` accepts padded input in the standard alphabet and nothing else. It returns `invalid_length` when the length isn't a multiple of four, `invalid_character` for anything outside the alphabet, whitespace included, and `invalid_padding` for `=` anywhere but the last one or two characters. As RFC 4648 allows, it doesn't check that the unused bits of the final group are zero. Use it for text that comes from outside the process.

`decode` works as it always has. It calls `std::terminate` on any character outside the alphabet, drops an incomplete final group without reporting it, and accepts `=` in the middle of the input.

## Exported but not covered

These names are reachable through `import cryptic;`, but they are not part of the SemVer contract.

- `cryptic::encode`, `cryptic::decode`, `cryptic::try_decode`, `cryptic::decode_error`, `cryptic::to_index` and `cryptic::to_character_set`. `cryptic.c++m` pulls the base64 names into `cryptic` with `using namespace cryptic::base64;`. Spell them `cryptic::base64::…`.
- `cryptic::base64::to_index` and `to_character_set`, alphabet lookups that use index 64 for `=`. `to_index` terminates on a character outside the alphabet.
- `hash_message<bool ForceScalar>()` and `message_length_type` on every hash class.
- `cryptic::details::sha1_paths` and `cryptic::details::sha2_paths`, the test hooks that run the scalar or the accelerated transform.

## Changes since v2.0.1

These are unreleased and planned for v2.1.0. Apart from the fixes below, code written against v2.0.1 compiles and gives the same results. There are two narrow exceptions:

- Explicit template arguments for a message, as in `sha1::hexadecimal<std::string>(text)`, no longer compile with an lvalue. Let the type be deduced.
- A program that specialized `std::hash` for a cryptic hash now defines it a second time, which breaks the one-definition rule without a diagnostic. Remove the program's own specialization.

### Fixed

- `sha1::operator<` read past the end of its five-word digest whenever the two digests were equal, because the loop used `sizeof` of the array as its element count. Inserting a duplicate into a `std::set<cryptic::sha1>` was enough to trigger it. A `static_assert` now compares equal digests in a constant expression, where an out-of-bounds read is a compile error.
- `sha224` and `sha256` could be neither copy- nor move-assigned. The user-declared move constructor deleted copy assignment and suppressed move assignment, so `std::sort` rejected a `std::vector<cryptic::sha256>`. `sha1` was not affected.
- Messages were passed by value through every layer, so `sha256::hexadecimal(text)` copied a `std::string` four times. They are now forwarding references, and the tests check that a message is never copied.
- A `char` array without a NUL was read past its end, because it decayed to a pointer and was measured up to the first NUL in memory. An array is now read up to its first NUL or its end, whichever comes first.
- CI runs `--tags='\[cryptic\]'`, but 17 of the 25 tests had no `[cryptic]` tag, so CI ran only 8 of them. Every test is tagged now.

### Added

- Incremental hashing with `update()` and `finalize()`.
- `digest()`, `digest_size`, `block_size`, `==`, `<=>` and `std::hash` for every hash.
- `base64::try_decode` and `base64::decode_error`.
- `base64::encode` for string literals, C strings and byte spans, and C arrays of `std::byte` for the hashes. v2.0.1 rejected all of these.
- `char8_t` strings, `u8"…"` literals and `const char8_t*` included, read like `char` strings.
- `constexpr` on `base64()`, `encode()` and the comparisons. The byte output no longer depends on the host's byte order.
- Constrained message parameters. An unsupported argument such as a `std::list<char>` now fails at the call with a `byte_source` error, instead of deep inside the library.

### Changed

- Every hash object carries a 64-byte block buffer for `update()`. `sha1` grew from 32 to 96 bytes, and `sha224` and `sha256` from 40 to 112.
- `cryptic.c++m` now exports every interface partition, as [module.unit]/3 requires, instead of importing them and re-exporting names one by one. The `:details` partition exports nothing, and `sha2` is exported only through `sha224` and `sha256`, so the names visible after `import cryptic;` are the same as before plus the additions.

## Deferred to v3.0.0

These change existing behaviour or remove names, so they need a major release.

- **Explicit message constructors.** Today `cryptic::sha256 hash = "text"s;` compiles, a function taking `const cryptic::sha256&` accepts a `std::string`, and `hash < "abc"s` hashes the string before comparing. The same conversion makes `hash < hash.digest()` ambiguous, so comparing with bytes needs an explicit `std::span`.
- **A `decode` that reports errors.** `decode` and `to_index` terminate on bad input. `try_decode` is the replacement.
- **A smaller exported surface.** Drop `using namespace cryptic::base64;` from `cryptic`, make `hash_message` private, and stop exporting `message_length_type`, `to_index`, `to_character_set` and the `details` hooks.
- **Byte-like messages only.** A `std::vector<int>` or a `std::wstring` is hashed as its in-memory representation, so the digest depends on the platform.
- **A digest value separate from the hasher.** A default-constructed hash reports its initial state as its digest, and every stored digest carries the hasher's block buffer.

A possible shape:

```c++
class sha256
{
public:
    static constexpr std::size_t digest_size = 32, block_size = 64;
    using digest_type = std::array<std::byte, digest_size>;

    sha256& update(std::span<const std::byte>) noexcept;
    sha256& update(std::string_view) noexcept;
    digest_type finalize() noexcept;

    static digest_type hash(std::span<const std::byte>) noexcept;
    static digest_type hash(std::string_view) noexcept;
    static std::string hexadecimal(std::string_view);
    static std::string base64(std::string_view);
};
```
