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
