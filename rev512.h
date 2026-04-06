#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <string>

namespace rev512 {

/**
 * 计算给定字节数组的REV-512哈希值
 * @param message 输入消息的字节数组
 * @return 64字节（512位）的哈希值数组
 */
std::array<uint8_t, 64> hash(const std::vector<uint8_t>& message);

/**
 * 便捷函数：接受字符串输入，返回哈希值
 * @param message 输入字符串
 * @return 64字节（512位）的哈希值数组
 */
std::array<uint8_t, 64> hash(const std::string& message);

/**
 * 便捷函数：接受字符串输入，返回十六进制字符串
 * @param message 输入字符串
 * @return 128字符的十六进制哈希值字符串
 */
std::string hash_hex(const std::string& message);

/**
 * 便捷函数：接受字节数组输入，返回十六进制字符串
 * @param message 输入字节数组
 * @return 128字符的十六进制哈希值字符串
 */
std::string hash_hex(const std::vector<uint8_t>& message);

} // namespace rev512
