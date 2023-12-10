//
// Created by kris on 27/04/2022.
//

#ifndef ESPSTREAMER_STREAMDECODER_HPP
#define ESPSTREAMER_STREAMDECODER_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "mad.h"
#include "ICETask.hpp"
#include "I2SOutput.hpp"


#define DECODERRINGBUFFERSIZE 9216  //Larger buffer will mean that we can tolerate more difficult network conditions, but will also adversely impact the mp3 decoder if we buffer too much data.
#define DECODERSTATICBUFFERSIZE 4608 //256 48khz stream will have frame size of 144 * 256 / (48 + 0) = 768 bytes, we would like 6 times this to guarantee some frames for MAD when we refill.

class StreamDecoder : public ICETask {
public:
    enum AudioType {
        MP3 = 0,
        AAC = 1, //Not implemented.
        FLAC = 2 //Not implemented.
    };

private:
    uint8_t* streamBuffer = nullptr;
    RingbufHandle_t inputBuffer = nullptr;
    mad_stream* madStream = nullptr;
    mad_frame* madFrame = nullptr;
    mad_synth* madSynth = nullptr;
    AudioType type = AudioType::MP3;

    [[noreturn]] void decodeMP3();
    void fillMadStream(mad_stream* stream);
public:
    StreamDecoder(I2SOutput* i2sOutput);
    ~StreamDecoder();
    void setAudioType(AudioType type);
    void runTask(void* unused) override;

    void QueueData(uint8_t* data, uint32_t size);
};

#endif //ESPSTREAMER_STREAMDECODER_HPP
