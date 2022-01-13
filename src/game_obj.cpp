/*!
  TinyDarius

  @file game_obj.cpp
  @brief Game object
*/
#include <LovyanGFX.hpp>
#include "game_obj.hpp"
#include "app.hpp"
#include "renderer.hpp"
#include "constants.hpp"
#include <lgfx/gob_lgfx.hpp>
using goblib::lgfx::GSprite;

#include <cstdio>

bool GameObj::_showHitBox = false;

//goblib::FixedVector<GameObj*, GameObj::Max> GameObj::_collideObjects;
GameObj::CollideObjects GameObj::_collideObjects;

bool GameObj::onInitialize()
{
    TinyDarius::instance().insertRenderObj(this);
    return true;
}

bool GameObj::onRelease()
{
    TinyDarius::instance().removeRenderObj(this);
    return true;
}

// Render bounding
void GameObj::render(void* arg)
{
    if(isShowHitBox())
    {
        _hbox.render(arg, hitable() ? CLR_MAGENTA : CLR_GREEN);
    }
}


bool GameObj::checkHit(GameObj* obj)
{
    if(!obj->hitable()) { return false; }

    return std::any_of(_collideObjects.begin(), _collideObjects.end(),
                       [&obj](GameObj* o)
                       {
                           if(obj != o && o->hitable() && (obj->category() & o->category()) == 0)
                           {
                               auto r = obj->hitBox() & o->hitBox();
                               if(r)
                               {
                                   obj->onHit(o, r);
                                   o->onHit(obj, r);
                                   return true;
                               }
                           }
                           return false;
                       });
}

void GameObj::print()
{
    printf("GameObj: collide:%zu\n", _collideObjects.size());
    for(auto& e: _collideObjects)
    {
        printf(" >[%16s]:%d:(%d,%d) - {(%d,%d) [%d,%d]}\n", e->tag(), e->hitable(),
               static_cast<std::int32_t>(e->_hbox._rpos.x()), static_cast<std::int32_t>(e->_hbox._rpos.y()),
               e->_hbox._box.left(), e->_hbox._box.top(),
               e->_hbox._box.width(), e->_hbox._box.height());
    }

}
