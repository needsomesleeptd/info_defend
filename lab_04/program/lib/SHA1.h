#include <array>
#include <cstdint>
#include <string>

namespace lib
{

namespace sha1
{

std::string HashFile(const std::string &filename);
std::string ToString(const std::array<uint32_t, 4> &hash_parts);

} // namespace sha1

} // namespace lib