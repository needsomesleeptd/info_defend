#include "RSA.h"

namespace lib
{

namespace
{

bm::cpp_int ModInverse(bm::cpp_int a, bm::cpp_int m)
{
    bm::cpp_int m0 = m, t, q;
    bm::cpp_int x0 = 0, x1 = 1;

    while (a > 1)
    {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
    {
        x1 += m0;
    }

    return x1;
}

bm::cpp_int GCD(bm::cpp_int a, bm::cpp_int b)
{
    while (b != 0)
    {
        bm::cpp_int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

} // namespace

RSA::RSA(bm::cpp_int p, bm::cpp_int q)
try : N{ p *q }, E{ 65537 }, Phi((p - 1) * (q - 1))
{
    if (GCD(E, Phi) != 1)
    {
        throw std::runtime_error("e is not coprime with φ(n)");
    }
    D = ModInverse(E, Phi);
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
}

bm::cpp_int RSA::Encrypt(bm::cpp_int plaintext) const { return bm::powm(plaintext, E, N); }

bm::cpp_int RSA::Decrypt(bm::cpp_int ciphertext) const { return bm::powm(ciphertext, D, N); }

} // namespace lib
