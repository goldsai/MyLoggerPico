#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <tusb.h>

// Определение GPIO для параллельного интерфейса AD7606
#define ADC_FRST_GPIO       22          // First
#define ADC_CS_GPIO         21          // Chip Select
#define ADC_BUSY_GPIO       20          // Busy pin
#define ADC_RST_GPIO        19          // Reset Chip
#define ADC_RD_GPIO         18          // Read data
#define ADC_CONVST_A_GPIO   17          // Conversion start
#define ADC_CONVST_B_GPIO   16          // Conversion start

#define ADC_D0_GPIO         0
#define ADC_D1_GPIO         1
#define ADC_D2_GPIO         2
#define ADC_D3_GPIO         3
#define ADC_D4_GPIO         4
#define ADC_D5_GPIO         5
#define ADC_D6_GPIO         6
#define ADC_D7_GPIO         7
#define ADC_D8_GPIO         8
#define ADC_D9_GPIO         9
#define ADC_D10_GPIO        10
#define ADC_D11_GPIO        11
#define ADC_D12_GPIO        12
#define ADC_D13_GPIO        13
#define ADC_D14_GPIO        14
#define ADC_D15_GPIO        15

const uint8_t DB_NUM[] = {ADC_D0_GPIO, ADC_D1_GPIO, ADC_D2_GPIO, ADC_D3_GPIO,
                          ADC_D4_GPIO, ADC_D5_GPIO, ADC_D6_GPIO, ADC_D7_GPIO,
                          ADC_D8_GPIO, ADC_D9_GPIO, ADC_D10_GPIO, ADC_D1_GPIO,
                          ADC_D12_GPIO, ADC_D13_GPIO, ADC_D14_GPIO, ADC_D15_GPIO,
                          ADC_BUSY_GPIO, ADC_FRST_GPIO };
uint8_t ch;

void init_adc() {
    gpio_init(ADC_CS_GPIO);
    gpio_set_dir(ADC_CS_GPIO, GPIO_OUT);
    gpio_put(ADC_CS_GPIO, 1); // CS High

    gpio_init(ADC_RD_GPIO);
    gpio_set_dir(ADC_RD_GPIO, GPIO_OUT);
    gpio_put(ADC_RD_GPIO, 1); // RD High

    gpio_init(ADC_CONVST_A_GPIO);
    gpio_set_dir(ADC_CONVST_A_GPIO, GPIO_OUT);
    gpio_put(ADC_CONVST_A_GPIO, 1); // CONVST High

    gpio_init(ADC_CONVST_B_GPIO);
    gpio_set_dir(ADC_CONVST_B_GPIO, GPIO_OUT);
    gpio_put(ADC_CONVST_B_GPIO, 1); // CONVST High

    gpio_init(ADC_RST_GPIO);
    gpio_set_dir(ADC_RST_GPIO, GPIO_OUT);
    gpio_put(ADC_RST_GPIO, 0); // CONVST High

    gpio_init(ADC_BUSY_GPIO);
    gpio_set_dir(ADC_BUSY_GPIO, GPIO_IN);
    // for (uint8_t i = 0; i < 18; i++){
    //     gpio_init(DB_NUM[i]);
    //     gpio_set_dir(DB_NUM[i], GPIO_IN);
    // }
}

void reset_ad7606() {
    gpio_put(ADC_RST_GPIO, 1);
    sleep_ms(1);
    gpio_put(ADC_RST_GPIO, 0);
    sleep_ms(1);
}
int16_t read_adc_channel() {
    int16_t data = 0;


    gpio_put(ADC_RD_GPIO, 0);  // RD Low для начала чтения данных
    sleep_us(20);

                if (ch == 1){
                    if (gpio_get(ADC_FRST_GPIO)){printf("First V1 OK\n");}
                    else {printf("!!!!!!!!! First V1 BAD >>>>>>>>>>>\n");}

                }
    // Чтение данных с выходов DB0...DB15

    data |= gpio_get(ADC_D0_GPIO) << 0;
    data |= gpio_get(ADC_D1_GPIO) << 1;
    data |= gpio_get(ADC_D2_GPIO) << 2;
    data |= gpio_get(ADC_D3_GPIO) << 3;
    data |= gpio_get(ADC_D4_GPIO) << 4;
    data |= gpio_get(ADC_D5_GPIO) << 5;
    data |= gpio_get(ADC_D6_GPIO) << 6;
    data |= gpio_get(ADC_D7_GPIO) << 7;
    data |= gpio_get(ADC_D8_GPIO) << 8;
    data |= gpio_get(ADC_D9_GPIO) << 9;
    data |= gpio_get(ADC_D10_GPIO) << 10;
    data |= gpio_get(ADC_D11_GPIO) << 11;
    data |= gpio_get(ADC_D12_GPIO) << 12;
    data |= gpio_get(ADC_D13_GPIO) << 13;
    data |= gpio_get(ADC_D14_GPIO) << 14;
    data |= gpio_get(ADC_D15_GPIO) << 15;

    gpio_put(ADC_RD_GPIO, 1);  // RD High
    // sleep_us(10);

    return data;
}

void print_numeric(){
    for (uint8_t i = 0; i < 18; i++){
        if (i < 10) printf(" ");
        printf("%d ", i);
    }
    printf("\n");
}
void print_parallel(){
    for (uint8_t i = 0; i < 18; i++){
        // if (i < 10) printf(" ");
        printf(" %d ", gpio_get(DB_NUM[i]) ? 1 : 0);
    }
    printf("\n");
}
void start_conversion(){
    // Старт преобразования
    gpio_put(ADC_CONVST_A_GPIO, 0);
    gpio_put(ADC_CONVST_B_GPIO, 0);
    sleep_us(25);  // Небольшая задержка для завершения преобразования
    gpio_put(ADC_CONVST_A_GPIO, 1);
    gpio_put(ADC_CONVST_B_GPIO, 1);
}
int main() {
    stdio_init_all();
    while (!tud_cdc_connected()) { sleep_ms(100); }
    printf("tud_cdc_connected()\n");

    reset_ad7606();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    bool led_on = true;

    for(uint16_t i = 0; i < 5; i++){
        printf("Wait before start - %d\n", i);
        sleep_ms(500);
    }
    // Инициализация ADC
    init_adc();
    uint16_t count = 0;
    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
        count++;
        if (count>1){
            led_on = !led_on;
            count = 0;
        }
        printf("    "); print_numeric();
        for (uint8_t i = 0; i < 9; i++){
            printf("%d - "); print_parallel();
        }
        printf("\n Dates from ADC:\n");
        // 1234554321
        // 1234567890
        // 00: 65535 |
        for (uint8_t j = 1; j < 9; j++){
            if ( j < 10) printf(" ");
            printf("    %d    ", j);
        }
        printf("\n");
        for (uint8_t i = 0; i < 10; i++){
            // Старт преобразования
            start_conversion();

            // Ожидание готовности ADC
            while (gpio_get(ADC_BUSY_GPIO) == 1) {
                printf("wait\n");
                sleep_us(5);  // Небольшая задержка для сигнала занятости
            }

            gpio_put(ADC_CS_GPIO, 0);  // CS Low
            sleep_us(5);
            for( ch = 1; ch < 9; ch++){

                // Чтение данных с ADC
                int16_t data = read_adc_channel();
                uint64_t v = data;
                // Формула для перевода:
                //   V = (Vadc/65536 - 0.5) * 20000  - для диапазона +-10В
                //    V = ((Vadc - 35768) * 625) >> 11 - после преобразований
                //   V = (Vadc/65536 - 0.5) * 10000  - для диапазона +-5В
                //    V = ((Vadc - 35768) * 625) >> 12 - после преобразований
                bool sing = (v > 32768);
                if (sing) {v = v - 32768;}
                    else {v = 32768 - v;}
                v = v * 625;
                if (false) { v = v >> 12;}
                        else { v = v >> 11;}
                int32_t d = v;
                if (!sing) {d= -d;}
                // Вывод данных в Serial Monitor
                // printf("V%d = %d\t\t%ld\n", ch, data, d);
                if (ch < 10) printf(" ");
                if (data < 10)         printf("%d:     %d ", ch, data);
                else if (data < 100)   printf("%d:    %d ", ch, data);
                else if (data < 1000)  printf("%d:   %d ", ch, data);
                else if (data < 10000) printf("%d:  %d ", ch, data);
                else                   printf("%d: %d ", ch, data);


            }
            printf("\n");
            gpio_put(ADC_CS_GPIO, 1);  // CS High
            sleep_us(200);
        }
        // Задержка перед следующим циклом
        sleep_ms(10000);  // Настройка задержки для требуемой частоты выборки
    }
}
