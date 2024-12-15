#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>

#include <iostream>

#include "lib/SHA1.h"
#include "lib/RSA.h"

namespace std
{

template <typename T, typename U>
ostream &operator<<(ostream &os, const std::pair<T, U> &p)
{
    os << "{\n\t" << std::hex << p.first << ",\n\t" << p.second << "\n}";
    return os;
}

} // namespace std

namespace
{

using namespace boost::multiprecision;
using namespace boost::random;

constexpr int s_BitLength{ 512 };

cpp_int GenerateLargePrime(int bit_length)
{
    mt11213b base_gen(static_cast<boost::uint32_t>(std::time(0)));
    independent_bits_engine<mt11213b, s_BitLength, cpp_int> gen(base_gen);

    cpp_int prime;
    do
    {
        prime = gen();
        prime |= (cpp_int(1) << (bit_length - 1));
        prime |= 1;
    } while (!miller_rabin_test(prime, 25));

    return prime;
}

void VerifySignature(const lib::RSA &rsa, const std::string &first_file,
                     const std::string &second_file)
{
    const cpp_int initial_message("0x" + lib::sha1::HashFile(first_file));
    std::cout << "Original hash: " << std::hex << initial_message << "\n";
    {
        cpp_int encrypted_message = rsa.Encrypt(initial_message);
        std::cout<< "N:" << std::dec << rsa.N  << "\n";
        std::cout << "E:" << std::dec << rsa.E << "\n";
        std::cout << "Public key: " << rsa.GetPublicKey() << "\n";
        std::cout << "Private key: " << rsa.GetPrivateKey() << "\n";
    }

    {
        cpp_int message("0x" + lib::sha1::HashFile(second_file));
        cpp_int encrypted_message = rsa.Encrypt(message);
        cpp_int decrypted_message = rsa.Decrypt(encrypted_message);
        std::cout << "encrypted,msg:" << encrypted_message << " decrypted,msg:" << decrypted_message << std::endl;
        std::cout << (initial_message == decrypted_message ? "\033[32mSignature is valid\033[0m"
                                                           : "\033[31mSignature is invalid\033[0m")
                  << "\n" << std::endl;
    }
}






} // namespace

int main()
{
    cpp_int p = GenerateLargePrime(s_BitLength / 2 - 1);
    cpp_int q = GenerateLargePrime(s_BitLength / 2);
    lib::RSA rsa{ p, q };

    VerifySignature(rsa, "./sources/archive_1.zip", "./sources/archive_2.zip");

    return 0;
}
