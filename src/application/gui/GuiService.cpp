#include <Arduino.h>

#include <lvgl.h>
#include <application/gui/GuiService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>
#include <framework/utils/Math.h>

using namespace std;

namespace Services
{
  namespace Gui
  {

    typedef struct Gauge
    {
      const char *TitleText;
      const char *Unit;
      const float MinValue;
      const float MaxValue;
      lv_anim_exec_xcb_t UpdateCallback;
      lv_obj_t *Arc;
      lv_obj_t *Title;
      lv_style_t IndicatorStyle;
      lv_style_t MainStyle;
      lv_style_t TitleStyle;
      lv_anim_t IndicatorAnimation;
    } Gauge;



    void Initialize();

    void DrawSeperationLine(lv_obj_t *parent, lv_color_t lineColor);

    void SetScreenBackground(lv_obj_t *screen, lv_color_t backgroundColor);

    void DrawTitles(lv_obj_t *parent, lv_color_t textColor);

    void DrawArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor);

    void DrawArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor);

    void OnArcUpdateEvent(void *args);

    void OnCpuUtilizationUpdate(void *component, int32_t value);

    void OnCpuWattageUpdate(void *component, int32_t value);

    void OnCpuTemperatureUpdate(void *component, int32_t value);

    void OnCpuMemoryUpdate(void *component, int32_t value);

    void OnGpuUtilizationUpdate(void *component, int32_t value);

    void OnGpuWattageUpdate(void *component, int32_t value);

    void OnGpuTemperatureUpdate(void *component, int32_t value);

    void OnGpuMemoryUpdate(void *component, int32_t value);
    
    void OnAnimationUpdate(Gauge *gaugeStruct, int32_t arcValue);



    const timespan_t ArcUpdateInterval_us = 1 * 1000 * 1000;

    const size_t GaugeCount = 8;

    Gauge Gauges[] =
    {
      {
        .TitleText = "Utilization",
        .Unit = "%",
        .MinValue = 0.0f,
        .MaxValue = 100.0f,
        .UpdateCallback = OnCpuUtilizationUpdate,
      },

      {
        .TitleText = "Wattage",
        .Unit = "W",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .UpdateCallback = OnCpuWattageUpdate,
      },

      {
        .TitleText = "Temperature",
        .Unit = "°C",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .UpdateCallback = OnCpuTemperatureUpdate,
      },

      {
        .TitleText = "Memory",
        .Unit = "B",
        .MinValue = 0.0f,
        .MaxValue = 32767.0f,
        .UpdateCallback = OnCpuMemoryUpdate,
      },

      {
        .TitleText = "Utilization",
        .Unit = "%",
        .MinValue = 0.0f,
        .MaxValue = 100.0f,
        .UpdateCallback = OnGpuUtilizationUpdate,
      },

      {
        .TitleText = "Wattage",
        .Unit = "W",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .UpdateCallback = OnGpuWattageUpdate,
      },

      {
        .TitleText = "Temperature",
        .Unit = "°C",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .UpdateCallback = OnGpuTemperatureUpdate,
      },

      {
        .TitleText = "Memory",
        .Unit = "B",
        .MinValue = 0.0f,
        .MaxValue = 32767.0f,
        .UpdateCallback = OnGpuMemoryUpdate,
      },
    };

    Event<void> ArcUpdateEvent;


    lv_style_t ScreenStyle;

    lv_style_t LineStyle;

    lv_style_t TitleStyle;


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



    void Initialize()
    {
      auto *display = lv_disp_get_default();
      auto *screen = lv_disp_get_scr_act(display);

      auto gray80 = lv_color_make(0x33, 0x33, 0x33);
      auto gray50 = lv_color_make(0x7F, 0x7F, 0x7F);


      SetScreenBackground(screen, gray80);

      DrawSeperationLine(screen, gray50);

      DrawTitles(screen, gray50);

      DrawArcs(screen, gray80, gray50);
    }

    void DrawSeperationLine(lv_obj_t *parent, lv_color_t lineColor)
    {
      auto *line = lv_obj_create(parent);
      lv_obj_set_size(line, LineThickness, LineHeight);
      lv_obj_set_pos(line, GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + 1, GridSpacing + 1);

      lv_style_set_bg_color(&LineStyle, lineColor);
      lv_style_set_border_color(&LineStyle, lineColor);
      lv_style_set_radius(&LineStyle, 0);
      lv_obj_add_style(line, &LineStyle, LV_PART_MAIN);
    }

    void SetScreenBackground(lv_obj_t *screen, lv_color_t backgroundColor)
    {  
      lv_style_set_bg_color(&ScreenStyle, backgroundColor);
      lv_obj_add_style(screen, &ScreenStyle, LV_PART_MAIN);
    }

    void DrawTitles(lv_obj_t *parent, lv_color_t textColor)
    {
      lv_style_set_text_align(&TitleStyle, LV_TEXT_ALIGN_CENTER);
      lv_style_set_text_font(&TitleStyle, &lv_font_montserrat_28);
      lv_style_set_text_color(&TitleStyle, textColor);

      {
        auto *cpuTitle = lv_label_create(parent);
        lv_label_set_long_mode(cpuTitle, LV_LABEL_LONG_CLIP);
        lv_label_set_text(cpuTitle, "AMD Ryzen 7 5800X");
        lv_obj_set_size(cpuTitle, ArcWidth + GridSpacing + ArcWidth, TitleHight);
        lv_obj_set_pos(cpuTitle, GridSpacing + 1, GridSpacing + 1);
        lv_obj_add_style(cpuTitle, &TitleStyle, LV_PART_MAIN);
      }

      {
        auto *gpuTitle = lv_label_create(parent);
        lv_label_set_long_mode(gpuTitle, LV_LABEL_LONG_CLIP);
        lv_label_set_text(gpuTitle, "NVIDIA GeForce RTX 3070");
        lv_obj_set_size(gpuTitle, ArcWidth + GridSpacing + ArcWidth, TitleHight);
        lv_obj_set_pos(gpuTitle, GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1, GridSpacing + 1);
        lv_obj_add_style(gpuTitle, &TitleStyle, LV_PART_MAIN);
      }
    }

    void DrawArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor)
    {
      auto y0 = GridSpacing + TitleHight + 1;
      auto y1 = GridSpacing + TitleHight + ArcHeight + GridSpacing + 1;
     
      auto x0 = GridSpacing + 1;
      auto x1 = GridSpacing + ArcWidth + GridSpacing + 1;
      auto x2 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1;
      auto x3 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + ArcWidth + GridSpacing + 1;


      DrawArc(parent, 0, x0, y0, backgroundColor, emptyColor);
      DrawArc(parent, 1, x1, y0, backgroundColor, emptyColor);
      DrawArc(parent, 2, x0, y1, backgroundColor, emptyColor);
      DrawArc(parent, 3, x1, y1, backgroundColor, emptyColor);

      DrawArc(parent, 4, x2, y0, backgroundColor, emptyColor);
      DrawArc(parent, 5, x3, y0, backgroundColor, emptyColor);
      DrawArc(parent, 6, x2, y1, backgroundColor, emptyColor);
      DrawArc(parent, 7, x3, y1, backgroundColor, emptyColor);

      ArcUpdateEvent.Subscribe(OnArcUpdateEvent);
      Services::System::InvokeLater(&ArcUpdateEvent, ArcUpdateInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void DrawArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor)
    {
      auto gaugeStruct = &Gauges[index];

      {
        auto arc = lv_arc_create(parent);
        auto indicatorStyle = &gaugeStruct->IndicatorStyle;
        auto mainStyle = &gaugeStruct->MainStyle;
        auto indicatorAnimation = &gaugeStruct->IndicatorAnimation;

        gaugeStruct->Arc = arc;

        lv_obj_set_size(arc, ArcWidth, ArcWidth);
        lv_obj_set_pos(arc, x, y);
        lv_arc_set_end_angle(arc, 200);
        lv_arc_set_range(arc, 0, 32767);
        lv_arc_set_value(arc, 0);


        lv_style_set_bg_color(indicatorStyle, backgroundColor);
        lv_style_set_arc_color(indicatorStyle, emptyColor);
        lv_style_set_arc_width(indicatorStyle, ArcIndicatorSize);

        lv_style_set_bg_color(mainStyle, backgroundColor);
        lv_style_set_arc_color(mainStyle, emptyColor);
        lv_style_set_arc_width(mainStyle, ArcIndicatorSize);


        lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
        lv_obj_add_style(arc, indicatorStyle, LV_PART_INDICATOR);
        lv_obj_add_style(arc, mainStyle, LV_PART_MAIN);

        lv_anim_init(indicatorAnimation);
      }

      {
        auto title = lv_label_create(parent);
        auto titleStyle = &gaugeStruct->TitleStyle;
        auto titleText = gaugeStruct->TitleText;

        gaugeStruct->Title = title;

        lv_label_set_long_mode(title, LV_LABEL_LONG_CLIP);
        lv_label_set_text(title, titleText);
        lv_obj_set_size(title, ArcWidth, ArcTextHeight);
        lv_obj_set_pos(title, x, y + ArcHeight - ArcTextHeight);


        lv_style_set_text_align(titleStyle, LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_font(titleStyle, &lv_font_montserrat_20);
        lv_style_set_text_color(titleStyle, emptyColor);

        lv_obj_add_style(title, titleStyle, LV_PART_MAIN);
      }
    }

    void OnArcUpdateEvent(void *args)
    {
      for (size_t index = 0; index < GaugeCount; index++)
      {
        auto gaugeStruct = &Gauges[index];

        auto minValue = gaugeStruct->MinValue;
        auto maxValue = gaugeStruct->MaxValue;
        auto nextValue = random(minValue * 1000.0f, maxValue * 1000.0f) / 1000.0f;


        {
          auto arc = gaugeStruct->Arc;

          auto currentArcValue = lv_arc_get_value(arc);
          auto nextArcValue = (int16_t)Math::Map<float>(nextValue, gaugeStruct->MinValue, gaugeStruct->MaxValue, 0.0f, 32767.0f);

          auto indicatorAnimation = &gaugeStruct->IndicatorAnimation;

          lv_anim_set_exec_cb(indicatorAnimation, gaugeStruct->UpdateCallback); 
          lv_anim_set_values(indicatorAnimation, currentArcValue, nextArcValue);
          lv_anim_set_var(indicatorAnimation, arc);
          lv_anim_set_time(indicatorAnimation, 950);
          lv_anim_set_path_cb(indicatorAnimation, lv_anim_path_ease_in_out);

          lv_anim_start(indicatorAnimation);
        }
      }
    }

    void OnCpuUtilizationUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[0], value);
    }

    void OnCpuWattageUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[1], value);
    }
    
    void OnCpuTemperatureUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[2], value);
    }
    
    void OnCpuMemoryUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[3], value);
    }

    void OnGpuUtilizationUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[4], value);
    }

    void OnGpuWattageUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[5], value);
    }
    
    void OnGpuTemperatureUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[6], value);
    }
    
    void OnGpuMemoryUpdate(void *component, int32_t value)
    {
      OnAnimationUpdate(&Gauges[7], value);
    }

    void OnAnimationUpdate(Gauge *gaugeStruct, int32_t arcValue)
    {
      auto arc = gaugeStruct->Arc;
      auto indicatorStyle = &gaugeStruct->IndicatorStyle;

      auto hue = (uint16_t)Math::Map<int16_t>(arcValue, 0, 32767, 105, 0);
      auto color = lv_color_hsv_to_rgb(hue, 75, 92);


      lv_style_set_arc_color(indicatorStyle, color);

      lv_arc_set_value(arc, arcValue);
    }

  } // namespace Display
} // namespace Services