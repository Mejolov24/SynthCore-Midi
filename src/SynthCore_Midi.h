#ifndef SC_MIDI_
    #define SC_MIDI_H
#include <stdint.h>
#include <SynthCore.h>
#include <MidiType.h>
#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 256
#endif


class SynthCoreMidi{
    public:
        struct ChannelOverride{
            bool instrument_override = false;
            bool sustain_override = false;
            bool vibrato_frequency_override = false;
            bool vibrato_range_override = false;
            bool bend_override = false;
            bool volume_override = false;
        };

    private:
        SynthCore* _synthcore = nullptr;
        #define BUFFER_SIZE 256
        int16_t _BufferA[BUFFER_SIZE];
        int16_t _BufferB[BUFFER_SIZE];
        bool _buffer_index = 0;

        const SampleData* instruments = nullptr;
        const SampleData* percussion = nullptr;

        uint8_t getSIDorFallback(uint8_t SID,bool is_percussion);

        SynthCore::ChannelParameters channels_parameters[16];
        uint8_t channels_sid[16];
        ChannelOverride channels_overrides[16];
    public:
        void setup(SynthCore *synthcore);
        void ProcessMidi(MidiMessage msg);
        void SetChannnelOverrides(uint8_t channel, ChannelOverride overrides);
        void SetChannelParameters(bool override, uint8_t channel, SynthCore::ChannelParameters parameters);
        void SetChannelInstrument(bool override, uint8_t channel, uint8_t instrument);
        void setSamplePointers(const SampleData* inst, const SampleData* perc);
        void updateAudioBuffer();
        int16_t* getAudioBuffer();
        int16_t channel_TX_buffers[16][BUFFER_SIZE];
        ChannelOverride GetChannelOverrides(uint8_t channel);
        SynthCore::ChannelParameters GetChannelParameters(uint8_t channel);
        uint8_t getChannelSID(uint8_t channel);


};
#endif
