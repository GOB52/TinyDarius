/*!
  TinyDarius

  @file scene_stage.hpp
  @brief Stage
*/
#pragma once
#ifndef TD_SCENE_STAGE_HPP
#define TD_SCENE_STAGE_HPP

#include <gob_scene.hpp>
#include "../stage.hpp"
#include <gob_m5s_debug.hpp>

class SceneGame;
class SilverHawk;
class Boss;
class RockSurfaceUpper;
class RockSurfaceLower;
class WaveSpot;
class BranchRock;
class Information;
class Mask;
class BlockMask;
class BlinkingMessage;
class CenteringMessage;
class RemainBonusMessage;
class NameEntry;
class MineGenerator;
class YazukaGenerator;

class SceneStage : public goblib::SceneTask
{
  public:
    SceneStage(SceneGame& sg, const Stage& stage, SilverHawk& player, bool firstStage = true);
    virtual ~SceneStage();

  protected:
    virtual void onUnchain() override;
    virtual bool onInitialize() override;
    virtual bool onRelease() override;
    virtual void onReceive (const goblib::TaskMessage& /*msg*/) override;
    virtual void onExecute(const float delta) override;

  private:
    void start();
    void rewind();
    void restart();
    void idle();
    void warning();
    void battleEnemy(bool restart = false);
    void appearBoss();
    void battleBoss();
    void defeatBoss();
    void escapeBoss();
    void branch();
    void roundClear();
    void gameClear();
    void dead();
    void nameEntry();
    void gameover();

    using phase_function = void(SceneStage::*)(const float);
    void nop(const float) {}
    void onStart(const float);
    void onRewind(const float);
    void onRestart(const float);
    void onIdle(const float);
    void onWarning(const float);
    void onBattleEnemy(const float);
    void onAppearBoss(const float);
    void onBattleBoss(const float);
    void onDefeatBoss(const float);
    void onEscapeBoss(const float);
    void onBranch(const float);
    void onRoundClear(const float);
    void onGameClear(const float);
    void onGameover(const float);
    void onNameEntry(const float);
    void onDead(const float);

    void hitCheck();
    void releaseCategoryObj(std::uint32_t category);
    void vanishEnemies();
    
  private:
    SceneGame& _sgame;
    const Stage& _stage;
    SilverHawk& _player;

    /// @name DON'T DELETE THEM in this class.
    /// They are delete themselves.
    /// @{
    Boss* _boss;
    RockSurfaceUpper* _upperSurface;
    RockSurfaceLower* _lowerSurface;
    WaveSpot* _wavespot;
    BranchRock* _branch;
    Information* _info;
    Mask* _mask[2];
    /// @}

    MineGenerator* _mineGenerator;
    YazukaGenerator* _yazukaGenerator;

    /// @name DON'T DELETE THEM in this class.
    /// @{
    BlockMask* _blockMask;
    BlinkingMessage* _bmessage;
    CenteringMessage* _cmessage;
    RemainBonusMessage* _rmessage;
    NameEntry* _entry;
    /// @}

    phase_function _function;
    std::uint32_t _counter;
    bool _firstStage;
    phase_function _function_of_dead;
    
#ifndef NDEBUG
    goblib::m5s::ScopedLeakCheck _leak;
#endif
};

#endif
