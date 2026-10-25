# Public interface

This page lists what `import cryptic;` promises under the [release policy](release-policy.md) in v3.0.0, and how to move to it from v2. v3.0.0 is not released yet; the current release is [`v2.0.1`](https://github.com/ruoka/cryptic/releases/tag/v2.0.1).

## Digests

A hash returns a `cryptic::digest<N>`: N bytes in the order the algorithm defines, 20 for SHA-1, 28 for SHA-224 and 32 for SHA-256.

```c++
const auto digest = cryptic::sha256::hash(message);   // cryptic::digest<32>
digest.hexadecimal();                                 // std::string, lower-case hex
digest.base64();                                      // std::string, padded base64
digest.bytes();                                       // std::span<const std::byte, 32>
```

- `cryptic::digest<N>{}` has every byte zero, and `cryptic::digest<N>{bytes}` copies a `std::span<const std::byte, N>`. That constructor is explicit, and a span of another length doesn't compile.
- `size()`, `data()`, `begin()` and `end()` make a digest a contiguous range of `const std::byte`, so it can be passed wherever bytes are accepted, `base64::encode` and the hashes included.
- `==` compares every byte without stopping at the first difference. `<=>` orders digests by their bytes, so a digest works as a key in `std::set` and `std::map`, and `std::hash<cryptic::digest<N>>` makes it one in `std::unordered_set` and `std::unordered_map`. Digests of different sizes don't compare.
- A digest is trivially copyable and exactly N bytes, and everything above works in a constant expression.

## Hashes

`cryptic::sha1`, `cryptic::sha224` and `cryptic::sha256` share one interface.

```c++
cryptic::sha256::hash(message);          // cryptic::digest<32>
cryptic::sha256::hexadecimal(message);   // hash(message).hexadecimal()
cryptic::sha256::base64(message);        // hash(message).base64()

auto hasher = cryptic::sha256{};
hasher.update(header).update(body);
const auto digest = hasher.digest();     // cryptic::digest<32>
```

`message` is a C string or a contiguous range of `char`, `signed char`, `unsigned char`, `char8_t` or `std::byte`: a `std::string` or `std::string_view`, a `std::u8string`, a digest, or a span, array or vector of bytes. A pointer to `char` or `char8_t` is read up to its terminating NUL. So is an array of them, a string literal included, but never past the array's end. Any other range is hashed whole, NULs included. Anything else doesn't compile: a `std::vector<int>`, a `std::wstring`, a `std::list<char>`, or a pointer to `unsigned char` or `std::byte`, which has no length; pass a span instead. Messages are read in place, never copied.

- `update()` appends to the message and returns the hasher.
- `digest()` returns the digest of everything passed to `update()` so far. It doesn't change the hasher, so later `update()` calls continue the same message. `hasher = {}` starts a new one.
- `hash()`, `hexadecimal()` and `base64()` are static. They hash their argument alone, even when called through an object: `hasher.hexadecimal(text)` ignores what `hasher` has seen.
- `digest_type` is `cryptic::digest<digest_size>`. `digest_size` is 20, 28 or 32, and `block_size` is 64.
- A hasher has no message constructor, no comparisons and no `std::hash`. Keep its digest instead.
- A hasher is trivially copyable, so a copy can finish a prefix while the original takes more input. `sha1` is 96 bytes, and `sha224` and `sha256` are 104.

With `std::byte` input, everything above also works in a constant expression. Text can only be hashed at run time, because viewing characters as bytes takes a `reinterpret_cast`.

## Base64

```c++
cryptic::base64::encode(source);   // std::string, padded, standard alphabet
cryptic::base64::decode(text);     // std::expected<std::string, cryptic::base64::decode_error>
```

`encode` takes the same inputs as the hashes and works in a constant expression for `std::byte` input.

`decode` accepts padded input in the standard alphabet and nothing else. It returns `invalid_length` when the length isn't a multiple of four, `invalid_character` for anything outside the alphabet, whitespace and the URL-safe `-` and `_` included, and `invalid_padding` for `=` anywhere but the last one or two characters. As RFC 4648 allows, it doesn't check that the unused bits of the final group are zero. It works in a constant expression.

`try_decode` is a deprecated alias for `decode`, for code written against v2.1. It goes away in a later major release.

## Internal names

`import cryptic;` exports nothing else. Two internal names appear in compiler diagnostics but can't be named outside the module: `cryptic::details::byte_message`, the concept behind the message parameters, and `cryptic::sha2`, the class template behind `sha224` and `sha256`. Either can change in any release.

## Changes in v3.0.0

v3.0.0 removes the parts of v2 that made a hash easy to misuse. Most code that relied on them stops compiling instead of changing meaning. The exception to look for is `hasher.hash(message)`: it still compiles, as a call to the static member, but it no longer changes `hasher`. The compiler warns about it, because the digest it returns is discarded.

- **Digests are values.** `hash()` is static and returns a `cryptic::digest<N>`, and so does `digest()`. A hasher no longer hashes on construction, converts from a message, compares or keeps a result. In v2, `cryptic::sha256 hash = "text"s;` compiled, a function taking `const cryptic::sha256&` accepted a `std::string`, `hash < "abc"s` hashed the string before comparing, a default-constructed hash reported its initial state as its digest, and keeping a digest meant keeping the whole hasher.
- **`digest()` replaces `finalize()`.** It doesn't change the hasher, so `update()` continues the message instead of starting a new one.
- **Byte-like messages only.** A `std::vector<int>` or a `std::wstring` was hashed as its object representation, so the digest depended on the platform. `std::as_bytes(std::span{values})` still hashes the object representation, explicitly.
- **`base64::decode` reports errors.** It is the strict decoder v2.1 added as `try_decode`. The v2 `decode` called `std::terminate` on any character outside the alphabet, dropped an incomplete final group without reporting it, and accepted `=` in the middle of the input.
- **A smaller exported surface.** `cryptic` no longer pulls in the base64 names. `to_index`, `to_character_set`, `hash_message`, `message_length_type`, `buffer_type`, `size()`, `encode()`, the `std::hash` specializations for the hashers and the `cryptic::details` test hooks are gone. The tests compare every hash with OpenSSL instead, whichever transform the build uses.

### Migrating from v2

| v2 | v3 |
| --- | --- |
| `cryptic::sha256::hexadecimal(m)`, `cryptic::sha256::base64(m)` | Unchanged |
| `cryptic::sha256{m}.hexadecimal()` | `cryptic::sha256::hexadecimal(m)` |
| `cryptic::sha256{m}`, `hash.hash(m)`, `cryptic::sha256{m}.digest()` | `cryptic::sha256::hash(m)` |
| `hasher.finalize()` | `hasher.digest()` |
| `hasher.hexadecimal()`, `hasher.base64()` | `hasher.digest().hexadecimal()`, `hasher.digest().base64()` |
| `hash.encode(buffer)` | `std::ranges::copy(digest, buffer.begin())` |
| `cryptic::sha256::buffer_type` | `cryptic::sha256::digest_type`, a `cryptic::digest<32>` rather than a `std::array` |
| `hash.size()` | `cryptic::sha256::digest_size` |
| `a == b`, `std::set<cryptic::sha256>` | Compare and store digests: `std::set<cryptic::sha256::digest_type>` |
| `cryptic::base64::decode(t)` | `cryptic::base64::decode(t).value()`, or check the result first |
| `cryptic::base64::try_decode(t)` | `cryptic::base64::decode(t)` |
| `cryptic::encode`, `cryptic::decode`, `cryptic::decode_error` | `cryptic::base64::encode`, `cryptic::base64::decode`, `cryptic::base64::decode_error` |
| `cryptic::base64::to_index`, `cryptic::base64::to_character_set` | `cryptic::base64::encode` and `cryptic::base64::decode` |
| A `std::vector<int>` or a `std::wstring` message | `std::as_bytes(std::span{message})` |
