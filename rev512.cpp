#include "rev512.h"
#include <cstring>

namespace {

// 轮常数RC（80个64位值）
static constexpr uint64_t RC[80] = {
    0x6A09E667F3BCC908, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B, 0x7311C2812425CFA0,
    0xA54FF53A5F1D36F1, 0xD413CCCFE7799211, 0x298B075B4B6A5240, 0x510E527FADE682D1,
    0x76CF5D0B09954E76, 0x9B05688C2B3E6C1F, 0xBDDD422D07E9240A, 0xDF7BD629E9DB362F,
    0x1F83D9ABFB41BD6B, 0x3E1DB337DB365B1A, 0x5BE0CD19137E2179, 0x78DDE6E5FD29F057,
    0x9523AE4547A14CCC, 0xB0BF165515A9B348, 0xCBBB9D5DC1059ED8, 0xE6238502484B9F40,
    0x195957C48BFD9862, 0x32370B908E5FF5B1, 0x4A9FEA74BE3A6DE3, 0x629A292A367CD507,
    0x7A2B748DA963BC20, 0x9159015A3070DD17, 0xA827999FCEF32422, 0xBE9BA858B43C0323,
    0xD4B9436CE8E86C6C, 0xEA843464F08B39BC, 0x152FECD8F70E5939, 0x2A17093DC1965F6E,
    0x3EB83056B4E27892, 0x53160EB696D4A481, 0x67332667FFC00B31, 0x7B11D2898498F55F,
    0x8EB44A8768581511, 0xA21CA4FF5BCD05A3, 0xB54CDA58FBBEE883, 0xC846C71C3408C03C,
    0xDB0C2E0D64F98FA7, 0xED9EBA16132A9CEC, 0x12318007C2AFED2B, 0x2434A74B50F36125,
    0x360AD118567CD83F, 0x47B5481DBEFA4FA4, 0x593547836C716EE1, 0x6A8BFBEAB875FD80,
    0x7BBA845A0FD24815, 0x8CC1F315B3D6F707, 0x9DA34E67711BE573, 0xAE5F9156E7B6D99B,
    0xBEF7AC53D3B66C5E, 0xCF6C85D39D1A1E15, 0xDFBEFAE353C4785E, 0xEFEFDFAF1D57A4D5,
    0x0FF01FB0DD682588, 0x1FC0FB1B5C05DE20, 0x2F73477D6A4563CA, 0x3F07B357F6837AD6,
    0x4E7EE6C768047F3C, 0x5DD983D657ED756A, 0x6D1826CAFD82E1ED, 0x7C3B666FB66CB634,
    0x8B43D4570A51B936, 0x9A31FD1B80A706CA, 0xA906689B97F54427, 0xB7C19A3226FC42F3,
    0xC66410EB69F159EA, 0xD4EE47B6F881C075, 0xE360B596DC380C3F, 0xF1BBCDCBFA53E0AF,
    0x0E2DB86CFC11CF76, 0x1C456002CE13E9F8, 0x2A475C8A8F429998, 0x383410C8174D10FF,
    0x460BDC99BC19F287, 0x53CF1D166972D444, 0x617E2CAA2B536690, 0x6F19633143A0AF0E
};

// 旋转偏移量（用于ρ步）
static constexpr int RHO_OFFSETS[5][5] = {
    {0, 1, 62, 28, 27},
    {36, 44, 6, 55, 20},
    {3, 10, 43, 25, 39},
    {41, 45, 15, 21, 8},
    {18, 2, 61, 56, 14}
};

// 64位循环左移
inline constexpr uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

// 加载64位小端序数据（可移植实现）
inline constexpr uint64_t load64_le(const uint8_t* p) {
    return (uint64_t)p[0] |
           ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

// 存储64位小端序数据（可移植实现）
inline void store64_le(uint64_t x, uint8_t* p) {
    p[0] = static_cast<uint8_t>(x);
    p[1] = static_cast<uint8_t>(x >> 8);
    p[2] = static_cast<uint8_t>(x >> 16);
    p[3] = static_cast<uint8_t>(x >> 24);
    p[4] = static_cast<uint8_t>(x >> 32);
    p[5] = static_cast<uint8_t>(x >> 40);
    p[6] = static_cast<uint8_t>(x >> 48);
    p[7] = static_cast<uint8_t>(x >> 56);
}

// 执行一轮置换
void permute(uint64_t A[5][5], int round) {
    uint64_t C[5];
    uint64_t D[5];
    uint64_t B[5][5];
    
    // θ步：扩散层
    for (int x = 0; x < 5; ++x) {
        C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];
    }
    for (int x = 0; x < 5; ++x) {
        D[x] = C[(x + 4) % 5] ^ rotl64(C[(x + 1) % 5], 1);
    }
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            A[x][y] ^= D[x];
        }
    }
    
    // ρ步：字内旋转
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            A[x][y] = rotl64(A[x][y], RHO_OFFSETS[x][y]);
        }
    }
    
    // π步：字位置置换
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            B[y][(2 * x + 3 * y) % 5] = A[x][y];
        }
    }
    
    // 将B复制回A
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            A[x][y] = B[x][y];
        }
    }
    
    // χ步：非线性层
    for (int y = 0; y < 5; ++y) {
        uint64_t t0 = A[0][y];
        uint64_t t1 = A[1][y];
        uint64_t t2 = A[2][y];
        uint64_t t3 = A[3][y];
        uint64_t t4 = A[4][y];
        
        A[0][y] = t0 ^ ((~t1) & t2);
        A[1][y] = t1 ^ ((~t2) & t3);
        A[2][y] = t2 ^ ((~t3) & t4);
        A[3][y] = t3 ^ ((~t4) & t0);
        A[4][y] = t4 ^ ((~t0) & t1);
    }
    
    // ι步：加轮常数
    A[0][0] ^= RC[round];
}

// 执行80轮置换
void permute_80(uint64_t A[5][5]) {
    for (int round = 0; round < 80; ++round) {
        permute(A, round);
    }
}

// 填充消息，使用multi-rate padding（10*1规则）
std::vector<uint8_t> pad_message(const std::vector<uint8_t>& message) {
    const size_t r_bytes = 512 / 8; // 64 bytes
    const size_t m_len = message.size();
    
    // 计算需要填充的总长度
    // 先加1个字节的'1'，然后加足够0使长度到 r_bytes-1，最后加'1'占1字节
    // 总填充字节数 = (r_bytes - (m_len % r_bytes))，如果余数=0则加一个完整块
    size_t pad_len = r_bytes - (m_len % r_bytes);
    if (pad_len == 0) {
        pad_len = r_bytes; // 已经是整数倍，加一个完整块
    }
    
    // 创建填充后的消息
    std::vector<uint8_t> padded(m_len + pad_len, 0);
    std::memcpy(padded.data(), message.data(), m_len);
    
    // 第一个填充字节是 0x01
    padded[m_len] = 0x01;
    
    // 中间填充 0x00（已在vector初始化时设置）
    
    // 最后一个填充字节是 0x80
    padded.back() = 0x80;
    
    return padded;
}

} // anonymous namespace

namespace rev512 {

std::array<uint8_t, 64> hash(const std::vector<uint8_t>& message) {
    // 初始化状态
    uint64_t A[5][5] = {0};
    
    // 填充消息
    std::vector<uint8_t> padded = pad_message(message);
    const size_t r_bytes = 512 / 8;
    
    // 吸收阶段
    for (size_t block_idx = 0; block_idx < padded.size(); block_idx += r_bytes) {
        // 将块数据按小端序转换为8个64位字，并异或到状态中
        for (size_t i = 0; i < 8; ++i) {
            int x = i % 5;
            int y = i / 5;
            uint64_t word = load64_le(&padded[block_idx + i * 8]);
            A[x][y] ^= word;
        }
        
        // 执行80轮置换
        permute_80(A);
    }
    
    // 挤压阶段
    std::array<uint8_t, 64> hash_value;
    for (size_t i = 0; i < 8; ++i) {
        int x = i % 5;
        int y = i / 5;
        store64_le(A[x][y], &hash_value[i * 8]);
    }
    
    return hash_value;
}

std::array<uint8_t, 64> hash(const std::string& message) {
    std::vector<uint8_t> message_bytes(message.begin(), message.end());
    return hash(message_bytes);
}

std::string hash_hex(const std::vector<uint8_t>& message) {
    std::array<uint8_t, 64> hash_value = hash(message);
    
    std::string hex;
    hex.reserve(128);
    static const char hex_chars[] = "0123456789abcdef";
    
    for (uint8_t byte : hash_value) {
        hex.push_back(hex_chars[(byte >> 4) & 0x0F]);
        hex.push_back(hex_chars[byte & 0x0F]);
    }
    
    return hex;
}

std::string hash_hex(const std::string& message) {
    std::vector<uint8_t> message_bytes(message.begin(), message.end());
    return hash_hex(message_bytes);
}

} // namespace rev512
