/*!
  TinyDarius

  @file mask.hpp
  @brief Masking
*/
#pragma  once
#ifndef TD_MASK_HPP
#define TD_MASK_HPP

#include "../game_obj.hpp"
#include "../typedef.hpp"
#include "../constants.hpp"

class Mask : public GameObj
{
  public:
    explicit Mask(const Rect2& rect) : GameObj(PRIORITY_MASK, ORDER_MASK, CATEGORY_NONE, "mask"),_rect(rect) {}
    virtual ~Mask();

    virtual void render(void* arg) override;

  protected:
    virtual void onUnchain() override;

  protected:
    Rect2 _rect;
};

#endif
