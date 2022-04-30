/*!
  TinyDarius

  @file sound.cpp
  @brief Sound system
*/
#include "sound.hpp"
#include "debug.hpp"
#include "app.hpp"
#include <gob_utility.hpp>
#include <gob_m5s_stream.hpp>
#include <gob_m5s_speaker.hpp>
#include <cassert>

namespace
{
constexpr const char* BGM_PATH[BGM::Max] =
{
    "/res/td/wav/IC.wav",
    "/res/td/wav/ICB.wav",
    //
    "/res/td/wav/CN.wav",
    "/res/td/wav/CMT.wav",
    "/res/td/wav/CAW.wav",
    "/res/td/wav/IB.wav",
    "/res/td/wav/TS.wav",
    "/res/td/wav/W!.wav",
    "/res/td/wav/B1.wav",
    "/res/td/wav/B2.wav",
    "/res/td/wav/B3.wav",
    "/res/td/wav/B4.wav",
    "/res/td/wav/B5.wav",
    "/res/td/wav/B6.wav",
    "/res/td/wav/B7.wav",
    "/res/td/wav/RC.wav",
    //
    "/res/td/wav/ED.WAv",
    "/res/td/wav/NAME.WAV",
    "/res/td/wav/OVER.WAV",
};
}

using goblib::PcmStream;

SoundSystem::SoundSystem()
        : _speaker(nullptr)
        , _bgmStream(nullptr)
        , _creditStream(nullptr)
        , _bgmPcm(nullptr)
        , _creditPcm(nullptr)
        , _pool(SIMULTANEOUS_PLAYS - 1) // exclude bgm
        , _bgm(BGM::Max) // initialize by invalid value
{
    _speaker = new goblib::m5s::Speaker(SIMULTANEOUS_PLAYS);
    assert(_speaker);
    //setVolume(INITIAL_VOLUME);
    setVolume(0.5f);
    _speaker->setStreamingCallback(callbackStreaming, this);
    _speaker->begin();

    _bgmStream = new goblib::m5s::FileStream();
    assert(_bgmStream);
    _creditStream = new goblib::m5s::FileStream();
    assert(_creditStream);

    _bgmPcm = new PcmStream(_bgmStream);
    assert(_bgmPcm);
    _creditPcm = new PcmStream(_creditStream);
    assert(_creditPcm);
}

SoundSystem::~SoundSystem()
{
    stop();
    goblib::safeDelete(_creditPcm);
    goblib::safeDelete(_bgmPcm);
    goblib::safeDelete(_creditStream);
    goblib::safeDelete(_bgmStream);
    goblib::safeDelete(_speaker);
}


void SoundSystem::setupGame()
{
}
    
void SoundSystem::finalizeGame()
{
}

float SoundSystem::volume() const
{
    return _speaker->getVolume() / 255.0f;
}

void SoundSystem::setVolume(const float vol)
{
    _speaker->setVolume(static_cast<std::uint8_t>(255 * goblib::clamp(vol, 0.0f, 1.0f)));
}

std::size_t SoundSystem::playable() const
{
    return SIMULTANEOUS_PLAYS - _speaker->getPlayingChannels();
}

void SoundSystem::pump()
{
    SCOPED_RELEASE_BUS();
    _speaker->pump();
}

void SoundSystem::stop()
{
    stopBgm();
}

bool SoundSystem::playBgm(const BGM bgm, bool loop)
{
    if(bgm >= BGM::Max) { return false; }
    if(_bgm == bgm) { return true; }

    _speaker->stop(BGM_CHANNEL);

    SCOPED_RELEASE_BUS();
    if(_bgmStream->open(BGM_PATH[bgm]))
    {
        _bgmPcm->assign(_bgmStream);
        if(_bgmPcm->valid() && _speaker->play(*_bgmPcm, loop ? -1 : 1, BGM_CHANNEL))
        {
            _bgm = bgm;
            return true;
        }
    }
    return false;
}
    
void SoundSystem::stopBgm()
{
    _speaker->stop(BGM_CHANNEL);
    _bgm = BGM::Max;
}

bool SoundSystem::playSfx(const SFX sfx)
{
    // FileStream
    if(sfx == SFX::InsertCoin || sfx == SFX::InsertCoinB)
    {
        SCOPED_RELEASE_BUS();
        if(!_creditPcm->valid())
        {
            _creditStream->open(BGM_PATH[BGM::InsertCoin]);
            _creditPcm->assign(_creditStream);
        }
        return play(*_creditPcm);
    }
    return false;
}


// play
bool SoundSystem::play(goblib::Stream& s)
{
    SCOPED_RELEASE_BUS();
    PcmStream* ps = _pool.construct(&s);
    if(!ps){ return false; }

    ps->assign(&s);
    return play(*ps);
}

bool SoundSystem::play(PcmStream& ps)
{
    if(!ps.valid()) { return false; }

    for(int ch = 0; ch < SIMULTANEOUS_PLAYS; ++ch)
    {
        if(ch != BGM_CHANNEL && !_speaker->isPlaying(ch))
        {
            return _speaker->play(ps, ch);
        }
    }
    return false;
}

void SoundSystem::_callbackStreaming(std::uint8_t ch, bool start)
{
    // End of streaming bgm.
    if(!start && ch == BGM_CHANNEL)
    {
        _bgm = BGM::Max;
    }
}
