#define I2S_WS_TX  12
#define I2S_SCK_TX 13
#define I2S_DATA_OUT_TX  15
#define I2S_PORT_TX I2S_NUM_0

#define I2S_WS_RX  33
#define I2S_SCK_RX 25
#define I2S_SD_RX  26
#define I2S_PORT_RX I2S_NUM_1

#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (32)
#define I2S_READ_LEN      (1024)

char* i2s_read_buff;
uint8_t* flash_write_buff;

const i2s_config_t i2s_config_tx = {
  .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate = I2S_SAMPLE_RATE,
  .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 32,
  .dma_buf_len = 64
};

const i2s_pin_config_t pin_config_tx = {
  .bck_io_num = I2S_SCK_TX,
  .ws_io_num = I2S_WS_TX,
  .data_out_num = I2S_DATA_OUT_TX,
  .data_in_num = I2S_PIN_NO_CHANGE
};

const i2s_config_t i2s_config_rx = {
  .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = I2S_SAMPLE_RATE,
  .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 32,
  .dma_buf_len = 64
};

const i2s_pin_config_t pin_config_rx = {
  .bck_io_num = I2S_SCK_RX,
  .ws_io_num = I2S_WS_RX,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_SD_RX
};

void i2s_RX_init(){
  i2s_driver_install(I2S_PORT_RX, &i2s_config_rx, 0, NULL);
  i2s_set_pin(I2S_PORT_RX, &pin_config_rx);
}

void i2s_RX_uninst(){
  i2s_driver_uninstall(I2S_PORT_RX);
}

void i2s_TX_init(){
  i2s_driver_install(I2S_PORT_TX, &i2s_config_tx, 0, NULL);
  i2s_set_pin(I2S_PORT_TX, &pin_config_tx);
}

void i2s_TX_uninst(){
  i2s_driver_uninstall(I2S_PORT_TX);
}

void i2s_adc_data_scale(uint8_t * d_buff, uint8_t* s_buff, uint32_t len){
  uint32_t j = 0;
  uint32_t dac_value = 0;
  for (int i = 0; i < len; i += 2) {
      dac_value = ((((uint16_t) (s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
      d_buff[j++] = 0;
      d_buff[j++] = dac_value * 256 / 1024 ;
  }
}

void i2s_read_data(){
  size_t bytes_read;
  i2s_read(I2S_PORT_RX, (void*) i2s_read_buff, I2S_READ_LEN, &bytes_read, portMAX_DELAY);
  i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2s_read_buff, I2S_READ_LEN);
}

void i2s_write_data(char *buf_ptr, int buf_size){
  i2s_write_bytes(I2S_PORT_TX, buf_ptr, buf_size, portMAX_DELAY);
}

void i2s_buff_init(){
  i2s_read_buff = (char*) calloc(I2S_READ_LEN, sizeof(char));
  flash_write_buff = (uint8_t*) calloc(I2S_READ_LEN, sizeof(char));
}
