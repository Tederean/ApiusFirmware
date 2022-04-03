#include <Arduino.h>

#include <lvgl.h>
#include <application/gui/GuiService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace Gui
  {

    lv_style_t ScreenStyle;


    lv_obj_t *Arc;

    lv_style_t ArcIndicatorStyle;

    lv_style_t ArcMainStyle;

    lv_anim_t ArcAnimation;

    const timespan_t ArcUpdateInterval_us = 1 * 1000 * 1000;

    Event<void> ArcUpdateEvent;



    void Initialize();

    void InitializeStyle();

    void DrawArc(lv_obj_t *parent);

    void OnArcUpdateEvent(void *args);

    void OnAnimationSetValue(void *arc, int32_t value);



    void Initialize()
    {
      auto *display = lv_disp_get_default();
      auto *screen = lv_disp_get_scr_act(display);
      
      InitializeStyle();

      lv_obj_add_style(screen, &ScreenStyle, LV_PART_MAIN);

      DrawArc(screen);
    }

    void InitializeStyle()
    {
      auto gray20 = lv_color_make(0x33, 0x33, 0x33);
      auto gray50 = lv_color_make(0x7F, 0x7F, 0x7F);
      auto red = lv_color_make(0xFF, 0x00, 0x00);


      lv_style_set_bg_color(&ScreenStyle, gray20);

      lv_style_set_bg_color(&ArcIndicatorStyle, gray20);
      lv_style_set_arc_color(&ArcIndicatorStyle, red);

      lv_style_set_bg_color(&ArcMainStyle, gray20);
      lv_style_set_arc_color(&ArcMainStyle, gray50);
    }

    void DrawArc(lv_obj_t *parent)
    {
      Arc = lv_arc_create(parent);

      lv_obj_set_size(Arc, 480, 480);
      lv_arc_set_end_angle(Arc, 200);
      lv_obj_align(Arc, LV_ALIGN_CENTER, 0, 0);
      lv_arc_set_range(Arc, 0, 105);
      lv_arc_set_value(Arc, 75);

      lv_obj_remove_style(Arc, NULL, LV_PART_KNOB);
      lv_obj_add_style(Arc, &ArcIndicatorStyle, LV_PART_INDICATOR);
      lv_obj_add_style(Arc, &ArcMainStyle, LV_PART_MAIN);

      lv_anim_init(&ArcAnimation);

      ArcUpdateEvent.Subscribe(OnArcUpdateEvent);
      Services::System::InvokeLater(&ArcUpdateEvent, ArcUpdateInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void OnArcUpdateEvent(void *args)
    {
      auto minValue = lv_arc_get_min_value(Arc);
      auto maxValue = lv_arc_get_max_value(Arc);
      auto currentValue = lv_arc_get_value(Arc);
      auto nextValue = random(minValue, maxValue);

      lv_anim_set_exec_cb(&ArcAnimation, OnAnimationSetValue); 
      lv_anim_set_values(&ArcAnimation, currentValue, nextValue);
      lv_anim_set_var(&ArcAnimation, Arc);
      lv_anim_set_time(&ArcAnimation, 200);
      lv_anim_set_path_cb(&ArcAnimation, lv_anim_path_linear);

      lv_anim_start(&ArcAnimation);
    }

    void OnAnimationSetValue(void *component, int32_t value)
    {
      auto arc = (lv_obj_t*)component;

      lv_arc_set_value(arc, value);
    }
    
  } // namespace Display
} // namespace Services




    //if (value < (0.7 * this.max))
    //{
    //   return "#5ee432"; // green
    //} 
    //if (value < (0.8 * this.max))
    //{
    //  return "#fffa50"; // yellow
    //}
    //if (value < (0.9 * this.max))
    //{
    //  return "#f7aa38"; // orange
    //}
    //
    //return "#ef4655"; // red