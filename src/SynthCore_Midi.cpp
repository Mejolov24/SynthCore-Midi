#include <SynthCore_Midi.h>

void SynthCoreMidi::setup(SynthCore *synthcore){
    _synthcore = synthcore;
}

int16_t* SynthCoreMidi::getAudioBuffer(){
  if (!_buffer_index) return _BufferB; 
  else return _BufferA;
}

void SynthCoreMidi::updateAudioBuffer(){
  int16_t* _current_buffer;
  if (!_buffer_index){_current_buffer = _BufferA;}
  else {_current_buffer = _BufferB;}

  for (int i = 0; i < BUFFER_SIZE; i++){
    _synthcore->stepAudio();
    _current_buffer[i] = _synthcore->master_mix;
    for(uint16_t ch = 0; ch < 16; ch++){channel_TX_buffers[ch][i] = _synthcore->channel_output[ch];}
  }
  _buffer_index = !_buffer_index;
}


uint8_t SynthCoreMidi::getSIDorFallback(uint8_t SID,bool is_percussion){
    const SampleData* current_array = instruments;
    if (is_percussion) current_array = percussion;
    if (SID > 127) SID = 0;
    if ((current_array + SID)->length != 0) return SID;
    
    uint8_t categoryStart = (SID / 8) * 8;
    uint8_t categoryEnd = categoryStart + 7;

    for (uint8_t i = categoryStart; i <= categoryEnd; i++){
        if (current_array[i].length != 0) return i;
    }

    for (uint8_t i = 0; i < 128; i++){
        if (current_array[i].length != 0) return i;
    }
    return 0;
}

void SynthCoreMidi::ProcessMidi(MidiMessage msg) {
    SynthCore::ChannelParameters params = channels_parameters[msg.channel];
    Q_TYPE Q_volume = Q_TYPE(static_cast<float>(msg.data2) / 127.0f);
    bool is_percussion (msg.channel == 9);
    MidiType message = static_cast<MidiType>(msg.type);
    MidiType control = static_cast<MidiControlChange>(msg.data1);
    switch (message) {
        
        case MidiType::NoteOn:
            if (msg.data2 > 0) {
                if (!is_percussion){
                    _synthcore->createVoice(instruments + channels_sid[msg.channel],msg.data1,Q_volume,msg.channel,false);
                }
                else {_synthcore->createVoice(percussion + getSIDorFallback(msg.data1,true),msg.data1,Q_volume,msg.channel,true);}
            } else {
                _synthcore->releaseVoiceByNote(msg.data1,msg.channel);
            }
            break;
        case MidiType::NoteOff:
                    _synthcore->releaseVoiceByNote(msg.data1, msg.channel);
                    break;
        case MidiType::ProgramChange:
                    SetChannelInstrument(false,msg.channel,msg.data1);
                    break;

        case MidiType::ControlChange: {
            switch (control)
                {
                case MidiControlChange::DamperPedal:
                    params.sustain = (msg.data2 >= 64);
                    break;
                
                case MidiControlChange::ChannelVolumeMSB:
                    params.volume = Q_volume;
                    break;
                
                default:
                    break;
                }
                break;
            }

        case MidiType::PitchBend:{
            int16_t offset = msg.getPitchBend(); 
            params.pitch_bend = Q_TYPE(static_cast<float>(offset) / 8192.0f * 2.0f);
            break;
        }

        default:
            break;
        }
        SetChannelParameters(false,msg.channel,params);
}


// checks if override matches the channel override values, then applies changes.
// override is true when changing from ui, and false from incoming midi data
void SynthCoreMidi::SetChannelParameters(bool override, uint8_t channel, SynthCore::ChannelParameters parameters){
    ChannelOverride ch_override = channels_overrides[channel];
    if (override == ch_override.sustain_override) channels_parameters[channel].sustain = parameters.sustain;
    if (override == ch_override.vibrato_frequency_override) channels_parameters[channel].vibrato_frequency = parameters.vibrato_frequency;
    if (override == ch_override.vibrato_range_override) channels_parameters[channel].vibrato_range = parameters.vibrato_range;
    channels_parameters[channel].pitch_bend = parameters.pitch_bend;
    if (override == ch_override.volume_override) channels_parameters[channel].volume = parameters.volume;
    _synthcore->setChannelParameters(channel,channels_parameters[channel]);
}

void SynthCoreMidi::SetChannelInstrument(bool override, uint8_t channel, uint8_t instrument){
    bool is_percussion = (channel == 9);
    ChannelOverride ch_override = channels_overrides[channel];
    if (override == ch_override.instrument_override) channels_sid[channel] = getSIDorFallback(instrument,is_percussion);
}



void SynthCoreMidi::SetChannnelOverrides(uint8_t channel, ChannelOverride overrides){
  channels_overrides[channel] = overrides;
}

SynthCoreMidi::ChannelOverride SynthCoreMidi::GetChannelOverrides(uint8_t channel){
  return channels_overrides[channel];
}

SynthCore::ChannelParameters SynthCoreMidi::GetChannelParameters(uint8_t channel){
  return _synthcore->getChannelParameters(channel);
}

void SynthCoreMidi::setSamplePointers(const SampleData* inst, const SampleData* perc) {
    instruments = inst;
    percussion = perc;
}
uint8_t SynthCoreMidi::getChannelSID(uint8_t channel){
    return channels_sid[channel];
}