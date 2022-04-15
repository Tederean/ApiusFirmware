#include <Arduino.h>

#include <lvgl.h>
#include <application/gui/GuiService.h>
#include <application/communication/CommunicationService.h>
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

    void InitializeTitles(lv_obj_t *parent, lv_color_t textColor);

    void InitializeArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor);

    void InitializeArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor);


    void OnRecievedInitializationCommandEvent(InitializationCommand *initializationCommand);

    void OnRecievedUpdateCommandEvent(UpdateCommand *updateCommand);

    void UpdateChart(Gauge *gaugeStruct, int16_t nextArcValue, const char *valueText);

    void OnAnimationUpdate(void *component, int32_t value);


    Gauge Gauges[8];

    const size_t GaugeCount = sizeof(Gauges) / sizeof(Gauge);


    lv_obj_t *Title0;

    lv_obj_t *Title1;


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

      InitializeTitles(screen, textColor);

      InitializeArcs(screen, backgroundColor, arcEmptyColor, arcFillColor, textColor);


      Services::Communication::RecievedInitializationCommandEvent.Subscribe(OnRecievedInitializationCommandEvent);
      Services::Communication::RecievedUpdateCommandEvent.Subscribe(OnRecievedUpdateCommandEvent);
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

    void InitializeTitles(lv_obj_t *parent, lv_color_t textColor)
    {
      lv_style_set_text_align(&TitleStyle, LV_TEXT_ALIGN_CENTER);
      lv_style_set_text_font(&TitleStyle, &lv_font_montserrat_28);
      lv_style_set_text_color(&TitleStyle, textColor);


      Title0 = lv_label_create(parent);

      lv_label_set_long_mode(Title0, LV_LABEL_LONG_CLIP);
      lv_label_set_text(Title0, "");
      lv_obj_set_size(Title0, ArcWidth + GridSpacing + ArcWidth, TitleHeight);
      lv_obj_set_pos(Title0, GridSpacing + 1, GridSpacing + 1);
      lv_obj_add_style(Title0, &TitleStyle, LV_PART_MAIN);

  
      Title1 = lv_label_create(parent);

      lv_label_set_long_mode(Title1, LV_LABEL_LONG_CLIP);
      lv_label_set_text(Title1, "");
      lv_obj_set_size(Title1, ArcWidth + GridSpacing + ArcWidth, TitleHeight);
      lv_obj_set_pos(Title1, GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1, GridSpacing + 1);
      lv_obj_add_style(Title1, &TitleStyle, LV_PART_MAIN);
    }

    void InitializeArcs(lv_obj_t *parent, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor)
    {
      auto y0 = GridSpacing + TitleHeight + 1;
      auto y1 = GridSpacing + TitleHeight + ArcHeight + GridSpacing + 1;
     
      auto x0 = GridSpacing + 1;
      auto x1 = GridSpacing + ArcWidth + GridSpacing + 1;
      auto x2 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + 1;
      auto x3 = GridSpacing + ArcWidth + GridSpacing + ArcWidth + GridSpacing + LineThickness + GridSpacing + ArcWidth + GridSpacing + 1;


      InitializeArc(parent, 0, x0, y0, backgroundColor, emptyColor, fillColor, textColor);
      InitializeArc(parent, 1, x1, y0, backgroundColor, emptyColor, fillColor, textColor);
      InitializeArc(parent, 2, x0, y1, backgroundColor, emptyColor, fillColor, textColor);
      InitializeArc(parent, 3, x1, y1, backgroundColor, emptyColor, fillColor, textColor);

      InitializeArc(parent, 4, x2, y0, backgroundColor, emptyColor, fillColor, textColor);
      InitializeArc(parent, 5, x3, y0, backgroundColor, emptyColor, fillColor, textColor);
      InitializeArc(parent, 6, x2, y1, backgroundColor, emptyColor, fillColor, textColor);
      InitializeArc(parent, 7, x3, y1, backgroundColor, emptyColor, fillColor, textColor);
    }

    void InitializeArc(lv_obj_t *parent, size_t index, lv_coord_t x, lv_coord_t y, lv_color_t backgroundColor, lv_color_t emptyColor, lv_color_t fillColor, lv_color_t textColor)
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

        gaugeStruct->TitleLabel = titleLabel;

        lv_label_set_long_mode(titleLabel, LV_LABEL_LONG_CLIP);
        lv_label_set_text(titleLabel, "");
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


    void OnRecievedInitializationCommandEvent(InitializationCommand *initializationCommand)
    {
      lv_label_set_text(Title0, initializationCommand->Tile0);
      lv_label_set_text(Title1, initializationCommand->Tile1);

      lv_label_set_text(Gauges[0].TitleLabel, initializationCommand->Chart0);
      lv_label_set_text(Gauges[1].TitleLabel, initializationCommand->Chart1);
      lv_label_set_text(Gauges[2].TitleLabel, initializationCommand->Chart2);
      lv_label_set_text(Gauges[3].TitleLabel, initializationCommand->Chart3);

      lv_label_set_text(Gauges[4].TitleLabel, initializationCommand->Chart4);
      lv_label_set_text(Gauges[5].TitleLabel, initializationCommand->Chart5);
      lv_label_set_text(Gauges[6].TitleLabel, initializationCommand->Chart6);
      lv_label_set_text(Gauges[7].TitleLabel, initializationCommand->Chart7);
    }

    void OnRecievedUpdateCommandEvent(UpdateCommand *updateCommand)
    {
      UpdateChart(&Gauges[0], updateCommand->Ratio0, updateCommand->Text0);
      UpdateChart(&Gauges[1], updateCommand->Ratio1, updateCommand->Text1);
      UpdateChart(&Gauges[2], updateCommand->Ratio2, updateCommand->Text2);
      UpdateChart(&Gauges[3], updateCommand->Ratio3, updateCommand->Text3);

      UpdateChart(&Gauges[4], updateCommand->Ratio4, updateCommand->Text4);
      UpdateChart(&Gauges[5], updateCommand->Ratio5, updateCommand->Text5);
      UpdateChart(&Gauges[6], updateCommand->Ratio6, updateCommand->Text6);
      UpdateChart(&Gauges[7], updateCommand->Ratio7, updateCommand->Text7);
    }

    void UpdateChart(Gauge *gaugeStruct, int16_t nextArcValue, const char *valueText)
    {
      lv_label_set_text(gaugeStruct->ValueLabel, valueText);


      auto arc = gaugeStruct->Arc;
      auto currentArcValue = lv_arc_get_value(arc);
      auto indicatorAnimation = &gaugeStruct->IndicatorAnimation;

      lv_anim_set_exec_cb(indicatorAnimation, OnAnimationUpdate); 
      lv_anim_set_values(indicatorAnimation, currentArcValue, nextArcValue);
      lv_anim_set_var(indicatorAnimation, arc);
      lv_anim_set_time(indicatorAnimation, 950);
      lv_anim_set_path_cb(indicatorAnimation, lv_anim_path_ease_in_out);

      lv_anim_start(indicatorAnimation);
    }

    void OnAnimationUpdate(void *component, int32_t value)
    {
      auto arc = (lv_obj_t*)component;

      lv_arc_set_value(arc, value);
    }
  }
}