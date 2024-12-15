#pragma once

#include <boost/multiprecision/cpp_int.hpp>

namespace lib
{

namespace bm = boost::multiprecision;

class RSA
{
public:
    RSA(bm::cpp_int p, bm::cpp_int q);

    bm::cpp_int Encrypt(bm::cpp_int message) const;
    bm::cpp_int Decrypt(bm::cpp_int ciphertext) const;

    std::pair<bm::cpp_int, bm::cpp_int> GetPublicKey() const { return { N, E }; }
    std::pair<bm::cpp_int, bm::cpp_int> GetPrivateKey() const { return { N, D }; }


    bm::cpp_int N, E, D, Phi;
};

} // namespace lib