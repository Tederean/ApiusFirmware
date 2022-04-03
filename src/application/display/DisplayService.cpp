#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.h>

#include <application/display/DisplayConfig.h>
#include <lvgl.h>
#include <application/display/DisplayService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>


using namespace std;

namespace Services
{
  namespace Display
  {

    const timespan_t LvglRenderInterval_us = 5 * 1000;

    static const uint32_t ScreenWidth  = 800;

    static const uint32_t ScreenHeight = 480;


    LGFX tft;

    Event<void> LvglRenderEvent;


    static lv_disp_draw_buf_t DisplayBuffer;

    static lv_color_t ByteBuffer[ScreenWidth * 10];

    static lv_disp_drv_t DisplayDriver;


    void Initialize();

    void OnLvglRenderEvent(void *args);

    void OnDisplayFlush(lv_disp_drv_t *displayDriver, const lv_area_t *screenArea, lv_color_t *colorsPointer);


    void Initialize()
    {
      tft.begin();        

      lv_init();
      lv_disp_draw_buf_init(&DisplayBuffer, ByteBuffer, NULL, ScreenWidth * 10);


      lv_disp_drv_init(&DisplayDriver);

      DisplayDriver.hor_res = ScreenWidth;
      DisplayDriver.ver_res = ScreenHeight;
      DisplayDriver.flush_cb = OnDisplayFlush;
      DisplayDriver.draw_buf = &DisplayBuffer;

      lv_disp_drv_register(&DisplayDriver);


      LvglRenderEvent.Subscribe(OnLvglRenderEvent);
      Services::System::InvokeLater(&LvglRenderEvent, LvglRenderInterval_us, SchedulingBehaviour::FixedPeriodSkipTicks);
    }

    void OnLvglRenderEvent(void *args)
    {
      lv_timer_handler();
    }

    void OnDisplayFlush(lv_disp_drv_t *displayDriver, const lv_area_t *screenArea, lv_color_t *colorsPointer)
    {
      uint32_t width = (screenArea->x2 - screenArea->x1 + 1);
      uint32_t height = (screenArea->y2 - screenArea->y1 + 1);

      tft.startWrite();
      tft.setAddrWindow(screenArea->x1, screenArea->y1, width, height);

      tft.writePixels((lgfx::rgb565_t*)&colorsPointer->full, width * height);
      tft.endWrite();

      lv_disp_flush_ready(displayDriver);
    }

  } // namespace Display
} // namespace Services