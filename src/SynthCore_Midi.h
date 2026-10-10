#ifndef SC_MIDI_H
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
        struct MidiChannelParameters{
            uint8_t SampleID = 0;
            Q_TYPE max_lfo_semitone_range = 2;
            float max_lfo_herz = 10;

            bool split_to_channels = false;

            bool pitch_slide = false;
            uint16_t pitch_slide_ms = 50;
            uint8_t start_pitch = 0;
            uint8_t end_pitch = 0;

            SynthCore::ChannelParameters parameters;
            ChannelOverride override_parameters;
        };

    private:
        enum class CommandType{
            NOTE_ON,
            NOTE_OFF,
            DRUM_ON,
            PITCH_SLIDE,
            RETRIGGER_SLIDE
        };

        SynthCore* _synthcore = nullptr;
        #define BUFFER_SIZE 256
        int16_t _BufferA[BUFFER_SIZE];
        int16_t _BufferB[BUFFER_SIZE];
        bool _buffer_index = 0;

        const SampleData* instruments = nullptr;
        const SampleData* percussion = nullptr;

        uint8_t getSIDorFallback(uint8_t SID,bool is_percussion);

        MidiChannelParameters channel_parameters[16];
        uint8_t getUnactiveChannel(uint8_t channel);
        uint8_t note_channel[MAX_CHANNELS][128];
        uint8_t slide_vid[MAX_CHANNELS];
    public:
        void setup(SynthCore *synthcore);
        void ProcessMidi(MidiMessage msg);
        void SetChannelParameters(bool is_override, uint8_t channel, MidiChannelParameters parameters);
        MidiChannelParameters GetChannelParameters(uint8_t channel);
        void setSamplePointers(const SampleData* inst, const SampleData* perc);
        void updateAudioBuffer();
        int16_t* getAudioBuffer();
        int16_t channel_TX_buffers[16][BUFFER_SIZE];



};
#endif
