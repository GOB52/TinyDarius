/*!
  TinyDarius

  @file sound.hpp
  @brief Sound system
*/
#pragma once
#ifndef TD_SOUND_HPP
#define TD_SOUND_HPP


#include "constants.hpp"
#include <gob_macro.hpp>
#include <gob_singleton.hpp>
#include <gob_pcm_stream.hpp>
#include <gob_object_pool.hpp>

namespace goblib
{
class Stream;
class PcmStream;
namespace m5s
{
class FileStream;
class Speaker;
}
}

class SoundSystem : public goblib::Singleton<SoundSystem>
{
  public:
    constexpr static std::size_t SIMULTANEOUS_PLAYS = 6; // bgm + 5 se
    constexpr static float INITIAL_VOLUME = 0.30f;
    constexpr static int BGM_CHANNEL = 0; // BGM occupies channel
    constexpr static std::uint8_t SE_CHANNEL_BIt = ((1 << SIMULTANEOUS_PLAYS) -1) & ~(1 << BGM_CHANNEL); // SE occupies channel bit

    virtual ~SoundSystem();

    void setupGame();
    void finalizeGame();
    
    float volume() const;
    void setVolume(const float vol);

    void pump();
    
    // stop all.
    void stop();

    // BGM
    bool playBgm(const BGM bgm, bool loop = true);
    void stopBgm();

    // SFX
    bool playSfx(const SFX sfx);
    
    std::size_t playable() const;
    std::size_t available() const { return _pool.available(); }
    
  protected:
    friend class goblib::Singleton<SoundSystem>;
    SoundSystem();

    bool play(goblib::Stream& s);
    bool play(goblib::PcmStream& s);
    
    static void callbackStreaming(void* arg, std::uint8_t ch, bool start)
    {
        static_cast<SoundSystem*>(arg)->_callbackStreaming(ch, start);
    }
    void _callbackStreaming(std::uint8_t ch, bool start);
    
  private:
    goblib::m5s::Speaker* _speaker;
    goblib::m5s::FileStream* _bgmStream;
    goblib::m5s::FileStream* _creditStream;
    goblib::PcmStream* _bgmPcm;
    goblib::PcmStream* _creditPcm;
    goblib::ObjectPool<goblib::PcmStream> _pool;
    BGM _bgm;
};

#endif
