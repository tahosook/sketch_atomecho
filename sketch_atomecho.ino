
#include "M5Atom.h"
#include <driver/i2s.h>

#include "fft.h"
#include "slack.h"
#include "doorphone.h"

#define CONFIG_I2S_BCK_PIN 19
#define CONFIG_I2S_LRCK_PIN 33
#define CONFIG_I2S_DATA_PIN 22
#define CONFIG_I2S_DATA_IN_PIN 23

#define SPAKER_I2S_NUMBER I2S_NUM_0

#define MODE_MIC 0
#define MODE_SPK 1

//#define DEBUG(fmt, ...) (void(0))
#define DEBUG(fmt, ...) Serial.printf("DEBUG: %s(%d)" fmt "¥n", __FILE__, __LINE__, ##__VA_ARGS__)

const uint32_t DELAY_MSEC = 20; // FFT結果の確認頻度(msec)

extern const unsigned char audio[364808];

bool InitI2SSpakerOrMic(int mode)
{
    esp_err_t err = ESP_OK;

    i2s_driver_uninstall(SPAKER_I2S_NUMBER);
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
    };
    if (mode == MODE_MIC)
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    }
    else
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.use_apll = false;
        i2s_config.tx_desc_auto_clear = true;
    }

    //Serial.println("Init i2s_driver_install");

    err += i2s_driver_install(SPAKER_I2S_NUMBER, &i2s_config, 0, NULL);
    i2s_pin_config_t tx_pin_config;

    tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;
    tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;
    tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN;

    //Serial.println("Init i2s_set_pin");
    err += i2s_set_pin(SPAKER_I2S_NUMBER, &tx_pin_config);
    //Serial.println("Init i2s_set_clk");
    err += i2s_set_clk(SPAKER_I2S_NUMBER, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

    return true;
}

size_t bytes_written;
bool state = true;
bool loop_play = false;
void setup()
{
    M5.begin(true, false, true);
    M5.dis.drawpix(0, CRGB(128, 128, 0));

    pinMode(21, OUTPUT);
    pinMode(25, OUTPUT);

    digitalWrite(21, 0);
    digitalWrite(25, 1);

    InitI2SSpakerOrMic(MODE_SPK);
    if (M5.Btn.isPressed())
    {
        loop_play = true;
    }
    do
    {
        i2s_write(SPAKER_I2S_NUMBER, audio, 364808, &bytes_written, portMAX_DELAY);

    } while (loop_play);

    InitI2SSpakerOrMic(MODE_MIC);

    // Slack
    slack_setup();

    // Door phone Checker
    DP.init();
}

int8_t i2s_readraw_buff[1024];
size_t bytesread;
uint16_t count_n = 0;
double data = 0;
double maxData = 0;
uint16_t ydata;
float adc_data;
int16_t *buffptr;
double redatabuff[512] = {0};
uint16_t pos = 0;

void loop()
{
    // --- FFT
    fft_config_t *real_fft_plan = fft_init(512, FFT_REAL, FFT_FORWARD, NULL, NULL);
    i2s_read(I2S_NUM_0, (char *)i2s_readraw_buff, 1024, &bytesread, (100 / portTICK_RATE_MS));
    buffptr = (int16_t *)i2s_readraw_buff;

    for (count_n = 0; count_n < real_fft_plan->size; count_n++)
    {
        adc_data = (float)map(buffptr[count_n], INT16_MIN, INT16_MAX, -2000, 2000);
        real_fft_plan->input[count_n] = adc_data;
    }
    fft_execute(real_fft_plan);
    //Serial.printf("Size = %d\n",real_fft_plan->size);

    maxData = 0;
    for (count_n = 1; count_n < real_fft_plan->size / 4; count_n++)
    {
        data = sqrt(real_fft_plan->output[2 * count_n] * real_fft_plan->output[2 * count_n] + real_fft_plan->output[2 * count_n + 1] * real_fft_plan->output[2 * count_n + 1]);
        redatabuff[count_n] = data;
        if (data > maxData)
        {
            maxData = data;
            pos = count_n;
        }
    }
    // Serial.printf("pos=%d, maxData=%d\n", pos, maxData);
    fft_destroy(real_fft_plan);

    // --- PICK SOUND
    delay(DELAY_MSEC);

    DEBUG("DP.inc -> pos=%d", pos);
    DP.inc(pos);
    if (DP.isNeedCheck()) // データがたまったら
    {
        DEBUG("isNeedCheck");
        Serial.printf("%s\n", DP.dump().c_str()); // データダンプ

        if (DP.isNotice()) // 通知が必要なら
        {
            DEBUG("isNotice");
            String buf = "{\"text\":\":door: Door phone is ringed: data=";
            buf.concat(DP.dump());
            buf.concat("\"}");
            slack_senddata(buf);

            M5.dis.drawpix(0, CRGB(128, 0, 0));
        }
        else
        {
            M5.dis.drawpix(0, CRGB(0, 128, 0));
        }

        DEBUG("clear");
        DP.clear(); // データクリア
    }
    DEBUG("done");

    // --- Button control
    if (M5.Btn.wasPressed())
    {
        if (state)
        {
            digitalWrite(21, 0);
            digitalWrite(25, 1);
            state = false;
        }
        else
        {
            digitalWrite(21, 1);
            digitalWrite(25, 0);
            state = true;
        }
    }
    if (M5.Btn.isPressed())
    {
        M5.dis.drawpix(0, CRGB(0, 0, 128));
    }
    M5.update();
}
