/*!
  TinyDarius

  @file scene_stage.cpp
  @brief Stage
*/
#include "scene_stage.hpp"
#include "scene_game.hpp"
#include "../debug.hpp"
#include "../app.hpp"
#include "../game.hpp"
#include "../sound.hpp"
#include "../typedef.hpp"
#include "../constants.hpp"
#include "../player/silver_hawk.hpp"
#include "../boss/boss.hpp"
#include "../background/rock_surface.hpp"
#include "../background/wave_spot.hpp"
#include "../background/branch_rock.hpp"
#include "../info/information.hpp"
#include "../info/message.hpp"
#include "../info/name_entry.hpp"
#include "../background/mask.hpp"
#include "../effect/block_mask.hpp"
#include "../enemy/enemy.hpp"
#include "../effect/explosion.hpp"

#include <gob_utility.hpp>
#include <lgfx/gob_lgfx_animated_palette.hpp>
#include <gob_m5s_debug.hpp>

#include <cassert>
#include <cstdio>

namespace
{
constexpr Pos2i UPPER_ROCK_POS(0, 24);
constexpr Pos2i LOWER_ROCK_POS(128,200);

constexpr std::int16_t MESSAGE_POS_Y[] =
{
    88, // for Blink message
    120,
    136,
    152,
    168 // for remain bonus pts
};

constexpr const char* WARNING_MESSAGE[] =
{
    "WARNING!", // blink
    "A HUGE BATTLESHIP",
    "", // boss name
    "IS APPROACHING FAST."
};

constexpr const char* ROUND_CLEAR_MESSAGE[] =
{
    "\"%c\" ZONE IS OVER", // blink
    "WE ARE NOW RUSHING INTO \"%c\" ZONE.",
    "BE ON YOUR GUARD",
};

constexpr const char* GAME_CLEAR_MESSAGE[] =
{
    "FINAL ZONE IS OVER", // blink
    "YOU MADE IT!",
    "REMAINED PLAYER BONUS",
    /* remained bonus points */
};

constexpr const char* TIME_UP_MESSAGE[] = 
{
    "TIMEUP!", // blink
    "A HUGE BATTLESHIP",
    "", // bossname
    "HAS BEEN ESCAPED."
};

constexpr const char* GAMEOVER_MESSAGE = "GAME OVER";
//
}

SceneStage::SceneStage(SceneGame& sg, const Stage& stage, SilverHawk& player, bool firstStage)
        : goblib::SceneTask(SCENE_STAGE, PRIORITY_STAGE, "stage")
        , _sgame(sg)
        , _stage(stage)
        , _player(player)
        , _boss(nullptr)
        , _upperSurface(nullptr)
        , _lowerSurface(nullptr)
        , _wavespot(nullptr)
        , _branch(nullptr)
        , _info(nullptr)
        , _mask{}
        , _mineGenerator(nullptr)
        , _yazukaGenerator(nullptr)
        , _blockMask(nullptr)
        , _bmessage(nullptr)
        , _cmessage(nullptr)
        , _rmessage(nullptr)
        , _entry(nullptr)
        , _function(&SceneStage::nop)
        , _counter(0)
        , _firstStage(firstStage)
        , _function_of_dead(&SceneStage::nop)
#ifndef NDEBUG
        , _leak(__PRETTY_FUNCTION__, __LINE__, false)
#endif
{
    _boss = Boss::create(stage.current);
    assert(_boss);
    _boss->pause();
    _boss->hide();

    _upperSurface = new RockSurfaceUpper(UPPER_ROCK_POS.x(), UPPER_ROCK_POS.y());
    assert(_upperSurface);
    _lowerSurface = new RockSurfaceLower(LOWER_ROCK_POS.x(), LOWER_ROCK_POS.y());
    assert(_lowerSurface);

    _wavespot = new WaveSpot();
    assert(_wavespot);

    _branch = new BranchRock();
    assert(_branch);
    
    _info = new Information();
    assert(_info);

    _mask[0] = new Mask(Rect2(0, 0, FIELD_RECT.width(), FIELD_RECT.top()));
    assert(_mask[0]);
    _mask[1] = new Mask(Rect2(0, FIELD_RECT.bottom() + 1, FIELD_RECT.width(), SCREEN_RECT.bottom() - FIELD_RECT.bottom()));
    assert(_mask[1]);

    //
    _mineGenerator = new MineGenerator(*this);
    assert(_mineGenerator);
    _yazukaGenerator = new YazukaGenerator(*this);
    assert(_yazukaGenerator);
}

SceneStage:: ~SceneStage()
{
    TD_PRINT_FUNCTION();
    goblib::safeDelete(_yazukaGenerator);
    goblib::safeDelete(_mineGenerator);
} 

void SceneStage::onUnchain()
{
    TD_PRINT_FUNCTION();
    delete this;
}

bool SceneStage::onInitialize()
{
    TD_PRINT_FUNCTION();
    TinyDarius& app = TinyDarius::instance();

    app.reserveInsertNode(_boss, this);
    app.reserveInsertNode(_upperSurface, this);
    app.reserveInsertNode(_lowerSurface, this);
    app.reserveInsertNode(_wavespot, this);
    app.reserveInsertNode(_branch, this);
    app.reserveInsertNode(_info, this);
    app.reserveInsertNode(_mask[0], this);
    app.reserveInsertNode(_mask[1], this);

    if(_firstStage)
    {
        start();
    }
    else
    {
        rewind();
    }
    return true;
}

bool SceneStage::onRelease()
{
    TD_PRINT_FUNCTION();
    SoundSystem::instance().stop();
    return true;
}

void SceneStage::onReceive (const goblib::TaskMessage& msg)
{
    switch(msg.msg)
    {
    case MSG_DEFEAT_BOSS:
        if(_player.alive())
        {
            defeatBoss();
        }
        break;
    }
}

void SceneStage::onExecute(const float delta)
{
    (this->*_function)(delta);
    hitCheck();
    ++_counter;
}

// Phase: start
void SceneStage::start()
{
    TD_PRINTF("====%s:%d\n", __PRETTY_FUNCTION__, _player.visible());
    _counter = 0;
    _player.restart();
    _function = &SceneStage::onStart;

    _upperSurface->setVelocity(RockSurfaceUpper::Velocity::Normal);
    _lowerSurface->setVelocity(RockSurfaceUpper::Velocity::Normal);
    _upperSurface->pause();
    _lowerSurface->pause();

    _wavespot->pause();
    _wavespot->hide();

    _branch->pause();
    _branch->hide();
    
    _blockMask = new BlockMask();
    assert(_blockMask);
    TinyDarius::instance().reserveInsertNode(_blockMask, this);

    SoundSystem::instance().playBgm(_stage.bgm);
}

void SceneStage::onStart(const float)
{
    if(!_player.visible() && _blockMask->isExecute())
    {
        _player.show();
    }

    if(_blockMask && !_blockMask->busy())
    {
        idle();
    }
}

// Phase: rewind
void SceneStage::rewind()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _player.rewind();
    _function = &SceneStage::onRewind;

    _branch->hide();
    _wavespot->pause();
    _wavespot->hide();

    SoundSystem::instance().playBgm(_stage.bgm);
}

void SceneStage::onRewind(const float)
{
    if(_counter >= 3_fsec)
    {
        idle();
    }
}

// Phase: restart
void SceneStage::restart()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onRestart;

    _player.restart();
    _player.show();

    _boss->hideEffect();
    _lowerSurface->show();
    _upperSurface->show();
    _wavespot->show();
    
    _branch->show(_function_of_dead == &SceneStage::onBranch);
    
    _blockMask = new BlockMask();
    assert(_blockMask);
    TinyDarius::instance().reserveInsertNode(_blockMask, this);
}

void SceneStage::onRestart(const float)
{
    if(_blockMask && !_blockMask->busy())
    {
        if(_function_of_dead == &SceneStage::onDefeatBoss)
        {
            TD_PRINTF("%s:Restart defeated\n", __PRETTY_FUNCTION__);
            printf("Restart:defeat\n");
            _player.enable();
            _upperSurface->resume();
            _lowerSurface->resume();
            _wavespot->resume();
            branch();
            SoundSystem::instance().playBgm(BGM::RoundClear);
        }else if(_function_of_dead == &SceneStage::onBranch)
        {
            TD_PRINTF("%s:Restart branch\n", __PRETTY_FUNCTION__);
            _branch->resume();
            _upperSurface->resume();
            _lowerSurface->resume();
            _wavespot->resume();
            _function = _function_of_dead;
            SoundSystem::instance().playBgm(BGM::RoundClear);
        }
        else
        {
            TD_PRINTF("%s:Restart battle\n", __PRETTY_FUNCTION__);
            _upperSurface->resume();
            _lowerSurface->resume();
            _wavespot->resume();
            battleEnemy(true);
            SoundSystem::instance().playBgm(_stage.bossBgm());
        }
        _function_of_dead = nullptr;
    }
}

// Phase: idle
void SceneStage::idle()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _player.restart();
    _function = &SceneStage::onIdle;

    _upperSurface->resume();
    _lowerSurface->resume();
}

void SceneStage::onIdle(const float)
{
#ifdef NDEBUG
    if(_counter >= 5_fsec)
#else
    if(_counter >= 1_fsec)
#endif
    {
        warning();
    }
}

// Phase: warning
void SceneStage::warning()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);

    TinyDarius& app = TinyDarius::instance();

    _counter = 0;
    _function = &SceneStage::onWarning;

    _bmessage = new BlinkingMessage(WARNING_MESSAGE[0], MESSAGE_POS_Y[0], CLR_WHITE);
    assert(_bmessage);
    app.reserveInsertNode(_bmessage, this);
    
    _cmessage = new CenteringMessage();
    assert(_cmessage);
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "%s-%c", _boss->name(), _stage.toChar());
    _cmessage->insert(WARNING_MESSAGE[1],       MESSAGE_POS_Y[1], CLR_WHITE);
    _cmessage->insert(tmp,                      MESSAGE_POS_Y[2], CLR_RED);
    _cmessage->insert(WARNING_MESSAGE[3],       MESSAGE_POS_Y[3], CLR_WHITE);
    app.reserveInsertNode(_cmessage, this);
    
    _upperSurface->animatedPalette()->fadeOut(60);
    _lowerSurface->animatedPalette()->fadeOut(60);
    
    SoundSystem::instance().playBgm(BGM::Warning);
}

void SceneStage::onWarning(const float)
{
    if(_counter >= 9_fsec)
    {
        battleEnemy();
    }
}

// Phase battle Enemy
void SceneStage::battleEnemy(bool restart)
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onBattleEnemy;

    if(_bmessage)
    {
        _bmessage->release();
        _bmessage = nullptr;
    }
    if(_cmessage)
    {
        _cmessage->release();
        _cmessage = nullptr;
    }

    if(!restart)
    {
        _upperSurface->setVelocity(RockSurfaceUpper::Velocity::Boss);
        _lowerSurface->setVelocity(RockSurfaceUpper::Velocity::Boss);
        _upperSurface->resume();
        _lowerSurface->resume();
        _wavespot->resume();
        _wavespot->show();

        _upperSurface->animatedPalette()->fadeAll(60);
        _lowerSurface->animatedPalette()->fadeAll(60);
        _wavespot->animatedPalette()->fadeIn(60);
    }
    
    TinyDarius::instance().game().resume();
    _player.enable();

    _mineGenerator->spawn();
    
    SoundSystem::instance().playBgm(_stage.bossBgm());
}

void SceneStage::onBattleEnemy(const float delta)
{
    if(_counter >= 10_fsec)
    {
        appearBoss();
    }
}

// Phase apper boss
void SceneStage::appearBoss()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onAppearBoss;

    _boss->appear();
}

void SceneStage::onAppearBoss(const float)
{
    if(_boss->status() == Boss::Status::Battle)
    {
        battleBoss();
    }

}

// Phase: battle boss
void SceneStage::battleBoss()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onBattleBoss;

    _wavespot->animatedPalette()->clear();
    _wavespot->roundTripPalette(_stage.current);

}

void SceneStage::onBattleBoss(const float)
{
    if(TinyDarius::instance().game().isTimeup())
    {
        escapeBoss();
        return;
    }

    auto tm = TinyDarius::instance().game().remainingTime();
    if(tm <= 60_fsec)
    {
        if((tm % (6_fsec)) == 0)
        {
            _yazukaGenerator->spawn();
        }
    }
}

// Phase: defeat
void SceneStage::defeatBoss()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onDefeatBoss;

    vanishEnemies();

    TinyDarius::instance().game().pause();
    SoundSystem::instance().stopBgm();
}

void SceneStage::onDefeatBoss(const float)
{
    if(_counter == 4_fsec)
    {
        SoundSystem::instance().playBgm(BGM::RoundClear);
    }

    // Boss has been frameout.
    if(_boss->isNop())
    {
        if(_stage.isTail())
        {
            gameClear();
        }
        else
        {
            branch();
        }
    }
}

// Phase: escape
void SceneStage::escapeBoss()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onEscapeBoss;

    vanishEnemies();
    
    TinyDarius::instance().game().pause();
    SoundSystem::instance().stopBgm();
    
    TinyDarius& app = TinyDarius::instance();
    _bmessage = new BlinkingMessage(TIME_UP_MESSAGE[0], MESSAGE_POS_Y[0], CLR_WHITE);
    assert(_bmessage);
    app.reserveInsertNode(_bmessage, this);

    _cmessage = new CenteringMessage();
    assert(_cmessage);
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "%s-%c", _boss->name(), _stage.toChar());
    _cmessage->insert(TIME_UP_MESSAGE[1],       MESSAGE_POS_Y[1], CLR_WHITE);
    _cmessage->insert(tmp,                      MESSAGE_POS_Y[2], CLR_RED);
    _cmessage->insert(TIME_UP_MESSAGE[3],       MESSAGE_POS_Y[3], CLR_WHITE);
    app.reserveInsertNode(_cmessage, this);

    _boss->escape();
    SoundSystem::instance().playBgm(BGM::Warning);
}

void SceneStage::onEscapeBoss(const float)
{
    // Boss has been frameout.
    if(_counter >= 9_fsec && _boss->isNop() && _cmessage->isFinish())
    {
        SoundSystem::instance().playBgm(BGM::RoundClear);
        if(_stage.isTail())
        {
            gameClear();
        }
        else
        {
            branch();
        }
    }
}

// Phase: branch
void SceneStage::branch()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onBranch;

    if(_bmessage)
    {
        _bmessage->release();
        _bmessage = nullptr;
    }
    if(_cmessage)
    {
        _cmessage->release();
        _cmessage = nullptr;
    }
}

void SceneStage::onBranch(const float)
{
    if(_counter >= 4_fsec)
    {
        _branch->resume();
        _branch->show();
    }

    if(_counter >= 8_fsec)
    {
        roundClear();
    }
}

// Phase roundclear
void SceneStage::roundClear()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);

    _counter = 0;
    _function = &SceneStage::onRoundClear;

    _player.disable();
    auto ns = _stage.next();
    auto no = _player.pos().y() < SCREEN_HEIGHT/2 ? ns.first : ns.second;
    const Stage* nstage = &Stage::table[no];

    TD_PRINTF("next: %d:%c\n", nstage->current, nstage->toChar());
    
    TinyDarius& app = TinyDarius::instance();

    char tmp[64];
    snprintf(tmp, sizeof(tmp), ROUND_CLEAR_MESSAGE[0], _stage.toChar());
    _bmessage = new BlinkingMessage(tmp, MESSAGE_POS_Y[0], CLR_WHITE);
    assert(_bmessage);
    app.reserveInsertNode(_bmessage, this);

    _cmessage = new CenteringMessage();
    assert(_cmessage);
    snprintf(tmp, sizeof(tmp), ROUND_CLEAR_MESSAGE[1], nstage->toChar());
    _cmessage->insert(tmp,                      MESSAGE_POS_Y[1], CLR_WHITE);
    _cmessage->insert(ROUND_CLEAR_MESSAGE[2],   MESSAGE_POS_Y[2], CLR_WHITE);
    app.reserveInsertNode(_cmessage, this);

    _wavespot->animatedPalette()->fadeOut(4_fsec);
    _branch->animatedPalette()->fadeOut(4_fsec);
}

void SceneStage::onRoundClear(const float)
{
    if(_counter >= 10_fsec)
    {
        // %%%%%
        //        TinyDarius::instance().game().chooseNext( _player.pos().y() < SCREEN_HEIGHT/2 );
        //        popScene();
        goblib::TaskMessage msg(MSG_ROUND_CLEAR, _player.pos().y() < SCREEN_HEIGHT/2 );
        TinyDarius::instance().task().postMessage(msg, &_sgame);
        release(true);
    }
}

// Phase: gameclear
void SceneStage::gameClear()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);

    _counter = 0;
    _function = &SceneStage::onGameClear;

    _player.disable();
    
    TinyDarius& app = TinyDarius::instance();

    if(_bmessage)
    {
        _bmessage->release();
        _bmessage = nullptr;
    }
    if(_cmessage)
    {
        _cmessage->release();
        _cmessage = nullptr;
    }
    
    _bmessage = new BlinkingMessage(GAME_CLEAR_MESSAGE[0], MESSAGE_POS_Y[0], CLR_WHITE);
    assert(_bmessage);
    app.reserveInsertNode(_bmessage, this);

    _cmessage = new CenteringMessage();
    _cmessage->insert(GAME_CLEAR_MESSAGE[1],    MESSAGE_POS_Y[1], CLR_WHITE);
    _cmessage->insert(GAME_CLEAR_MESSAGE[2],    MESSAGE_POS_Y[2], CLR_WHITE);

    app.reserveInsertNode(_cmessage, this);

    _rmessage = new RemainBonusMessage(0, MESSAGE_POS_Y[4], CLR_WHITE);
    assert(_rmessage);
    _rmessage->pause();
    _rmessage->hide();
    app.reserveInsertNode(_rmessage, this);
    
    _upperSurface->animatedPalette()->fadeOut(2_fsec);
    _lowerSurface->animatedPalette()->fadeOut(2_fsec);
    _wavespot->animatedPalette()->fadeOut(2_fsec);
}

void SceneStage::onGameClear(const float)
{
    if(_rmessage->isPause() && _cmessage->isFinish())
    {
        _rmessage->resume();
        _rmessage->show();
        _counter = 0;
        return;
    }

    if(!_rmessage->isPause() && _counter % (2_fsec) == 0)
    {
        Game& game = TinyDarius::instance().game();
        if(game.remaining())
        {
            Score s(PTS_REMAIN_BONUS);
            s.insertObserver(game);
            s.notify();
            _rmessage->increasePts(s.score());
            game.dead();
        }
        else
        {
            nameEntry();
        }
    }
}

// Phase: dead
void SceneStage::dead()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function_of_dead = _function;
    _function = &SceneStage::onDead;

    // cancel fadein in onAppear
    //_upperSurface->animatedPalette()->apply();
    //_lowerSurface->animatedPalette()->apply();
    
    TinyDarius::instance().game().dead();
    TinyDarius::instance().game().pause();
}

void SceneStage::onDead(const float)
{
    if(_counter >= 2_fsec)
    {
        if(TinyDarius::instance().game().isGameover())
        {
            nameEntry();
            return;
        }

        _lowerSurface->pause();
        _upperSurface->pause();
        _wavespot->pause();
        _lowerSurface->hide();
        _upperSurface->hide();
        _wavespot->hide();

        _branch->pause();
        _branch->hide();

        _boss->hide();
        _boss->disableHit();
        _boss->pause();

        releaseCategoryObj(CATEGORY_ENEMY | CATEGORY_ENEMY_BULLET);
        restart();
    }
}

// Phase: name entry
void SceneStage::nameEntry()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);

    if(_bmessage)
    {
        _bmessage->release();
        _bmessage = nullptr;
    }
    if(_cmessage)
    {
        _cmessage->release();
        _cmessage = nullptr;
    }
    if(_rmessage)
    {
        _rmessage->release();
        _rmessage = nullptr;
    }
    
    _player.hide();
    //
    //    if(outofranking) { gameover(); return; }
    //

    _counter = 0;
    _function = &SceneStage::onNameEntry;
    
    _entry = new NameEntry(TinyDarius::instance().game().score(), 52 /* FIXME : dummy rank*/ , MESSAGE_POS_Y[1]);
    assert(_entry);
    TinyDarius::instance().reserveInsertNode(_entry, this);

    SoundSystem::instance().playBgm(BGM::Name);
}

void SceneStage::onNameEntry(const float)
{
    if(_entry->isFinish())
    {
        _entry->hide();
        _entry->release();
        _entry = nullptr;
        gameover();
    }
}

// Phase: gameover
void SceneStage::gameover()
{
    TD_PRINTF("====%s\n", __PRETTY_FUNCTION__);
    _counter = 0;
    _function = &SceneStage::onGameover;

    _player.disable();
    _player.hide();
    
    TinyDarius& app = TinyDarius::instance();

    _bmessage = new BlinkingMessage(GAMEOVER_MESSAGE, MESSAGE_POS_Y[2], CLR_WHITE, 0);
    assert(_bmessage);
    app.reserveInsertNode(_bmessage, this);

    SoundSystem::instance().playBgm(BGM::Gameover, false);
}

void SceneStage::onGameover(const float)
{
    if(_counter >= 7_fsec)
    {
        goblib::TaskMessage msg(MSG_GAME_OVER);
        TinyDarius::instance().task().postMessage(msg, &_sgame);
        release(true);
    }
}

void SceneStage::hitCheck()
{
    if(!_player.isEnable()) { return; }

    GameObj::CollideObjects& objes = GameObj::collideObjects();

    // Comment out if you want to make it invincible.
#if 1
    // player vs any
    if(GameObj::checkHit(&_player))
    {
        dead();
        return;
    }
#endif
    
    // player bullets vs any
    std::vector<GameObj*> bullets;
    std::copy_if(objes.begin(), objes.end(), std::back_inserter(bullets),
                 [](GameObj* e)
                 {
                     return e->category() == CATEGORY_BULLET;
                 });
    if(bullets.empty()) { return; }

    for(auto& e : bullets)
    {
        GameObj::checkHit(e);
    }
}

void SceneStage::releaseCategoryObj(std::uint32_t category)
{
    GameObj::CollideObjects& objes = GameObj::collideObjects();
    for(auto& e : objes)
    {
        if(e->category() & category)
        {
            e->release();
        }
    }
}

void SceneStage::vanishEnemies()
{
    goblib::TaskMessage msg;
    msg.msg = MSG_VANISH_ENEMY;
    msg.arg = this;
    TinyDarius::instance().sendBroadcastMessage(msg, this);
}
