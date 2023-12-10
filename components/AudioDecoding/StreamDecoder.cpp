//
// Created by kris on 27/04/2022.
//

#include <cstring>
#include "StreamDecoder.hpp"
#include "QPrint.hpp"
#include "ICESleep.hpp"
#include "mad.h"



uint32_t sampleRate = 0; //Variable that saves current MP3 samplerate
int sampleRateAdjustment = 0; //Variable that saves the adjustment to the samplerate (for clock sync)

uint32_t minimumAdjustTicks = 100; //Amount of ticks between adjustment of samplerate
uint32_t lastAdjustment = 0;        //Holds the last adjustment to samplerate so we don't overadjust.

uint32_t lastSampleRate = 0; //Used for making sure we don't set samplerate every time we process a synth.

//For the libmad C callbacks. This should probably be changed for something better in the future...
I2SOutput* i2s = nullptr;
void registerI2SOutput(I2SOutput* i2sOutput) {
    i2s = i2sOutput;
}

StreamDecoder::StreamDecoder(I2SOutput* i2sOutput) {
    registerI2SOutput(i2sOutput);
    streamBuffer = (uint8_t*) malloc(DECODERSTATICBUFFERSIZE);
    inputBuffer = xRingbufferCreate(DECODERRINGBUFFERSIZE, RINGBUF_TYPE_BYTEBUF);
}

StreamDecoder::~StreamDecoder() {
    vRingbufferDelete(inputBuffer);
    free(streamBuffer);
    delete(madStream);
    delete(madFrame);
    delete(madSynth);
}

void StreamDecoder::setAudioType(AudioType type) {
    this->type = type;
}

void StreamDecoder::QueueData(uint8_t *data, uint32_t size) {
    if (xRingbufferSend(inputBuffer, data, size, 0) != pdTRUE) {
        QPrint::println("No space in StreamDecoder Ringbuffer!");
        if (xTaskGetTickCount() > lastAdjustment + minimumAdjustTicks) {
            QPrint::println("StreamDecoder has no space. Adjusting samplerate +5. Now: " + std::to_string(sampleRate + sampleRateAdjustment));
            sampleRateAdjustment += 5;
            lastAdjustment = xTaskGetTickCount();
        }
    }
}

void StreamDecoder::runTask(void *unused) {
        if (type == AudioType::MP3) {
            decodeMP3();
        }
}


[[noreturn]] void StreamDecoder::decodeMP3() {
    madStream = new mad_stream;
    madFrame = new mad_frame;
    madSynth = new mad_synth;

    mad_stream_init(madStream);
    mad_frame_init(madFrame);
    mad_synth_init(madSynth);

    fillMadStream(madStream);

    while(1) {
        //QPrint::println("starting while");
        int success = mad_frame_decode(madFrame, madStream);
        if (success == 0) { //0 means everthing is okay.
            mad_synth_frame(madSynth, madFrame);
        } else {
            if (!MAD_RECOVERABLE(madStream->error)) {
                //QPrint::println("mad error, refill!");
                //Likely the non-recoverable error is due to no more data, so we fill buffer again
                fillMadStream(madStream);
            }
        }
    }
}


void StreamDecoder::fillMadStream(mad_stream* stream) {
    if (streamBuffer  == nullptr) {
        return;
    }
    int leftover = stream->bufend - stream->next_frame;
    memcpy(streamBuffer, stream->next_frame, leftover); //Copy leftover to front
    //memset(streamBuffer + leftover, 0, DECODERSTATICBUFFERSIZE - leftover);

    //Copy new data in
    size_t readBytes = 0;
    
    void* received = xRingbufferReceiveUpTo(inputBuffer, &readBytes, 0, DECODERSTATICBUFFERSIZE - leftover); //Fill stream buffer as much as possible

    //QPrint::println("Received " + std::to_string(readBytes) + " bytes from ringbuf");
    if (readBytes > 0) {
        memcpy(streamBuffer + leftover, received, readBytes);
        vRingbufferReturnItem(inputBuffer, received);
    } else {
        QPrint::println("No data in ringbuf!");

        if (xTaskGetTickCount() > lastAdjustment + minimumAdjustTicks) {
            sampleRateAdjustment -= 5; //slower samplerate
            lastAdjustment = xTaskGetTickCount();
            QPrint::println("Sample rate adjustment: " + std::to_string(sampleRateAdjustment));
            QPrint::println("Sample rate is now: " + std::to_string(sampleRate + sampleRateAdjustment));
            if (i2s != nullptr) {
                i2s->setSampleRate(sampleRate + sampleRateAdjustment);
            }
            lastSampleRate = sampleRate + sampleRateAdjustment;
        }
        
        ICESleep(200); //No data available, give some time for lwip to cache...
    }

    mad_stream_buffer(stream, (unsigned char *) streamBuffer, leftover + readBytes);
}

/*//These are callbacks from the synth functionality with libmad
extern "C" void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels) {
    //We just about have time for copying here, but nothing else really..
    void* channel0 = malloc(num_samples * sizeof(short));
    memcpy(channel0, sample_buff_ch0, num_samples * sizeof(short));

    if (num_channels > 1) {
        void* channel1 = malloc(num_samples * sizeof(short));
        memcpy(channel1, sample_buff_ch0, num_samples * sizeof(short));
    }

    streamReceiver->queue(sample_buff_ch0, sample_buff_ch1, num_samples, num_channels);

    for (int i = 0; i < num_samples; i++) {
        streamReceiver->i2s->writeDirect((uint8_t*)&sample_buff_ch0[i], 2);
        streamReceiver->i2s->writeDirect((uint8_t*)&sample_buff_ch1[i], 2);
    }

}*/


extern "C" void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels) {
    //uint32_t ticks = 0;
    if (i2s == nullptr) {
        return;
    }

    if (sampleRate + sampleRateAdjustment != lastSampleRate) {
        lastSampleRate = sampleRate + sampleRateAdjustment;
        i2s->setSampleRate(sampleRate + sampleRateAdjustment);
    }

    bool tooFast = false;

    if (num_channels > 1) { //TODO: Maybe handle more than stereo?
        uint32_t* interleaved_buffer = (uint32_t*) malloc(num_samples * sizeof(uint32_t));
        if (interleaved_buffer == nullptr) {
            return; //Bail if we do not get memory for some reason
        }
        for (int i = 0; i < num_samples; i++) {
            interleaved_buffer[i] = ((uint32_t)sample_buff_ch0[i] << 16) | (0x0000ffff & sample_buff_ch1[i]);
        }
        //ticks = xTaskGetTickCount();
        tooFast = i2s->writeDirect((uint8_t*) interleaved_buffer, num_samples * sizeof(uint32_t));
        free(interleaved_buffer);
    } else {
        //ticks = xTaskGetTickCount();
        tooFast = i2s->writeDirect((uint8_t*) sample_buff_ch0, num_samples * sizeof(short));
    }

    if (tooFast == false) {
        QPrint::println("I2S has no space. Adjusting samplerate +25. Now: " + std::to_string(sampleRate + sampleRateAdjustment));
        sampleRateAdjustment += 25;
    }

    /*if (xTaskGetTickCount() - ticks > 0) {
        QPrint::println("Ticks from queue to finish queue: " + std::to_string(xTaskGetTickCount() - ticks));
    }*/
}

extern "C" void set_dac_sample_rate(int rate)
{
    sampleRate = rate;
}