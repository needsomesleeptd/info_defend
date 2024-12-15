#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace lib {

namespace {

    std::uint32_t Leftrotate(std::uint32_t x, std::uint32_t c) { 
        return (x << c) | (x >> (32 - c)); 
    }

    std::vector<std::uint32_t> AddPadding(const std::uint32_t *data, std::size_t length) {
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

    std::array<std::uint32_t, 5> ProcessChunk(std::uint32_t *current_hash, std::uint32_t *M) {
        const std::uint32_t K[] = { 0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6 };

        std::uint32_t W[80];

        // Copy chunk into W (first 16 words)
        for (size_t i = 0; i < 16; ++i) {
            W[i] = M[i];
        }

        // Extend the first 16 words into the remaining 64 words of W
        for (size_t i = 16; i < 80; ++i) {
            W[i] = Leftrotate(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
        }

        std::uint32_t A = current_hash[0];
        std::uint32_t B = current_hash[1];
        std::uint32_t C = current_hash[2];
        std::uint32_t D = current_hash[3];
        std::uint32_t E = current_hash[4];

        for (size_t j = 0; j < 80; ++j) {
            std::uint32_t temp = Leftrotate(A, 5) + E + W[j];
            if (j < 20) {
                temp += ((B & C) | ((~B) & D)) + K[0];
            } else if (j < 40) {
                temp += (B ^ C ^ D) + K[1];
            } else if (j < 60) {
                temp += ((B & C) | (B & D) | (C & D)) + K[2];
            } else {
                temp += (B ^ C ^ D) + K[3];
            }
            E = D;
            D = C;
            C = Leftrotate(B, 30);
            B = A;
            A = temp;
        }

        current_hash[0] += A;
        current_hash[1] += B;
        current_hash[2] += C;
        current_hash[3] += D;
        current_hash[4] += E;

        return { current_hash[0], current_hash[1], current_hash[2], current_hash[3], current_hash[4] };
    }

} // namespace

namespace sha1 {

    std::string ToString(const std::array<std::uint32_t, 5> &hash_parts) {
        std::ostringstream result;
        for (std::uint32_t part : hash_parts) {
            result << std::hex << std::setfill('0') << std::setw(8) << part;
        }
        return result.str();
    }

    std::string HashFile(const std::string &filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Couldn't open file.");
        }
        std::uint32_t current_hash[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCF0, 0x10325476, 0xC3D2E1F0 };
        std::streamsize buffer_size = 512;
        std::vector<std::uint8_t> buffer(buffer_size);
        std::uint64_t total_bits = 0;

        while (file) {
            file.read(reinterpret_cast<char *>(buffer.data()), buffer_size);
            std::streamsize bytes_read = file.gcount();
            total_bits += bytes_read * 8;

            std::size_t chunk_size = bytes_read / 64;
            for (std::size_t i = 0; i < chunk_size; ++i) {
                ProcessChunk(current_hash, reinterpret_cast<uint32_t *>(buffer.data() + i * 64));
            }

            if (bytes_read % 64 != 0) {
                std::size_t remain = bytes_read % 64;
                buffer[remain] = 0x80;
                if (remain < 56) {
                    std::memset(buffer.data() + remain + 1, 0, 56 - remain - 1);
                    buffer[56] = static_cast<std::uint8_t>(total_bits);
                    buffer[57] = static_cast<std::uint8_t>(total_bits >> 8);
                    buffer[58] = static_cast<std::uint8_t>(total_bits >> 16);
                    buffer[59] = static_cast<std::uint8_t>(total_bits >> 24);
                    ProcessChunk(current_hash, reinterpret_cast<uint32_t *>(buffer.data()));
                } else {
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

        return ToString({ current_hash[0], current_hash[1], current_hash[2], current_hash[3], current_hash[4] });
    }

} // namespace sha1

} // namespace lib