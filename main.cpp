#include "rev512.h"
#include <iostream>
#include <iomanip>

// 打印十六进制哈希值
void print_hash(const std::array<uint8_t, 64>& hash) {
    for (uint8_t byte : hash) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "REV-512 Hash Function Demo" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // 测试用例1：空字符串
    std::cout << "\nTest case 1: Empty string" << std::endl;
    std::array<uint8_t, 64> hash1 = rev512::hash(std::vector<uint8_t>{});
    std::cout << "Hash: " << rev512::hash_hex(std::vector<uint8_t>{}) << std::endl;
    
    // 测试用例2：短字符串 "abc"
    std::cout << "\nTest case 2: String \"abc\"" << std::endl;
    std::string str2 = "abc";
    std::array<uint8_t, 64> hash2 = rev512::hash(str2);
    std::cout << "Hash: " << rev512::hash_hex(str2) << std::endl;
    
    // 测试用例3：长字符串（1000个 'a'）
    std::cout << "\nTest case 3: 1000 'a' characters" << std::endl;
    std::string str3(1000, 'a');
    std::array<uint8_t, 64> hash3 = rev512::hash(str3);
    std::cout << "Hash: " << rev512::hash_hex(str3) << std::endl;
    
    // 测试用例4：数字字符串
    std::cout << "\nTest case 4: String \"1234567890\"" << std::endl;
    std::string str4 = "1234567890";
    std::array<uint8_t, 64> hash4 = rev512::hash(str4);
    std::cout << "Hash: " << rev512::hash_hex(str4) << std::endl;
    
    return 0;
}
