#include "XTTSClient.hpp"
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>

using namespace std;
using namespace AIKIT;

XTTSClient* XTTSClient::instance_ = nullptr;

// XTTSClient构造函数
XTTSClient::XTTSClient() : ttsFinished(false), fin(nullptr) {

    // 创建目录./audio
    const char* dir = "./audio";
    struct stat info;
    if (stat(dir, &info) != 0) {
        if (mkdir(dir, 0777) == -1) {
            throw runtime_error("mkdir ./audio fail.");
        }
    } else if (!(info.st_mode & S_IFDIR)) {
        throw runtime_error("./audio is not a directory.");
    }

    // 密钥读取
    json config = readConfig("./config/config.json");

    // 确认 JSON 文件包含预期的键
    if (!config.contains("appID") || !config.contains("apiSecret") || !config.contains("apiKey")) {
        throw runtime_error("JSON config file is missing required fields.");
    }


    // 将 std::string 转换为 const char*
    std::string appID = config["appID"].get<std::string>();
    std::string apiSecret = config["apiSecret"].get<std::string>();
    std::string apiKey = config["apiKey"].get<std::string>();

    // 初始化AIKIT配置
    AIKIT_Configurator::builder()
        .app().appID(appID.c_str()).apiSecret(apiSecret.c_str()).apiKey(apiKey.c_str()).workDir("./")
        .auth().authType(0)
        .log().logLevel(LOG_LVL_INFO).logPath("./");

    // 初始化AIKIT
    if (AIKIT_Init() != 0) {
        throw runtime_error("AIKIT_Init failed");
    }

    // 注册回调函数
    AIKIT_Callbacks cbs = {OnOutput, OnEvent, OnError};
    AIKIT_RegisterAbilityCallback(ABILITY, cbs);
    instance_ = this;
}

// XTTSClient析构函数
XTTSClient::~XTTSClient() {
    AIKIT_UnInit();
}

// 从JSON文件中读取配置
json XTTSClient::readConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Unable to open config file: " + filename);
    }

    json config;
    file >> config;
    return config;
}

// 创建XTTS合成任务
void XTTSClient::createXTTS(const json& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    // 输出配置参数
    cout << "==========================> text: " << config["text"] << endl;
    cout << "==========================> vcn: " << config["vcn"] << endl;
    cout << "==========================> vcnModel: " << config["vcnModel"] << endl;
    cout << "==========================> language: " << config["language"] << endl;
    cout << "==========================> speed: " << config["speed"] << endl;
    cout << "==========================> pitch: " << config["pitch"] << endl;
    cout << "==========================> volume: " << config["volume"] << endl;
    cout << "==========================> textEncoding: " << config["textEncoding"] << endl;
    cout << "==========================> sample_rate: " << config["sample_rate"] << endl;
    cout << "==========================> bits_per_sample: " << config["bits_per_sample"] << endl;
    cout << "==========================> channels: " << config["channels"] << endl;

    // 打开输出文件
    std::string audio_name = "./audio/" + config["audio_name"].get<std::string>() + ".wav";

    AIKIT_ParamBuilder* paramBuilder = nullptr;
    AIKIT_DataBuilder* dataBuilder = nullptr;
    string cntext = config["text"].get<string>();
    AIKIT_HANDLE* handle = nullptr;
    bool need_cleanup = false;

    // 创建参数构建器
    paramBuilder = AIKIT_ParamBuilder::create();
    paramBuilder->param("vcn", config["vcn"].get<string>().c_str(), config["vcn"].get<string>().size());
    paramBuilder->param("vcnModel", config["vcnModel"].get<string>().c_str(), config["vcnModel"].get<string>().size());
    paramBuilder->param("language", config["language"].get<int>());
    paramBuilder->param("speed", config["speed"].get<int>());
    paramBuilder->param("pitch", config["pitch"].get<int>());
    paramBuilder->param("volume", config["volume"].get<int>());
    paramBuilder->param("textEncoding", config["textEncoding"].get<string>().c_str(), config["textEncoding"].get<string>().size());

    // 启动AIKIT任务
    if (AIKIT_Start(ABILITY, AIKIT_Builder::build(paramBuilder), nullptr, &handle) != 0) {
        need_cleanup = true;
        goto cleanup;
    }

    // 创建数据构建器
    dataBuilder = AIKIT_DataBuilder::create();
    dataBuilder->payload(AiText::get("text")->data(cntext.c_str(), cntext.length())->once()->valid());

    fin = fopen(audio_name.c_str(), "wb");
    if (fin == nullptr) {
        printf("===================================> fopen output.wav fail.\n");
        need_cleanup = true;
        goto cleanup;
    }

    //  初始化音频数据缓冲区，写入WAV文件头
    audioData.clear();
    WriteWAVHeader(fin, config["sample_rate"], config["bits_per_sample"], config["channels"]);

    // 写入数据
    if (AIKIT_Write(handle, AIKIT_Builder::build(dataBuilder)) != 0) {
        need_cleanup = true;
        goto cleanup;
    }

    // 等待合成完成
    {
        unique_lock<mutex> lk(cvMutex);
        cv.wait(lk, [this] { return ttsFinished.load(); });
    }

    // 结束AIKIT任务
    AIKIT_End(handle);

cleanup:
    // 清理资源
    if (fin != nullptr) {
        UpdateWAVHeader(fin);
        fclose(fin);
        fin = nullptr;
    }
    if (paramBuilder != nullptr) {
        delete paramBuilder;
        paramBuilder = nullptr;
    }
    if (dataBuilder != nullptr) {
        delete dataBuilder;
        dataBuilder = nullptr;
    }
    if (need_cleanup && handle != nullptr) {
        AIKIT_End(handle);
    }
}

// 写入WAV文件头
void XTTSClient::WriteWAVHeader(FILE *fout, int sampleRate, int bitsPerSample, int channels) {
    WAVHeader hdr{};
    memcpy(hdr.riff_header, "RIFF", 4);
    memcpy(hdr.wave_header, "WAVE", 4);
    memcpy(hdr.fmt_header, "fmt ", 4);
    hdr.fmt_chunk_size = 16;
    hdr.audio_format = 1; // PCM
    hdr.num_channels = channels;
    hdr.sample_rate = sampleRate;
    hdr.bit_depth = bitsPerSample;
    hdr.byte_rate = sampleRate * channels * (bitsPerSample / 8);
    hdr.sample_alignment = channels * (bitsPerSample / 8);
    memcpy(hdr.data_header, "data", 4);
    hdr.data_bytes = 0;
    hdr.wav_size = 36 + hdr.data_bytes;

    fwrite(&hdr, sizeof(hdr), 1, fout);
}

// 更新WAV文件头
void XTTSClient::UpdateWAVHeader(FILE *fout) {
    long file_size = ftell(fout);
    fseek(fout, 4, SEEK_SET);
    uint32_t riff_size = file_size - 8;
    fwrite(&riff_size, sizeof(riff_size), 1, fout);

    fseek(fout, 40, SEEK_SET);
    uint32_t data_size = file_size - 44;
    fwrite(&data_size, sizeof(data_size), 1, fout);

    fseek(fout, 0, SEEK_END);
}

// 合成输出回调函数
void XTTSClient::OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output) {
    if ((output->node->value) && (instance_->fin != nullptr)) {
        fwrite(output->node->value, sizeof(char), output->node->len, instance_->fin);
        UpdateWAVHeader(instance_->fin);

        // 返回音频数据
        instance_->audioData.insert(instance_->audioData.end(), (char*)output->node->value, (char*)output->node->value + output->node->len);
    }
}

// 事件回调函数
void XTTSClient::OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue) {
    if (eventType == AIKIT_Event_End) {
        {
            std::lock_guard<std::mutex> lock(instance_->cvMutex);
            instance_->ttsFinished = true;
        }
        instance_->cv.notify_all();
    }
}

// 错误回调函数
void XTTSClient::OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc) {
    printf("OnError:%d\n", err);
}

// 获取音频数据
std::vector<char> XTTSClient::getAudioData() const {
    return audioData;
}

// 从本地读取WAV文件数据
std::vector<char> XTTSClient::readLocalWavFile(const std::string& filePath) const {
    std::ifstream file(filePath, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}