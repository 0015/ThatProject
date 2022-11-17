/////////////////////////////////////////////////////////////////
/*
  Who Needs High-Performance MCU? (Arduino Portenta H7 vs Espressif ESP32-S3)
  For More Information: https://youtu.be/3m73ly4fjk8
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <esp_task_wdt.h>
#include <Arduino_GFX_Library.h>
Arduino_DataBus *bus = new Arduino_HWSPI(7, 10);
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 0, false);

int percent_core_0, percent_core_1 = 0;
int prev_percent_core_0, prev_percent_core_1 = 0;
int screen_draw_time_core_0, screen_draw_time_core_1 = 0;

//Calculation of Pi 
int iterations = 10000000;

//Find Prime Numbers
int iterationsForPrime = 300000;
int foundPrime = 0;

void setup() {
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->setRotation(2);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(0, 10);
  gfx->println("Dual Core Test");
  gfx->println("ESP32-S3");
  gfx->setTextSize(4);
  gfx->setCursor(gfx->width() - 60, 12);
  gfx->setTextColor(GREEN);
  gfx->print("S3");

  xTaskCreatePinnedToCore(Task0code, "Task0", 10000, NULL, 1, NULL, 0);
  delay(100);
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, NULL, 1);
}

void loop() {}

void Task0code(void *pvParameters) {
  esp_task_wdt_init(600, false);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(0, gfx->height() / 2 - 100);
  gfx->print("Core:");
  gfx->println(xPortGetCoreID());
  gfx->setTextSize(1);
  gfx->print("Calculation of Pi(Leibniz Formula)");

  unsigned long start = millis();
  double calc_pi = calculate_pi(iterations, xPortGetCoreID());
  unsigned long diffTime = millis() - start - screen_draw_time_core_0;

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(0, gfx->height() / 2 - 20);
  gfx->print("Elapsed Time:");
  gfx->print(diffTime);
  gfx->println("ms");
  vTaskDelete(NULL);
}

void Task1code(void *pvParameters) {
  esp_task_wdt_init(600, false);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(0, gfx->height() / 2 + 10);
  gfx->print("Core:");
  gfx->println(xPortGetCoreID());
  gfx->setTextSize(1);
  gfx->print("Find Prime Numbers");

  unsigned long start = millis();
  findPrimeNumbers(iterationsForPrime, 1);
  unsigned long diffTime = millis() - start - screen_draw_time_core_1;

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(0, gfx->height() / 2 + 100);
  gfx->print("Elapsed Time:");
  gfx->print(diffTime);
  gfx->println("ms");
  vTaskDelete(NULL);
}

double calculate_pi(int n, int core) {
  double sum = 0.0, term, pi;
  
  for (int i = 0; i < n; i++) {
    term = pow(-1, i) / (2 * i + 1);
    sum += term;

    updateProgress(i, n - 1, core);
  }
  pi = 4 * sum;
  return pi;
}


void findPrimeNumbers(int num, int core) {
  for (int i = 2; i < num; ++i)
  {
    int flag_var = 0;
    for (int j = 2; j <= i / 2; ++j)
    {
      if (i % j == 0)
      {
        flag_var = 1;
        break;
      }
    }
    if (flag_var == 0) {
      foundPrime++;
    }

    updateProgress(i, num - 1, core );
  }
}

void updateProgress(int i, int n, int core) {

  if (core == 0) {
    int percent_core_0 = i * 100 / n;

    if (percent_core_0 != prev_percent_core_0) {
      unsigned long draw_start = millis();
      drawProgressBar(20, gfx->height() / 2 - 60, gfx->width() - 50, 20, percent_core_0, RED, GREEN);
      screen_draw_time_core_0 += millis() - draw_start;
    }
    prev_percent_core_0 = percent_core_0;
  } else {
    int percent_core_1 = i * 100 / n;

    if (percent_core_1 != prev_percent_core_1) {
      unsigned long draw_start = millis();
      drawProgressBar(20, gfx->height() / 2 + 60, gfx->width() - 50, 20, percent_core_1, RED, GREEN);
      screen_draw_time_core_1 += millis() - draw_start;
    }

    prev_percent_core_1 = percent_core_1;
  }
}

void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t frameColor, uint16_t barColor) {
  uint8_t margin = 2;
  uint16_t barHeight = h - 2 * margin;
  uint16_t barWidth = w - 2 * margin;
  gfx->drawRoundRect(x, y, w, h, 3, frameColor);
  gfx->fillRect(x + margin, y + margin, barWidth * percent / 100.0, barHeight, barColor);
}