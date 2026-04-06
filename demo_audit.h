#include "rev_audit.h"
#include <iostream>

int main() {
    rev_audit::AuditTrail trail;

    // 模拟 AI 对话
    trail.record("你好", "我是 Rev.on AI");
    trail.record("1+1等于几", "2");
    trail.record("你是谁开发的", "Rev.on 团队");

    // 验证
    std::cout << "验证结果: " << (trail.verify(0) ? "通过" : "失败") << std::endl;

    // 篡改测试
    std::cout << "\n篡改测试:\n";
    trail.record("恶意输入", "恶意输出");
    if (trail.size() > 0) {
        // 手动改内存（演示用，实际不会这么干）
        std::cout << "注意：这里可以演示篡改后 verify 失败\n";
    }

    // 导出 JSON
    std::cout << "\n审计日志 JSON:\n" << trail.export_json() << std::endl;

    return 0;
}