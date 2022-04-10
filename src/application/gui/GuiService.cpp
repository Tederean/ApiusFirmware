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

    const size_t GaugeCount = 8;

    const timespan_t ArcUpdateInterval_us = 1 * 1000 * 1000;

    Gauge Gauges[GaugeCount];

    Event<void> ArcUpdateEvent;


    lv_style_t ScreenStyle;

    lv_style_t LineStyle;

    lv_style_t TitleStyle;

    lv_style_t TextStyle;


    const lv_coord_t ScreenWidth  = 800;

    const lv_coord_t ScreenHeight = 480;

    const lv_coord_t ArcWidth = 174;

    const lv_coord_t ArcHeight = 190;

    const lv_coord_t ArcTextHeight = 30;

    const lv_coord_t ArcIndicatorSize = 20;

    const lv_coord_t TitleHight = 49;
    
    const lv_coord_t GridSpacing = 17;

    const lv_coord_t LineThickness = 2;

    const lv_coord_t LineHeight = 480 - GridSpacing - GridSpacing;


    void Initialize();

    void DrawArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t filledColor);

    void DrawArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t filledColor, const char *text);

    void OnArcUpdateEvent(void *args);

    void OnAnimationSetValue(void *arc, int32_t value);



    void Initialize()
    {
      auto *display = lv_disp_get_default();
      auto *screen = lv_disp_get_scr_act(display);

      auto gray80 = lv_color_make(0x33, 0x33, 0x33);
      auto gray50 = lv_color_make(0x7F, 0x7F, 0x7F);
      auto red = lv_color_make(0xFF, 0x00, 0x00);

      
      lv_style_set_bg_color(&ScreenStyle, gray80);
      lv_obj_add_style(screen, &ScreenStyle, LV_PART_MAIN);


      {
        auto *line = lv_obj_create(screen);
        lv_obj_set_size(line, LineThickness, LineHeight);
        lv_obj_set_pos(line, GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + 1, GridSpacing + 1);

        lv_style_set_bg_color(&LineStyle, gray50);
        lv_style_set_border_color(&LineStyle, gray50);
        lv_style_set_radius(&LineStyle, 0);
        lv_obj_add_style(line, &LineStyle, LV_PART_MAIN);
      }


      lv_style_set_text_align(&TitleStyle, LV_TEXT_ALIGN_CENTER);
      lv_style_set_text_font(&TitleStyle, &lv_font_montserrat_28);
      lv_style_set_text_color(&TitleStyle, gray50);


      lv_style_set_text_align(&TextStyle, LV_TEXT_ALIGN_CENTER);
      lv_style_set_text_font(&TextStyle, &lv_font_montserrat_20);
      lv_style_set_text_color(&TextStyle, gray50);


      {
        auto *cpuTitle = lv_label_create(screen);
        lv_label_set_long_mode(cpuTitle, LV_LABEL_LONG_CLIP);
        lv_label_set_text(cpuTitle, "AMD Ryzen 7 5800X");
        lv_obj_set_size(cpuTitle, ArcWidth + GridSpacing + ArcWidth, TitleHight);
        lv_obj_set_pos(cpuTitle, GridSpacing + 1, GridSpacing + 1);
        lv_obj_add_style(cpuTitle, &TitleStyle, LV_PART_MAIN);
      }

      {
        auto *gpuTitle = lv_label_create(screen);
        lv_label_set_long_mode(gpuTitle, LV_LABEL_LONG_CLIP);
        lv_label_set_text(gpuTitle, "NVIDIA GeForce RTX 3070");
        lv_obj_set_size(gpuTitle, ArcWidth + GridSpacing + ArcWidth, TitleHight);
        lv_obj_set_pos(gpuTitle, GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1, GridSpacing + 1);
        lv_obj_add_style(gpuTitle, &TitleStyle, LV_PART_MAIN);
      }


      //auto *debugLine = lv_obj_create(screen);
      //lv_obj_set_size(debugLine, ArcWidth, ArcHeight);
      //lv_obj_set_pos(debugLine, GridSpacing + 1, GridSpacing + TitleHight + 1);
      //lv_obj_add_style(debugLine, &LineStyle, LV_PART_MAIN);


      DrawArcs(screen, gray80, gray50, red);
    }

    void DrawArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t filledColor)
    {
      auto y0 = GridSpacing + TitleHight + 1;
      auto y1 = GridSpacing + TitleHight + ArcHeight + GridSpacing + 1;
     
      auto x0 = GridSpacing + 1;
      auto x1 = GridSpacing + ArcWidth + GridSpacing + 1;
      auto x2 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1;
      auto x3 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + ArcWidth + GridSpacing + 1;

      DrawArc(parent, 0, x0, y0, backgroundColor, emptyColor, filledColor, "Utilization");
      DrawArc(parent, 1, x1, y0, backgroundColor, emptyColor, filledColor, "Wattage");
      DrawArc(parent, 2, x2, y0, backgroundColor, emptyColor, filledColor, "Utilization");
      DrawArc(parent, 3, x3, y0, backgroundColor, emptyColor, filledColor, "Wattage");

      DrawArc(parent, 4, x0, y1, backgroundColor, emptyColor, filledColor, "Temperature");
      DrawArc(parent, 5, x1, y1, backgroundColor, emptyColor, filledColor, "Memory");
      DrawArc(parent, 6, x2, y1, backgroundColor, emptyColor, filledColor, "Temperature");
      DrawArc(parent, 7, x3, y1, backgroundColor, emptyColor, filledColor, "Memory");

      ArcUpdateEvent.Subscribe(OnArcUpdateEvent);
      Services::System::InvokeLater(&ArcUpdateEvent, ArcUpdateInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void DrawArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t filledColor, const char *text)
    {
      auto gaugeStruct = &Gauges[index];
      auto arc = lv_arc_create(parent);
      auto indicatorStyle = &gaugeStruct->IndicatorStyle;
      auto mainStyle = &gaugeStruct->MainStyle;
      auto animation = &gaugeStruct->Animation;

      gaugeStruct->Arc = arc;

      lv_obj_set_size(arc, ArcWidth, ArcWidth);
      lv_obj_set_pos(arc, x, y);
      lv_arc_set_end_angle(arc, 200);
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


      auto *arcLabel = lv_label_create(parent);
      lv_label_set_long_mode(arcLabel, LV_LABEL_LONG_CLIP);
      lv_label_set_text(arcLabel, text);
      lv_obj_set_size(arcLabel, ArcWidth, ArcTextHeight);
      lv_obj_set_pos(arcLabel, x, y + ArcHeight - ArcTextHeight);
      lv_obj_add_style(arcLabel, &TextStyle, LV_PART_MAIN);
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