#ifndef SOUND_H
#define SOUND_H

// audio setup
#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio.h"
#include <cmath>
#include <iostream>

#define SAMPLE_RATE 44100
#define FREQUENCY 440.0
#define AMPLITUDE 0.5

struct SoundData {
    double phase;
    bool isPlaying;
};

class Sound {
public:
    ma_device device;
    SoundData soundData = {0, false};

    static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
        SoundData* data = (SoundData*)device->pUserData;
        float* out = (float*)output;

        for (ma_uint32 i = 0; i < frameCount; i++) {
            float sample = data->isPlaying ? AMPLITUDE * sin(2.0 * M_PI * FREQUENCY * data->phase / SAMPLE_RATE) : 0.0f;
            *out++ = sample;
            *out++ = sample;
            data->phase += 1.0;
            if (data->phase >= SAMPLE_RATE) data->phase -= SAMPLE_RATE;
        }
    }

    Sound() {
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = ma_format_f32;
        config.playback.channels = 2;
        config.sampleRate = SAMPLE_RATE;
        config.pUserData = &soundData;
        config.dataCallback = data_callback;

        if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to initialize audio device");
        }
        ma_device_start(&device);
    }

    ~Sound() {
        ma_device_uninit(&device);
    }

    void play(bool enable) {
        soundData.isPlaying = enable;
    }
};

#endif //SOUND_H
