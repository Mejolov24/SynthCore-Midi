#include <SynthCore_Midi.h>

void SynthCoreMidi::setup(SynthCore *synthcore){
    _synthcore = synthcore;
	for(uint8_t channel = 0; channel < MAX_CHANNELS; channel++){
		for(uint8_t note = 0; note < 128; note++){
			note_channel[channel][note] = 255;
		}
	}
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

uint8_t SynthCoreMidi::getUnactiveChannel(uint8_t channel){
    uint8_t _channel = channel;
    uint8_t start_channel = channel;
    while(_synthcore->isChannelActive(channel) and channel_parameters[channel].split_to_channels){
        channel ++;
        if(channel >= MAX_CHANNELS) {return start_channel;}
    }
    return channel;
}

void SynthCoreMidi::ProcessMidi(MidiMessage msg) {
    MidiChannelParameters parameters = channel_parameters[msg.channel];
    Q_TYPE Q_volume = Q_TYPE(static_cast<float>(msg.data2) / 127.0f);
    bool is_percussion (msg.channel == 9);
    MidiType message = static_cast<MidiType>(msg.type);
    MidiControlChange control = static_cast<MidiControlChange>(msg.data1);
    switch (message) {
        
        case MidiType::NoteOn: // omg this monolitic ahh needs to be refactored
            if (msg.data2 < 0) {
                if (!is_percussion){
                    //splitting
                    uint8_t channel = msg.channel;
                    if(parameters.split_to_channels){channel = getUnactiveChannel(channel);}
                    
                    // pitch slide
                    if(channel_parameters[channel].pitch_slide){
                        uint8_t latest_voice_id = _synthcore->isChannelActive(channel);

                        if(latest_voice_id != 255){ // check if we have a note
                            SynthCore::Voice latest_voice = _synthcore->getVoiceInfo(latest_voice_id);
                            if(latest_voice.note != msg.data1){ // check if its the same note
                                _synthcore->slideVoice(latest_voice_id, channel, msg.data1, parameters.pitch_slide_ms);
                                }
                            else{
                            // Same note, retrigger it
                                parameters.start_pitch = msg.data1;
                                _synthcore->releaseVoice(latest_voice_id);
                                _synthcore->createVoice(instruments + channel_parameters[channel].SampleID, msg.data1, Q_volume, channel, false);
                            }
                        }
                        else{
                            if(latest_voice_id != 255){
                                _synthcore->releaseVoice(latest_voice_id);
                            }
                            parameters.start_pitch = msg.data1;
                            _synthcore->releaseVoice(latest_voice_id);
                            _synthcore->createVoice(instruments + channel_parameters[channel].SampleID,msg.data1,Q_volume,channel,false);
                        }
                    }
                    else{
                        parameters.start_pitch = msg.data1;
                        _synthcore->createVoice(instruments + channel_parameters[channel].SampleID,msg.data1,Q_volume,channel,false);
                    }
                    note_channel[msg.channel][msg.data1] = channel;
                }
                else {_synthcore->createVoice(percussion + getSIDorFallback(msg.data1,true),msg.data1,Q_volume,msg.channel,true);}
            } else {
                    uint8_t channel = msg.channel;
                    if(parameters.split_to_channels){channel = note_channel[msg.channel][msg.data1];}
                    if(channel != 255){
                        uint8_t vid = _synthcore->getVIDByChannel(msg.data1,channel);
                        _synthcore->releaseVoice(vid);
                        note_channel[msg.channel][msg.data1] = 255;
                    }
            }
            break;
        
        case MidiType::NoteOff:{
                    uint8_t channel = msg.channel;
                    if(parameters.split_to_channels){channel = note_channel[msg.channel][msg.data1];}
                    if(channel != 255){
                        if(channel_parameters[channel].pitch_slide){break;}
                        uint8_t vid = _synthcore->getVIDByChannel(msg.data1,channel);
                        _synthcore->releaseVoice(vid);
                        note_channel[msg.channel][msg.data1] = 255;
                    }
                    break;
                }
        case MidiType::ProgramChange:
                    if(parameters.override_parameters.instrument_override) break;
                    parameters.SampleID = msg.data1;
                    break;

        case MidiType::ControlChange: {
            switch (control)
                {
                case MidiControlChange::DamperPedal:
                    if(parameters.override_parameters.sustain_override) break;
                    parameters.parameters.sustain = (msg.data2 >= 64);
                    break;
                
                case MidiControlChange::ChannelVolumeMSB:
                    if(parameters.override_parameters.volume_override) break;
                    parameters.parameters.volume = Q_volume;
                    break;
                case MidiControlChange::SoundController7: //vibrato rate
                    if(parameters.override_parameters.vibrato_frequency_override) break;
                    parameters.parameters.vibrato_frequency = static_cast<float>(msg.data2) / 127.0f * parameters.max_lfo_herz;
                    break;
                case MidiControlChange::SoundController8:
                    if(parameters.override_parameters.vibrato_range_override) break;
                    parameters.parameters.vibrato_range = Q_TYPE(static_cast<float>(msg.data2) / 127.0f) * parameters.max_lfo_semitone_range;
                    break;
                
                default:
                    break;
                }
                break;
            }

        case MidiType::PitchBend:{
            int16_t offset = msg.getPitchBend(); 
            parameters.parameters.pitch_bend = Q_TYPE(static_cast<float>(offset) / 8192.0f * 2.0f);
            break;
        }

        default:
            break;
        }
        SetChannelParameters(false,msg.channel,parameters);
}



void SynthCoreMidi::SetChannelParameters(bool is_override, uint8_t channel, MidiChannelParameters parameters){
    parameters.SampleID = getSIDorFallback(parameters.SampleID,(channel == 9));
    channel_parameters[channel] = parameters;
    _synthcore->setChannelParameters(channel, parameters.parameters);
}

SynthCoreMidi::MidiChannelParameters SynthCoreMidi::GetChannelParameters(uint8_t channel){
    return channel_parameters[channel];
}

void SynthCoreMidi::setSamplePointers(const SampleData* inst, const SampleData* perc) {
    instruments = inst;
    percussion = perc;
}