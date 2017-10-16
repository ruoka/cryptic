#include <iostream>
#include <fstream>
#include "cryptic/sha1.hpp"

using namespace std::string_literals;

int main()
{
    std::cout << cryptic::sha1::base64("The quick brown fox jumps over the lazy dog"s) << std::endl;

    auto hash = cryptic::sha1{"The quick brown fox jumps over the lazy cog"s};
    std::cout << hash.hexadecimal() << std::endl;
    std::cout << hash.base64() << std::endl;

    auto file = std::ifstream{"./src/test.cpp"};
    auto test2 = ""s;
    std::getline(file,test2,static_cast<char>(std::char_traits<char>::eof()));
    std::cout << cryptic::sha1::base64(test2) << std::endl;

    auto test3 = "omQGMC65WBEzzZAX7H8l+g==258EAFA5-E914-47DA-95CA-C5AB0DC85B11"s;
    std::cout << cryptic::sha1::base64(test3) << std::endl;

    auto test4 = "omQGMC65WBEzzZAX7H8l+g==258EAFA5-E914-47DA-95CA-C5AB0DC85B11_XXXXXX"s;
    std::cout << cryptic::sha1::base64(test4) << std::endl;

    auto test5 = cryptic::base64::encode("Man"s);
    std::cout << test5 << std::endl;

    auto test6 = cryptic::base64::decode(test5);
    std::cout << test6 << std::endl;

    auto test7 = cryptic::base64::decode("TWFu"s);
    std::cout << test7 << std::endl;

    return 0;
}