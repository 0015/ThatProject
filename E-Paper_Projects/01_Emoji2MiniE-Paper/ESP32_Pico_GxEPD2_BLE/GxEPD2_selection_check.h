// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Display ePaper for Arduino: https://forum.arduino.cc/index.php?topic=436411.0

#define GxEPD2_102_IS_BW true
#define GxEPD2_154_IS_BW true
#define GxEPD2_154_D67_IS_BW true
#define GxEPD2_154_T8_IS_BW true
#define GxEPD2_154_M09_IS_BW true
#define GxEPD2_154_M10_IS_BW true
#define GxEPD2_213_IS_BW true
#define GxEPD2_213_B72_IS_BW true
#define GxEPD2_213_B73_IS_BW true
#define GxEPD2_213_B74_IS_BW true
#define GxEPD2_213_flex_IS_BW true
#define GxEPD2_213_M21_IS_BW true
#define GxEPD2_213_T5D_IS_BW true
#define GxEPD2_290_IS_BW true
#define GxEPD2_290_T5_IS_BW true
#define GxEPD2_290_T5D_IS_BW true
#define GxEPD2_290_T94_IS_BW true
#define GxEPD2_290_T94_V2_IS_BW true
#define GxEPD2_290_M06_IS_BW true
#define GxEPD2_260_IS_BW true
#define GxEPD2_260_M01_IS_BW true
#define GxEPD2_270_IS_BW true
#define GxEPD2_371_IS_BW true
#define GxEPD2_420_IS_BW true
#define GxEPD2_420_M01_IS_BW true
#define GxEPD2_583_IS_BW true
#define GxEPD2_583_T8_IS_BW true
#define GxEPD2_750_IS_BW true
#define GxEPD2_750_T7_IS_BW true
#define GxEPD2_1160_T91_IS_BW true
// 3-color e-papers
#define GxEPD2_154c_IS_3C true
#define GxEPD2_154_Z90c_IS_3C true
#define GxEPD2_213c_IS_3C true
#define GxEPD2_213_Z19c_IS_3C true
#define GxEPD2_290c_IS_3C true
#define GxEPD2_290_Z13c_IS_3C true
#define GxEPD2_290_C90c_IS_3C true
#define GxEPD2_270c_IS_3C true
#define GxEPD2_420c_IS_3C true
#define GxEPD2_583c_IS_3C true
#define GxEPD2_750c_IS_3C true
#define GxEPD2_750c_Z08_IS_3C true
#define GxEPD2_750c_Z90_IS_3C true
#define GxEPD2_1248_IS_3C true
// 7-color e-paper
#define GxEPD2_565c_IS_7C true

#if defined(GxEPD2_DISPLAY_CLASS) && defined(GxEPD2_DRIVER_CLASS)
#define IS_GxEPD2_DRIVER(c, x) (c##x)
#define IS_GxEPD2_DRIVER_BW(x) IS_GxEPD2_DRIVER(x, _IS_BW)
#define IS_GxEPD2_DRIVER_3C(x) IS_GxEPD2_DRIVER(x, _IS_3C)
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS) && IS_GxEPD2_DRIVER_3C(GxEPD2_DRIVER_CLASS)
#error "GxEPD2_BW used with 3-color driver class"
#endif
#if IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS) && IS_GxEPD2_DRIVER_BW(GxEPD2_DRIVER_CLASS)
#error "GxEPD2_3C used with b/w driver class"
#endif

#endif
