#ifndef XTTSCLIENT_H
#define XTTSCLIENT_H

#include <fstream>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <nlohmann/json.hpp>
#include "aikit_biz_api.h"
#include "aikit_constant.h"
#include "aikit_biz_config.h"

using json = nlohmann::json;

struct WAVHeader {
    char riff_header[4];
    uint32_t wav_size;
    char wave_header[4];
    char fmt_header[4];
    uint32_t fmt_chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t sample_alignment;
    uint16_t bit_depth;
    char data_header[4];
    uint32_t data_bytes;
};


class XTTSClient {
public:
    XTTSClient();
    ~XTTSClient();

    void createXTTS(const json& config);
    std::vector<char> getAudioData() const;
    std::vector<char> readLocalWavFile(const std::string& filePath) const;
    json readConfig(const std::string& filename);

private:
    static void WriteWAVHeader(FILE *fout, int sampleRate, int bitsPerSample, int channels);
    static void UpdateWAVHeader(FILE *fout);
    static void OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output);
    static void OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue);
    static void OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc);

    std::atomic_bool ttsFinished;
    std::vector<char> audioData;
    FILE* fin;
    static constexpr const char* ABILITY = "e2e44feff";
    std::mutex mutex_;
    std::mutex cvMutex;
    std::condition_variable cv;

    static XTTSClient* instance_;
};

#endif // XTTSCLIENT_H
