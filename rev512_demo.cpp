#include "rev512.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

/**
 * REV-512 多行输入哈希演示
 * 读取用户输入的多行文本，计算并输出其REV-512哈希值
 */

int main() {
    std::cout << "REV-512 多行输入哈希计算器" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "请输入多行文本（按 Ctrl+Z 或 Ctrl+D 结束输入）：" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    
    // 读取多行输入
    std::vector<uint8_t> input_data;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        // 将每行文本转换为字节并添加到输入数据中
        for (char c : line) {
            input_data.push_back(static_cast<uint8_t>(c));
        }
        // 添加换行符（保持输入的行结构）
        input_data.push_back(static_cast<uint8_t>('\n'));
    }
    
    // 如果有输入数据，移除最后一个多余的换行符
    if (!input_data.empty()) {
        input_data.pop_back();
    }
    
    std::cout << "----------------------------------" << std::endl;
    std::cout << "输入长度：" << input_data.size() << " 字节" << std::endl;
    
    // 计算REV-512哈希值
    std::array<uint8_t, 64> hash_value = rev512::hash(input_data);
    
    // 输出哈希值（十六进制格式）
    std::cout << "REV-512 哈希值：" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    
    for (size_t i = 0; i < hash_value.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(hash_value[i]);
        // 每16个字节换行，提高可读性
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;
    std::cout << "----------------------------------" << std::endl;
    
    return 0;
}
