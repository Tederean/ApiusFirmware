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

    typedef struct Gauge
    {
      lv_obj_t *Arc;
      lv_style_t IndicatorStyle;
      lv_style_t MainStyle;
      lv_anim_t Animation;
    } Gauge;


    const size_t GaugeCount = 1;

    const lv_coord_t ArcSize = 400;

    const lv_coord_t ArcIndicatorSize = 40;


    Gauge Gauges[GaugeCount];

    lv_style_t ScreenStyle;


    const timespan_t ArcUpdateInterval_us = 1 * 1000 * 1000;

    Event<void> ArcUpdateEvent;



    void Initialize();

    void DrawArc(lv_obj_t *parent, size_t index, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t filledColor);

    void OnArcUpdateEvent(void *args);

    void OnAnimationSetValue(void *arc, int32_t value);



    void Initialize()
    {
      auto *display = lv_disp_get_default();
      auto *screen = lv_disp_get_scr_act(display);

      auto gray20 = lv_color_make(0x33, 0x33, 0x33);
      auto gray50 = lv_color_make(0x7F, 0x7F, 0x7F);
      auto red = lv_color_make(0xFF, 0x00, 0x00);


      lv_style_set_bg_color(&ScreenStyle, gray20);
      lv_obj_add_style(screen, &ScreenStyle, LV_PART_MAIN);


      DrawArc(screen, 0, gray20, gray50, red);


      ArcUpdateEvent.Subscribe(OnArcUpdateEvent);
      Services::System::InvokeLater(&ArcUpdateEvent, ArcUpdateInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }


    void DrawArc(lv_obj_t *parent, size_t index, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t filledColor)
    {
      auto temperatureGauge = &Gauges[index];
      auto arc = lv_arc_create(parent);
      auto indicatorStyle = &temperatureGauge->IndicatorStyle;
      auto mainStyle = &temperatureGauge->MainStyle;
      auto animation = &temperatureGauge->Animation;

      temperatureGauge->Arc = arc;

      lv_obj_set_size(arc, ArcSize, ArcSize);
      lv_arc_set_end_angle(arc, 200);
      lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
      lv_arc_set_range(arc, 0, 10500);
      lv_arc_set_value(arc, 7500);


      lv_style_set_bg_color(indicatorStyle, backgroundColor);
      lv_style_set_arc_color(indicatorStyle, filledColor);
      lv_style_set_arc_width(indicatorStyle, ArcIndicatorSize);

      lv_style_set_bg_color(mainStyle, backgroundColor);
      lv_style_set_arc_color(mainStyle, emptyColor);
      lv_style_set_arc_width(mainStyle, ArcIndicatorSize);


      lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
      lv_obj_add_style(arc, indicatorStyle, LV_PART_INDICATOR);
      lv_obj_add_style(arc, mainStyle, LV_PART_MAIN);

      lv_anim_init(animation);
    }

    void OnArcUpdateEvent(void *args)
    {
      for (size_t index = 0; index < GaugeCount; index++)
      {
        auto gauge = &Gauges[index];
        auto arc = gauge->Arc;
        auto animation = &gauge->Animation;

        auto minValue = lv_arc_get_min_value(arc);
        auto maxValue = lv_arc_get_max_value(arc);
        auto currentValue = lv_arc_get_value(arc);
        auto nextValue = random(minValue, maxValue);

        lv_anim_set_exec_cb(animation, OnAnimationSetValue); 
        lv_anim_set_values(animation, currentValue, nextValue);
        lv_anim_set_var(animation, arc);
        lv_anim_set_time(animation, 950);
        lv_anim_set_path_cb(animation, lv_anim_path_ease_in_out);

        lv_anim_start(animation);
      }
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