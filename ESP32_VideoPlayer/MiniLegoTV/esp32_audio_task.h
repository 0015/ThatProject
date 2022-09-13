#include "driver/i2s.h"

#include "AACDecoderHelix.h"
#include "MP3DecoderHelix.h"

static unsigned long total_read_audio_ms = 0;
static unsigned long total_decode_audio_ms = 0;
static unsigned long total_play_audio_ms = 0;

static i2s_port_t _i2s_num;
static esp_err_t i2s_init(i2s_port_t i2s_num, uint32_t sample_rate,
                          int mck_io_num,   /*!< MCK in out pin. Note that ESP32 supports setting MCK on GPIO0/GPIO1/GPIO3 only*/
                          int bck_io_num,   /*!< BCK in out pin*/
                          int ws_io_num,    /*!< WS in out pin*/
                          int data_out_num, /*!< DATA out pin*/
                          int data_in_num   /*!< DATA in pin*/
)
{
    _i2s_num = i2s_num;

    esp_err_t ret_val = ESP_OK;

    i2s_config_t i2s_config;
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.sample_rate = sample_rate;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    i2s_config.dma_buf_count = 8;
    i2s_config.dma_buf_len = 160;
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = true;
    i2s_config.fixed_mclk = 0;
    i2s_config.mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT;
    i2s_config.bits_per_chan = I2S_BITS_PER_CHAN_16BIT;

    i2s_pin_config_t pin_config;
    pin_config.mck_io_num = mck_io_num;
    pin_config.bck_io_num = bck_io_num;
    pin_config.ws_io_num = ws_io_num;
    pin_config.data_out_num = data_out_num;
    pin_config.data_in_num = data_in_num;

    ret_val |= i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    ret_val |= i2s_set_pin(i2s_num, &pin_config);

    return ret_val;
}

static int _samprate = 0;
static void aacAudioDataCallback(AACFrameInfo &info, int16_t *pwm_buffer, size_t len)
{
    unsigned long s = millis();
    if (_samprate != info.sampRateOut)
    {
        // log_i("bitRate: %d, nChans: %d, sampRateCore: %d, sampRateOut: %d, bitsPerSample: %d, outputSamps: %d, profile: %d, tnsUsed: %d, pnsUsed: %d",
        //       info.bitRate, info.nChans, info.sampRateCore, info.sampRateOut, info.bitsPerSample, info.outputSamps, info.profile, info.tnsUsed, info.pnsUsed);
        i2s_set_clk(_i2s_num, info.sampRateOut /* sample_rate */, info.bitsPerSample /* bits_cfg */, (info.nChans == 2) ? I2S_CHANNEL_STEREO : I2S_CHANNEL_MONO /* channel */);
        _samprate = info.sampRateOut;
    }
    size_t i2s_bytes_written = 0;
    i2s_write(_i2s_num, pwm_buffer, len * 2, &i2s_bytes_written, portMAX_DELAY);
    // log_i("len: %d, i2s_bytes_written: %d", len, i2s_bytes_written);
    total_play_audio_ms += millis() - s;
}
static void mp3AudioDataCallback(MP3FrameInfo &info, int16_t *pwm_buffer, size_t len)
{
    unsigned long s = millis();
    if (_samprate != info.samprate)
    {
        log_i("bitrate: %d, nChans: %d, samprate: %d, bitsPerSample: %d, outputSamps: %d, layer: %d, version: %d",
              info.bitrate, info.nChans, info.samprate, info.bitsPerSample, info.outputSamps, info.layer, info.version);
        i2s_set_clk(_i2s_num, info.samprate /* sample_rate */, info.bitsPerSample /* bits_cfg */, (info.nChans == 2) ? I2S_CHANNEL_STEREO : I2S_CHANNEL_MONO /* channel */);
        _samprate = info.samprate;
    }
    size_t i2s_bytes_written = 0;
    i2s_write(_i2s_num, pwm_buffer, len * 2, &i2s_bytes_written, portMAX_DELAY);
    // log_i("len: %d, i2s_bytes_written: %d", len, i2s_bytes_written);
    total_play_audio_ms += millis() - s;
}

static uint8_t _frame[MP3_MAX_FRAME_SIZE]; // MP3_MAX_FRAME_SIZE is smaller, so always use MP3_MAX_FRAME_SIZE

static libhelix::AACDecoderHelix _aac(aacAudioDataCallback);
static void aac_player_task(void *pvParam)
{
    Stream *input = (Stream *)pvParam;

    int r, w;
    unsigned long ms = millis();
    while (r = input->readBytes(_frame, MP3_MAX_FRAME_SIZE))
    {
        total_read_audio_ms += millis() - ms;
        ms = millis();

        while (r > 0)
        {
            w = _aac.write(_frame, r);
            // log_i("r: %d, w: %d\n", r, w);
            r -= w;
        }
        total_decode_audio_ms += millis() - ms;
        ms = millis();
    }
    log_i("AAC stop.");

    vTaskDelete(NULL);
}

static libhelix::MP3DecoderHelix _mp3(mp3AudioDataCallback);
static void mp3_player_task(void *pvParam)
{
    Stream *input = (Stream *)pvParam;

    int r, w;
    unsigned long ms = millis();
    while (r = input->readBytes(_frame, MP3_MAX_FRAME_SIZE))
    {
        total_read_audio_ms += millis() - ms;
        ms = millis();

        while (r > 0)
        {
            w = _mp3.write(_frame, r);
            // log_i("r: %d, w: %d\n", r, w);
            r -= w;
        }
        total_decode_audio_ms += millis() - ms;
        ms = millis();
    }
    log_i("MP3 stop.");

    vTaskDelete(NULL);
}

static BaseType_t aac_player_task_start(Stream *input, BaseType_t audioAssignCore)
{
    _aac.begin();

    return xTaskCreatePinnedToCore(
        (TaskFunction_t)aac_player_task,
        (const char *const)"AAC Player Task",
        (const uint32_t)2000,
        (void *const)input,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        (TaskHandle_t *const)NULL,
        (const BaseType_t)audioAssignCore);
}

static BaseType_t mp3_player_task_start(Stream *input, BaseType_t audioAssignCore)
{
    _mp3.begin();

    return xTaskCreatePinnedToCore(
        (TaskFunction_t)mp3_player_task,
        (const char *const)"MP3 Player Task",
        (const uint32_t)2000,
        (void *const)input,
        (UBaseType_t)configMAX_PRIORITIES - 1,
        (TaskHandle_t *const)NULL,
        (const BaseType_t)audioAssignCore);
}