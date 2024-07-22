#include "xtts/XTTSClient.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

// 测试
using json = nlohmann::json;

int main() {
    // 构建json
    std::ifstream i("config.json");
    json config;
    i >> config;

    // 语言合成
    XTTSClient client;
    client.createXTTS(config);
    return 0;
}
