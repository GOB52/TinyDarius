/*!
  TinyDarius

  @file app.cpp
  @brief Application class
*/
#if __has_include(<M5Unified.h>)
# include <SdFat.h>
# include <M5Unified.h>
#else
# include <M5stack.h>
# ifdef min
#   undef min
# endif
#endif

#include "lgfx.hpp"
#include "debug.hpp"
#include "app.hpp"
#include "game.hpp"
#include "constants.hpp"

#include "df88.hpp"

#include "renderer.hpp"
#include "sound.hpp"
#include "game_obj.hpp"
#include "typedef.hpp"
#include "scene_manager.hpp"

#include <gob_scene.hpp>
#include <gob_renderer.hpp>
#include <gob_m5s_faces.hpp>
#include <gob_m5s_cpu_usage.hpp>
using goblib::m5s::CpuUsage;
#include <gob_utility.hpp>
#include <gob_m5s_thread.hpp>
#include <gob_m5s_debug.hpp>


#ifndef NDEBUG
#include "scene/scene_debug.hpp"
#endif
#include "scene/scene_advertise.hpp"
//#include "scene/scene_stage.hpp"
#include "scene/scene_game.hpp"

#include <esp_system.h>
#include <cassert>

using goblib::m5s::FaceGB;

namespace
{
constexpr std::int32_t SPLIT = 6;
constexpr std::int32_t STRIP_HEIGHT = SCREEN_HEIGHT / SPLIT;
static_assert(SCREEN_HEIGHT % SPLIT == 0, "Please make it divisible by SPLIT.");
}

std::int32_t ScopedReleaseBus::_nest = 0;

TinyDarius::TinyDarius()
        : goblib::App<AppClock, APP_FPS, APP_FPS>()
        , goblib::Singleton<TinyDarius>()
        , _output(nullptr)
        , _sprite{}
        , _taskTree(nullptr)
        , _sceneManager(nullptr)
        , _renderer(nullptr)
        , _input(nullptr)
        , _game(nullptr)
        , _credits(0)
#ifndef NDEBUG
        , _freeHeap(0)
        , _debug(false)
#endif
        {}

TinyDarius::~TinyDarius()
{
    finalize();
}

#ifdef M5UNIFIED_VERSION
void TinyDarius::setup(M5GFX* output)
#else
void TinyDarius::setup(LGFX* output)
#endif
{
    assert(output);

    _output = output;

    _output->init();
    _output->setRotation(1);
    _output->setBrightness(48);

    _output->fillScreen(TFT_WHITE);
    
    assert(_output->width() == SCREEN_WIDTH);
    assert(_output->height() == SCREEN_HEIGHT);

    for(auto& p : _sprite)
    {
        p = new LGFX_Sprite();
        assert(p);
        p->setColorDepth(_output->getColorDepth());
        if(!p->createSprite(SCREEN_WIDTH, STRIP_HEIGHT))
        {
            abort();
        }
        //#if __has_include("df88.cpp")
        p->setFont(&df88_gfx_font);
        //#endif
        //        p->setFont(&Font2);
        p->setAddrWindow(0, 0, SCREEN_WIDTH, STRIP_HEIGHT);
    }

    printf("sp:%p\n", _sprite[0]);
    printf("sp:%p\n", _sprite[1]);

    
    // Occupy the bus.
    _output->startWrite();
    _output->setAddrWindow(0, 0, _output->width(), _output->height());

    _taskTree = new TdTaskTree();
    assert(_taskTree);
    _sceneManager = new SceneManager(*_taskTree, PRIORITY_SCENE_MAANGER);
    assert(_sceneManager);
    
    _renderer = new goblib::graph::Renderer2D(96);
    assert(_renderer);

    _input = new FaceGB();
    assert(_input);
    _input->setup();

    _game = new Game();
    assert(_game);

#ifndef NDEBUG
    auto s = new SceneDebug();
    assert(s);
    _sceneManager->push(s);
    _freeHeap = esp_get_free_heap_size();
    printDebugInformation();
#else
    auto s = new SceneAdvertise();
    assert(s);
    _sceneManager->push(s);
#endif
}

void TinyDarius::finalize()
{
    goblib::safeDelete(_game);
    goblib::safeDelete(_input);
    goblib::safeDelete(_renderer);

    if(_taskTree)
    {
        _taskTree->clear();
    }
    goblib::safeDelete(_sceneManager);
    goblib::safeDelete(_taskTree);
    for(auto& e : _sprite)
    {
        goblib::safeDelete(e);
    }
}


void TinyDarius::endWrite()
{
    _output->endWrite();
}

void TinyDarius::startWrite()
{
    _output->startWrite();
}

void TinyDarius::fixedUpdate()
{
    //    GOBLIB_SCOPED_PROFILE_HIGH("fixed");
    SoundSystem::instance().pump();
}

void TinyDarius::update(float delta)
{
    //    GOBLIB_SCOPED_PROFILE_HIGH("update");

    _input->pump();

    if(_input->wasPressedEqual(FaceGB::Button::Select) && _credits < CREDIT_MAX)
    {
        SoundSystem::instance().playSfx(SFX::InsertCoin);
        ++_credits;
    }
    
#ifndef NDEBUG
    M5.update();
    if(M5.BtnB.wasPressed())
    {
        _debug = !_debug;
        GameObj::showHitBox(_debug);
    }
    if(M5.BtnC.wasPressed())
    {
        _taskTree->pauseGlobal(!_taskTree->isPauseGlobal());
    }
    if(M5.BtnA.wasPressed())
    {
        printDebugInformation();
    }
#endif

    if(!_taskTree->isPauseGlobal())
    {
        _game->pump();
    }
    _taskTree->pump(delta);
}

#ifndef NDEBUG
void TinyDarius::printDebugInformation() const
{
    printf("%s\n", "----------------------");
    
    _freeHeap = esp_get_free_heap_size();
    printf("H:%u LH:%u FH:%u\n"
           , _freeHeap
           , heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)
           , heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT)
           );
    //    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    _sceneManager->print();
    _taskTree->print();
    _renderer->print();
    GameObj::print();
}

void TinyDarius::drawDebugInformation(LGFX_Sprite* spr) const
{
    //    GOBLIB_SCOPED_LEAK_ABORT();

    int yoff = SCREEN_HEIGHT/SPLIT - 28;
    auto cpu0 = CpuUsage::cpu0();
    auto cpu1 = CpuUsage::cpu1();
    
#define FWID 6
#define FHGT 8
    spr->setTextColor(TFT_WHITE, TFT_BLACK);
    
    spr->writeFillRect(0,yoff,     320,         4, TFT_BLACK);
    spr->writeFillRect(0,yoff,     1.6f * cpu0, 2, cpu0 < 100.0f ? TFT_WHITE : TFT_MAGENTA);
    spr->writeFillRect(0,yoff + 2, 1.6f * cpu1, 2, cpu1 < 100.0f ? TFT_WHITE : TFT_MAGENTA);

    spr->setCursor(0, yoff + FHGT * 1);
    //spr->printf("F:%2.1f", fps()); // ATTENTION: Leak memory if output float!
    float fi = 0, fp;
    fp = std::modf(fps(), &fi);
    spr->printf("F:%2d.%-1d", static_cast<std::int32_t>(fi), static_cast<std::int32_t>(fp*10));

    spr->setCursor(FWID * 12, yoff + FHGT * 1);
    spr->printf("T:%zu R:%zu", _taskTree->size(), _renderer->size());

    spr->setCursor(FWID * 12, yoff + FHGT * 2);
    //spr->printf("D:%1.3f", delta());
    fp = std::modf(delta(), &fi);
    spr->printf("%d.%-4d", static_cast<std::int32_t>(fi), static_cast<std::int32_t>(fp*1000));
    
    spr->setCursor(0, yoff + FHGT * 2);
    spr->printf("H:%u", _freeHeap);

    fp = std::modf(cpu0, &fi);
    spr->setCursor(41 * FWID, yoff + 1 * FHGT);
    //spr->printf("C0:%3.2f", cpu0);
    spr->printf("C0:%3d.%-2d", static_cast<std::int32_t>(fi), static_cast<std::int32_t>(fp*100));
        
    //spr->setCursor(30 * FWID, yoff + 1 * FHGT);
    //spr->printf("C0:%d/%d %3.2f", CpuUsage::get0_i(), CpuUsage::get0_t(), CpuUsage::get0());
    //    CpuUsage::reset0();
    fp = std::modf(cpu1, &fi);
    spr->setCursor(41 * FWID, yoff + 2 * FHGT);
    //spr->printf("C1:%3.2f", cpu1);
    spr->printf("C1:%3d.%-2d", static_cast<std::int32_t>(fi), static_cast<std::int32_t>(fp*100));
    
    if(_taskTree->isPauseGlobal())
    {
        spr->setCursor(0,0);
        spr->printf("%s", "PAUSE");
    }
}
#endif

void TinyDarius::render()
{
    //    GOBLIB_SCOPED_PROFILE_HIGH("render");

    constexpr std::size_t tlen = SCREEN_WIDTH * STRIP_HEIGHT;
    std::int_fast8_t cnt = SPLIT;
    std::int_fast8_t cur = 0;
    std::int32_t y = 0;
    while(cnt--)
    {
        _sprite[cur]->clear();

        RenderArg arg{y, _sprite[cur] };
        _renderer->render(&arg);

        if(cnt == 0)
        {
            _sprite[cur]->setCursor(SCREEN_WIDTH - 8*10, SCREEN_HEIGHT - 16 + y);
            _sprite[cur]->printf("CREDIT: %u", credits());
#ifndef NDEBUG
            if(_debug)
            {
                drawDebugInformation(_sprite[cur]);
            }
#endif
        }

        _output->pushPixelsDMA(static_cast<lgfx::swap565_t*>(_sprite[cur]->getBuffer()), tlen);
        y -= STRIP_HEIGHT;
        cur ^= 1;
    }

}

void TinyDarius::reserveInsertNode(goblib::Task* t, goblib::Task* parent)
{
    _taskTree->reserveInsertNode(t, parent);
}

void TinyDarius::sendBroadcastMessage(goblib::TaskMessage& m, goblib::Task* t)
{
    _taskTree->sendBroadcastMessage(m, t);
}
;
void TinyDarius::postBroadcastMessage(goblib::TaskMessage& m, goblib::Task* t)
{
    _taskTree->postBroadcastMessage(m, t);
}

void TinyDarius::insertRenderObj(goblib::graph::RenderObj2D* r)
{
    _renderer->insert(r);
}

void TinyDarius::removeRenderObj(goblib::graph::RenderObj2D* r)
{
    _renderer->remove(r);
}

void TinyDarius::startGame()
{
    if(credits() == 0) { return; }

    --_credits;
    _game->start(0);
    //    _sceneManager->push(new SceneStage(_game->stage()));
    auto sg = new SceneGame();
    assert(sg);
    _sceneManager->push(sg);
}

void TinyDarius::endGame()
{
    auto a = new SceneAdvertise();
    assert(a);
    _sceneManager->push(a);
}

/*
void TinyDarius::nextStage()
{
}
*/

