#include <Arduino.h>

#include <lvgl.h>
#include <application/gui/GuiService.h>
#include <application/utils/Formatter.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>
#include <framework/utils/Math.h>

using namespace std;

namespace Services
{
  namespace Gui
  {

    typedef enum RoundMode
    {
      Integer,
      ThreeDigitsMegaByte,
    } RoundMode;

    typedef struct Gauge
    {
      const char *TitleText;
      const char *Unit;
      const float MinValue;
      const float MaxValue;
      const RoundMode ValueRounding;

      lv_obj_t *Arc;
      lv_style_t IndicatorStyle;
      lv_style_t MainStyle;
      lv_anim_t IndicatorAnimation;

      lv_obj_t *TitleLabel;
      lv_style_t TitleStyle;

      lv_obj_t *ValueLabel;
      lv_style_t ValueStyle;
    } Gauge;



    void Initialize();

    void DrawSeperationLine(lv_obj_t *parent, lv_color_t lineColor);

    void SetScreenBackground(lv_obj_t *screen, lv_color_t backgroundColor);

    void DrawTitles(lv_obj_t *parent, lv_color_t textColor);

    void DrawArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor);

    void DrawArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor);

    void OnArcUpdateEvent(void *args);

    void OnAnimationUpdate(void *component, int32_t value);



    const timespan_t ArcUpdateInterval_us = 1 * 1000 * 1000;

    const size_t GaugeCount = 8;

    Gauge Gauges[] =
    {
      {
        .TitleText = "Utilization",
        .Unit = "%",
        .MinValue = 0.0f,
        .MaxValue = 100.0f,
        .ValueRounding = RoundMode::Integer,
      },

      {
        .TitleText = "Wattage",
        .Unit = "W",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .ValueRounding = RoundMode::Integer,
      },

      {
        .TitleText = "Temperature",
        .Unit = "°C",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .ValueRounding = RoundMode::Integer,
      },

      {
        .TitleText = "Memory",
        .Unit = "B",
        .MinValue = 0.0f,
        .MaxValue = 32768.0f,
        .ValueRounding = RoundMode::ThreeDigitsMegaByte,
      },

      {
        .TitleText = "Utilization",
        .Unit = "%",
        .MinValue = 0.0f,
        .MaxValue = 100.0f,
        .ValueRounding = RoundMode::Integer,
      },

      {
        .TitleText = "Wattage",
        .Unit = "W",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .ValueRounding = RoundMode::Integer,
      },

      {
        .TitleText = "Temperature",
        .Unit = "°C",
        .MinValue = 0.0f,
        .MaxValue = 105.0f,
        .ValueRounding = RoundMode::Integer,
      },

      {
        .TitleText = "Memory",
        .Unit = "B",
        .MinValue = 0.0f,
        .MaxValue = 8192.0f,
        .ValueRounding = RoundMode::ThreeDigitsMegaByte,
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

    const lv_coord_t TitleHeight = 49;

    const lv_coord_t ValueHeight = 40;
    
    const lv_coord_t GridSpacing = 17;

    const lv_coord_t LineThickness = 2;

    const lv_coord_t LineHeight = 480 - GridSpacing - GridSpacing;



    void Initialize()
    {
      auto *display = lv_disp_get_default();
      auto *screen = lv_disp_get_scr_act(display);


      //auto white = lv_color_make(0xFF, 0xFF, 0xFF);
      //auto gray100 = lv_color_make(0xF6, 0xF7, 0xF8);
      //auto gray200 = lv_color_make(0xEB, 0xEC, 0xEF);
      auto gray300 = lv_color_make(0xCE, 0xD2, 0xD9);
      //auto gray400 = lv_color_make(0xB2, 0xB8, 0xC2);
      auto gray500 = lv_color_make(0x95, 0x9E, 0xAC);
      //auto gray600 = lv_color_make(0x78, 0x83, 0x96);
      //auto gray700 = lv_color_make(0x60, 0x6A, 0x7B);
      auto gray800 = lv_color_make(0x4A, 0x51, 0x5E);
      //auto gray900 = lv_color_make(0x33, 0x39, 0x42);
      //auto black = lv_color_make(0x00, 0x00, 0x00);


      auto textColor = gray500;
      auto arcEmptyColor = gray800;
      auto arcFillColor = gray300;
      auto backgroundColor = lv_color_make(0x33, 0x39, 0x42);


      SetScreenBackground(screen, backgroundColor);

      DrawSeperationLine(screen, gray800);

      DrawTitles(screen, textColor);

      DrawArcs(screen, backgroundColor, arcEmptyColor, arcFillColor, textColor);
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
        lv_obj_set_size(cpuTitle, ArcWidth + GridSpacing + ArcWidth, TitleHeight);
        lv_obj_set_pos(cpuTitle, GridSpacing + 1, GridSpacing + 1);
        lv_obj_add_style(cpuTitle, &TitleStyle, LV_PART_MAIN);
      }

      {
        auto *gpuTitle = lv_label_create(parent);
        lv_label_set_long_mode(gpuTitle, LV_LABEL_LONG_CLIP);
        lv_label_set_text(gpuTitle, "NVIDIA GeForce RTX 3070");
        lv_obj_set_size(gpuTitle, ArcWidth + GridSpacing + ArcWidth, TitleHeight);
        lv_obj_set_pos(gpuTitle, GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1, GridSpacing + 1);
        lv_obj_add_style(gpuTitle, &TitleStyle, LV_PART_MAIN);
      }
    }

    void DrawArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor)
    {
      auto y0 = GridSpacing + TitleHeight + 1;
      auto y1 = GridSpacing + TitleHeight + ArcHeight + GridSpacing + 1;
     
      auto x0 = GridSpacing + 1;
      auto x1 = GridSpacing + ArcWidth + GridSpacing + 1;
      auto x2 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1;
      auto x3 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + ArcWidth + GridSpacing + 1;


      DrawArc(parent, 0, x0, y0, backgroundColor, emptyColor, fillColor, textColor);
      DrawArc(parent, 1, x1, y0, backgroundColor, emptyColor, fillColor, textColor);
      DrawArc(parent, 2, x0, y1, backgroundColor, emptyColor, fillColor, textColor);
      DrawArc(parent, 3, x1, y1, backgroundColor, emptyColor, fillColor, textColor);

      DrawArc(parent, 4, x2, y0, backgroundColor, emptyColor, fillColor, textColor);
      DrawArc(parent, 5, x3, y0, backgroundColor, emptyColor, fillColor, textColor);
      DrawArc(parent, 6, x2, y1, backgroundColor, emptyColor, fillColor, textColor);
      DrawArc(parent, 7, x3, y1, backgroundColor, emptyColor, fillColor, textColor);

      ArcUpdateEvent.Subscribe(OnArcUpdateEvent);
      Services::System::InvokeLater(&ArcUpdateEvent, ArcUpdateInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void DrawArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor)
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
        lv_style_set_arc_color(indicatorStyle, fillColor);
        lv_style_set_arc_width(indicatorStyle, ArcIndicatorSize);

        lv_style_set_bg_color(mainStyle, backgroundColor);
        lv_style_set_arc_color(mainStyle, emptyColor);
        lv_style_set_arc_width(mainStyle, ArcIndicatorSize);


        lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
        lv_obj_add_style(arc, indicatorStyle, LV_PART_INDICATOR);
        lv_obj_add_style(arc, mainStyle, LV_PART_MAIN);

        lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);

        lv_anim_init(indicatorAnimation);
      }

      {
        auto titleLabel = lv_label_create(parent);
        auto titleStyle = &gaugeStruct->TitleStyle;
        auto titleText = gaugeStruct->TitleText;

        gaugeStruct->TitleLabel = titleLabel;

        lv_label_set_long_mode(titleLabel, LV_LABEL_LONG_CLIP);
        lv_label_set_text(titleLabel, titleText);
        lv_obj_set_size(titleLabel, ArcWidth, ArcTextHeight);
        lv_obj_set_pos(titleLabel, x, y + ArcHeight - ArcTextHeight);


        lv_style_set_text_align(titleStyle, LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_font(titleStyle, &lv_font_montserrat_20);
        lv_style_set_text_color(titleStyle, textColor);

        lv_obj_add_style(titleLabel, titleStyle, LV_PART_MAIN);
      }

      {
        auto valueLabel = lv_label_create(parent);
        auto valueStyle = &gaugeStruct->ValueStyle;

        gaugeStruct->ValueLabel = valueLabel;

        lv_label_set_long_mode(valueLabel, LV_LABEL_LONG_CLIP);
        lv_label_set_text(valueLabel, "");

        lv_obj_set_size(valueLabel, ArcWidth, ValueHeight);
        lv_obj_set_pos(valueLabel, x, y + ((ArcWidth - ValueHeight)/2));

        lv_style_set_text_align(valueStyle, LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_font(valueStyle, &lv_font_montserrat_28);
        lv_style_set_text_color(valueStyle, fillColor);

        lv_obj_add_style(valueLabel, valueStyle, LV_PART_MAIN);
      }
    }

    void OnArcUpdateEvent(void *args)
    {
      for (size_t index = 0; index < GaugeCount; index++)
      {
        auto gaugeStruct = &Gauges[index];

        auto minValue = gaugeStruct->MinValue;
        auto maxValue = gaugeStruct->MaxValue;
        auto nextValue = random(minValue * 100.0f, maxValue * 100.0f) / 100.0f;

        {
          auto valueLabel = gaugeStruct->ValueLabel;


          if (gaugeStruct->ValueRounding == RoundMode::Integer)
          {
            char stringBuilder[10];

            sprintf(stringBuilder, "%.0f ", nextValue);
            strcat(stringBuilder, gaugeStruct->Unit);

            lv_label_set_text(valueLabel, stringBuilder);
          }

          if (gaugeStruct->ValueRounding == RoundMode::ThreeDigitsMegaByte)
          {
            char formattedValue[15];

            Formatter::FormatForBinary(nextValue * 1024.0f * 1024.f, gaugeStruct->Unit, formattedValue);

            lv_label_set_text(valueLabel, formattedValue);
          }
        }

        {
          auto arc = gaugeStruct->Arc;

          auto currentArcValue = lv_arc_get_value(arc);
          auto mappedNextValue = Math::Map<float>(nextValue, gaugeStruct->MinValue, gaugeStruct->MaxValue, 0.0f, 32767.0f);
          auto nextArcValue = (int16_t)Math::Clamp<float>(roundf(mappedNextValue), 0.0f, 32767.0f);


          auto indicatorAnimation = &gaugeStruct->IndicatorAnimation;

          lv_anim_set_exec_cb(indicatorAnimation, OnAnimationUpdate); 
          lv_anim_set_values(indicatorAnimation, currentArcValue, nextArcValue);
          lv_anim_set_var(indicatorAnimation, arc);
          lv_anim_set_time(indicatorAnimation, 950);
          lv_anim_set_path_cb(indicatorAnimation, lv_anim_path_ease_in_out);

          lv_anim_start(indicatorAnimation);
        }
      }
    }

    void OnAnimationUpdate(void *component, int32_t value)
    {
      auto arc = (lv_obj_t*)component;

      lv_arc_set_value(arc, value);
    }

  } // namespace Display
} // namespace Services