#ifndef MIDI_T_H
    #define MIDI_T_H
    #include <stdint.h>

    struct MidiMessage {
        uint8_t type;
        uint8_t channel;
        uint8_t data1;
        uint8_t data2;

        int16_t getPitchBend() const {
            if (type == 0xE0) {
                return (int16_t)((data2 << 7) | data1) - 8192; 
            }
            return 0;
        }
    };

    enum MidiType : uint8_t {
        NoteOff              = 0x80,
        NoteOn               = 0x90,
        Aftertouch           = 0xA0,
        ControlChange        = 0xB0,
        ProgramChange        = 0xC0,
        ChannelPressure      = 0xD0,
        PitchBend            = 0xE0,
        SystemExclusive      = 0xF0,
        TimeCodeQuarterFrame = 0xF1,
        SongPosition         = 0xF2,
        SongSelect           = 0xF3,
        TuneRequest          = 0xF6,
        EndOfSystemExclusive = 0xF7,
        TimingClock          = 0xF8,
        Start                = 0xFA,
        Continue             = 0xFB,
        Stop                 = 0xFC,
        ActiveSensing        = 0xFE,
        SystemReset          = 0xFF,
        Unknown              = 0x00
    };

    enum MidiControlChange : uint8_t {
        // MSB Controllers (0-31)
        BankSelectMSB				= 0,
        ModulationWheelMSB			= 1,
        BreathControllerMSB			= 2,
        Undefined3MSB				= 3,
        FootControllerMSB			= 4,
        PortamentoTimeMSB			= 5,
        DataEntryMSB				= 6,
        ChannelVolumeMSB			= 7,
        BalanceMSB					= 8,
        Undefined9MSB				= 9,
        PanMSB						= 10,
        ExpressionControllerMSB		= 11,
        EffectControl1MSB			= 12,
        EffectControl2MSB			= 13,
        Undefined14MSB				= 14,
        Undefined15MSB				= 15,
        GeneralPurposeController1MSB	= 16,
        GeneralPurposeController2MSB	= 17,
        GeneralPurposeController3MSB	= 18,
        GeneralPurposeController4MSB	= 19,
        Undefined20MSB				= 20,
        Undefined21MSB				= 21,
        Undefined22MSB				= 22,
        Undefined23MSB				= 23,
        Undefined24MSB				= 24,
        Undefined25MSB				= 25,
        Undefined26MSB				= 26,
        Undefined27MSB				= 27,
        Undefined28MSB				= 28,
        Undefined29MSB				= 29,
        Undefined30MSB				= 30,
        Undefined31MSB				= 31,

        // LSB Controllers (32-63)
        BankSelectLSB				= 32,
        ModulationWheelLSB			= 33,
        BreathControllerLSB			= 34,
        Undefined3LSB				= 35,
        FootControllerLSB			= 36,
        PortamentoTimeLSB			= 37,
        DataEntryLSB				= 38,
        ChannelVolumeLSB			= 39,
        BalanceLSB					= 40,
        Undefined9LSB				= 41,
        PanLSB						= 42,
        ExpressionControllerLSB		= 43,
        EffectControl1LSB			= 44,
        EffectControl2LSB			= 45,
        Undefined14LSB				= 46,
        Undefined15LSB				= 47,
        GeneralPurposeController1LSB	= 48,
        GeneralPurposeController2LSB	= 49,
        GeneralPurposeController3LSB	= 50,
        GeneralPurposeController4LSB	= 51,
        Undefined20LSB				= 52,
        Undefined21LSB				= 53,
        Undefined22LSB				= 54,
        Undefined23LSB				= 55,
        Undefined24LSB				= 56,
        Undefined25LSB				= 57,
        Undefined26LSB				= 58,
        Undefined27LSB				= 59,
        Undefined28LSB				= 60,
        Undefined29LSB				= 61,
        Undefined30LSB				= 62,
        Undefined31LSB				= 63,

        // Switches, Sound Controllers, Effects (64-95)
        DamperPedal					= 64,
        PortamentoOnOff				= 65,
        Sostenuto					= 66,
        SoftPedalOnOff				= 67,
        LegatoFootswitch				= 68,
        Hold2						= 69,
        SoundController1				= 70,
        SoundController2				= 71,
        SoundController3				= 72,
        SoundController4				= 73,
        SoundController5				= 74,
        SoundController6				= 75,
        SoundController7				= 76,
        SoundController8				= 77,
        SoundController9				= 78,
        SoundController10				= 79,
        GeneralPurposeController5		= 80,
        GeneralPurposeController6		= 81,
        GeneralPurposeController7		= 82,
        GeneralPurposeController8		= 83,
        PortamentoControl				= 84,
        Undefined85					= 85,
        Undefined86					= 86,
        Undefined87					= 87,
        HighResolutionVelocityPrefix	= 88,
        Undefined89					= 89,
        Undefined90					= 90,
        Effects1Depth					= 91,
        Effects2Depth					= 92,
        Effects3Depth					= 93,
        Effects4Depth					= 94,
        Effects5Depth					= 95,

        // Data / Parameter Controllers (96-119)
        DataIncrement					= 96,
        DataDecrement					= 97,
        NRPNLSB						= 98,
        NRPNMSB						= 99,
        RPNLSB						= 100,
        RPNMSB						= 101,
        Undefined102					= 102,
        Undefined103					= 103,
        Undefined104					= 104,
        Undefined105					= 105,
        Undefined106					= 106,
        Undefined107					= 107,
        Undefined108					= 108,
        Undefined109					= 109,
        Undefined110					= 110,
        Undefined111					= 111,
        Undefined112					= 112,
        Undefined113					= 113,
        Undefined114					= 114,
        Undefined115					= 115,
        Undefined116					= 116,
        Undefined117					= 117,
        Undefined118					= 118,
        Undefined119					= 119,

        // Channel Mode Messages (120-127)
        AllSoundOff					= 120,
        ResetAllControllers			= 121,
        LocalControlOnOff				= 122,
        AllNotesOff					= 123,
        OmniModeOff					= 124,
        OmniModeOn					= 125,
        MonoModeOn					= 126,
        PolyModeOn					= 127
    };

#endif