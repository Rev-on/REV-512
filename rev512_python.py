#!/usr/bin/env python3
"""
REV-512 哈希函数 Python 实现
"""

import sys

# 轮常数RC（80个64位值）
RC = [
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
]

# 旋转偏移量（用于ρ步）
RHO_OFFSETS = [
    [0, 1, 62, 28, 27],
    [36, 44, 6, 55, 20],
    [3, 10, 43, 25, 39],
    [41, 45, 15, 21, 8],
    [18, 2, 61, 56, 14]
]

# 64位循环左移
def rotl64(x, n):
    return ((x << n) & 0xFFFFFFFFFFFFFFFF) | (x >> (64 - n))

# 初始化5×5状态矩阵
def init_state():
    return [[0] * 5 for _ in range(5)]

# 执行一轮置换
def permute(state, round_num):
    # θ步：扩散层
    C = [0] * 5
    for x in range(5):
        C[x] = state[x][0] ^ state[x][1] ^ state[x][2] ^ state[x][3] ^ state[x][4]
    
    D = [0] * 5
    for x in range(5):
        D[x] = C[(x + 4) % 5] ^ rotl64(C[(x + 1) % 5], 1)
    
    for x in range(5):
        for y in range(5):
            state[x][y] ^= D[x]
    
    # ρ步：字内旋转
    new_state = init_state()
    for x in range(5):
        for y in range(5):
            new_state[x][y] = rotl64(state[x][y], RHO_OFFSETS[x][y])
    
    # π步：字位置置换
    for x in range(5):
        for y in range(5):
            new_x = y
            new_y = (2 * x + 3 * y) % 5
            state[new_x][new_y] = new_state[x][y]
    
    # χ步：非线性层
    for y in range(5):
        t0 = state[0][y]
        t1 = state[1][y]
        t2 = state[2][y]
        t3 = state[3][y]
        t4 = state[4][y]
        
        state[0][y] = t0 ^ ((~t1) & t2)
        state[1][y] = t1 ^ ((~t2) & t3)
        state[2][y] = t2 ^ ((~t3) & t4)
        state[3][y] = t3 ^ ((~t4) & t0)
        state[4][y] = t4 ^ ((~t0) & t1)
    
    # ι步：加轮常数
    state[0][0] ^= RC[round_num]

# 执行80轮置换
def permute_80(state):
    for round_num in range(80):
        permute(state, round_num)

# 填充消息，使用multi-rate padding（10*1规则）
def pad_message(message):
    r_bytes = 512 // 8  # 64
    m_len = len(message)
    
    # 需要填充的总长度（使填充后长度为 r_bytes 的整数倍）
    # 先加1个字节的'1'，然后加足够0使长度到 r_bytes-1，最后加'1'占1字节
    # 所以总填充字节数 = (r_bytes - (m_len % r_bytes)) 如果余数=0则加一个完整块
    pad_len = r_bytes - (m_len % r_bytes)
    if pad_len == 0:
        pad_len = r_bytes  # 已经是整数倍，加一个完整块
    
    # 创建填充后的消息
    padded = bytearray(m_len + pad_len)
    padded[:m_len] = message
    
    # 第一个填充字节是 0x01
    padded[m_len] = 0x01
    
    # 中间填充 0x00
    for i in range(1, pad_len - 1):
        padded[m_len + i] = 0x00
    
    # 最后一个填充字节是 0x80
    padded[m_len + pad_len - 1] = 0x80
    
    return padded

# 加载64位小端序数据
def load64_le(bytes_data, offset):
    return (
        bytes_data[offset] |
        (bytes_data[offset + 1] << 8) |
        (bytes_data[offset + 2] << 16) |
        (bytes_data[offset + 3] << 24) |
        (bytes_data[offset + 4] << 32) |
        (bytes_data[offset + 5] << 40) |
        (bytes_data[offset + 6] << 48) |
        (bytes_data[offset + 7] << 56)
    )

# 存储64位小端序数据
def store64_le(word, bytes_data, offset):
    bytes_data[offset] = word & 0xFF
    bytes_data[offset + 1] = (word >> 8) & 0xFF
    bytes_data[offset + 2] = (word >> 16) & 0xFF
    bytes_data[offset + 3] = (word >> 24) & 0xFF
    bytes_data[offset + 4] = (word >> 32) & 0xFF
    bytes_data[offset + 5] = (word >> 40) & 0xFF
    bytes_data[offset + 6] = (word >> 48) & 0xFF
    bytes_data[offset + 7] = (word >> 56) & 0xFF

# 计算REV-512哈希值
def rev512_hash(message):
    # 将字符串转换为字节数组
    if isinstance(message, str):
        message = message.encode('utf-8')
    
    # 初始化状态
    state = init_state()
    
    # 填充消息
    padded = pad_message(message)
    r_bytes = 512 // 8
    
    # 吸收阶段
    for block_idx in range(0, len(padded), r_bytes):
        # 将块数据按小端序转换为8个64位字，并异或到状态中
        for i in range(8):
            x = i % 5
            y = i // 5
            word = load64_le(padded, block_idx + i * 8)
            state[x][y] ^= word
        
        # 执行80轮置换
        permute_80(state)
    
    # 挤压阶段
    hash_bytes = bytearray(64)
    for i in range(8):
        x = i % 5
        y = i // 5
        store64_le(state[x][y], hash_bytes, i * 8)
    
    return hash_bytes

# 将哈希值转换为十六进制字符串
def hash_to_hex(hash_bytes):
    return ''.join(f'{byte:02x}' for byte in hash_bytes)

# 主函数，提供用户交互
def main():
    print("REV-512 哈希计算器")
    print("==================")
    print("输入字符串，程序将输出其REV-512哈希值")
    print("输入'quit'或'exit'退出程序")
    print()
    
    while True:
        try:
            # 获取用户输入
            user_input = input("请输入字符串: ").strip()
            
            # 检查退出命令
            if user_input.lower() in ['quit', 'exit']:
                print("感谢使用，再见！")
                break
            
            # 计算哈希值
            hash_bytes = rev512_hash(user_input)
            hash_hex = hash_to_hex(hash_bytes)
            
            # 输出结果
            print(f"\nREV-512 哈希值: {hash_hex}")
            print()
            
        except KeyboardInterrupt:
            print("\n\n程序被中断，退出")
            break
        except Exception as e:
            print(f"\n发生错误: {e}")
            print()

if __name__ == "__main__":
    main()
