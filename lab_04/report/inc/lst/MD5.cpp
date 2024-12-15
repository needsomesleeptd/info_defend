#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace lib
{

namespace
{

std::uint32_t Leftrotate(std::uint32_t x, std::uint32_t c) { return (x << c) | (x >> (32 - c)); }

std::vector<std::uint32_t> AddPadding(const std::uint32_t *data, std::size_t length)
{
    std::size_t original_byte_len = length;
    std::size_t bit_len = original_byte_len * 8;
    std::size_t padding_len = 64 - ((original_byte_len + 8) % 64);
    std::size_t total_len = original_byte_len + padding_len + 8;

    std::vector<std::uint8_t> padded_data(total_len, 0);
    std::memcpy(padded_data.data(), data, original_byte_len);

    padded_data[original_byte_len] = 0x80;
    std::memcpy(padded_data.data() + total_len - 8, &bit_len, 8);

    return std::vector<std::uint32_t>(
        reinterpret_cast<std::uint32_t *>(padded_data.data()),
        reinterpret_cast<std::uint32_t *>(padded_data.data() + total_len));
}

std::array<std::uint32_t, 4> ProcessChunk(std::uint32_t *current_hash, std::uint32_t *M)
{
    const std::uint32_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                           5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
                           4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                           6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    const std::uint32_t K[] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
                           0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                           0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
                           0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                           0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
                           0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                           0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
                           0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                           0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
                           0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                           0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

    std::uint32_t A = current_hash[0];
    std::uint32_t B = current_hash[1];
    std::uint32_t C = current_hash[2];
    std::uint32_t D = current_hash[3];

    for (size_t j = 0; j < 64; ++j)
    {
        std::uint32_t F, g;

        if (j < 16)
        {
            F = (B & C) | ((~B) & D);
            g = j;
        }
        else if (j < 32)
        {
            F = (D & B) | ((~D) & C);
            g = (5 * j + 1) % 16;
        }
        else if (j < 48)
        {
            F = B ^ C ^ D;
            g = (3 * j + 5) % 16;
        }
        else
        {
            F = C ^ (B | (~D));
            g = (7 * j) % 16;
        }

        std::uint32_t temp = D;
        D = C;
        C = B;
        B = B + Leftrotate(A + F + K[j] + M[g], s[j]);
        A = temp;
    }

    current_hash[0] += A;
    current_hash[1] += B;
    current_hash[2] += C;
    current_hash[3] += D;

    return { current_hash[0], current_hash[1], current_hash[2], current_hash[3] };
}

} // namespace

namespace md5
{

std::string ToString(const std::array<std::uint32_t, 4> &hash_parts)
{
    std::ostringstream result;
    for (std::uint32_t part : hash_parts)
    {
        result << std::hex << std::setfill('0') << std::setw(8) << part;
    }
    return result.str();
}

std::string HashFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Couldn't open file.");
    }
    std::uint32_t current_hash[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
    std::streamsize buffer_size = 512;
    std::vector<std::uint8_t> buffer(buffer_size);
    std::uint64_t total_bits = 0;

    while (file)
    {
        file.read(reinterpret_cast<char *>(buffer.data()), buffer_size);
        std::streamsize bytes_read = file.gcount();
        total_bits += bytes_read * 8;

        std::size_t chunk_size = bytes_read / 64;

        for (std::size_t i = 0; i < chunk_size; ++i)
        {
            ProcessChunk(current_hash, reinterpret_cast<uint32_t *>(buffer.data() + i * 64));
        }

        if (bytes_read % 64 != 0)
        {
            std::size_t remain = bytes_read % 64;
            buffer[remain] = 0x80;
            if (remain < 56)
            {
                std::memset(buffer.data() + remain + 1, 0, 56 - remain - 1);
                buffer[56] = static_cast<std::uint8_t>(total_bits);
                buffer[57] = static_cast<std::uint8_t>(total_bits >> 8);
                buffer[58] = static_cast<std::uint8_t>(total_bits >> 16);
                buffer[59] = static_cast<std::uint8_t>(total_bits >> 24);
                ProcessChunk(current_hash, reinterpret_cast<uint32_t *>(buffer.data()));
            }
            else
            {
                std::memset(buffer.data() + remain + 1, 0, 64 - remain - 1);
                ProcessChunk(current_hash, reinterpret_cast<uint32_t *>(buffer.data()));
                std::memset(buffer.data(), 0, 56);
                buffer[56] = static_cast<std::uint8_t>(total_bits);
                buffer[57] = static_cast<std::uint8_t>(total_bits >> 8);
                buffer[58] = static_cast<std::uint8_t>(total_bits >> 16);
                buffer[59] = static_cast<std::uint8_t>(total_bits >> 24);
                ProcessChunk(current_hash, reinterpret_cast<uint32_t *>(buffer.data()));
            }
        }
    }

    return ToString({ current_hash[0], current_hash[1], current_hash[2], current_hash[3] });
}

} // namespace md5

} // namespace lib