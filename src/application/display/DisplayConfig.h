#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_SSD1963 _panel_instance;

  lgfx::Bus_Parallel8 _bus_instance;

  //lgfx::Light_PWM _light_instance;

  //lgfx::Touch_XPT2046 _touch_instance;


public:

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.i2s_port = I2S_NUM_0;
      cfg.freq_write = 20000000;
      cfg.pin_wr =  4;
      cfg.pin_rd =  2;
      cfg.pin_rs = 15;
      cfg.pin_d0 = 12;
      cfg.pin_d1 = 13;
      cfg.pin_d2 = 26;
      cfg.pin_d3 = 25;
      cfg.pin_d4 = 17;
      cfg.pin_d5 = 16;
      cfg.pin_d6 = 27;
      cfg.pin_d7 = 14;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs           =     5;
      cfg.pin_rst          =    33;
      cfg.pin_busy         =    -1;

      cfg.memory_width     =   800;
      cfg.memory_height    =   480;
      cfg.panel_width      =   800;
      cfg.panel_height     =   480;
      cfg.offset_x         =     0;
      cfg.offset_y         =     0;
      cfg.offset_rotation  =     2;
      cfg.dummy_read_pixel =     8;
      cfg.dummy_read_bits  =     1;
      cfg.readable         =  true;
      cfg.invert           = false;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       =  true;

      _panel_instance.config(cfg);
    }

    //{
    //  auto cfg = _light_instance.config();
    //
    //  cfg.pin_bl = 32;
    //  cfg.invert = true;
    //  cfg.freq   = 44100;
    //  cfg.pwm_channel = 7;
    //
    //  _light_instance.config(cfg);
    //  _panel_instance.setLight(&_light_instance);
    //}

    //{
    //  auto cfg = _touch_instance.config();
    //
    //  cfg.x_min      = 0;
    //  cfg.x_max      = 239;
    //  cfg.y_min      = 0;
    //  cfg.y_max      = 319;
    //  cfg.pin_int    = 21;
    //  cfg.bus_shared = true;
    //  cfg.offset_rotation = 0;
    //
    //  cfg.spi_host = VSPI_HOST;
    //  cfg.freq = 1000000;
    //  cfg.pin_sclk = 18;
    //  cfg.pin_mosi = 23;
    //  cfg.pin_miso = 19;
    //  cfg.pin_cs   =  5;
    //
    //  _touch_instance.config(cfg);
    //  _panel_instance.setTouch(&_touch_instance);
    //}

    setPanel(&_panel_instance);
  }
};
