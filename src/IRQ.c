/* MIT License
   Copyright (c) 2016 Jae Choi
   … (lisans metni değişmedi) …
*/

#include "main.h"

/*--------------------------------------------------------------
 *  Local buffers / working arrays
 *-------------------------------------------------------------*/
float32_t output_IIR[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW];
float32_t output_IIR_IMPEDANCE[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW];

float32_t sum_Impedance[ADS1299_CHANNELS];
float32_t rms_Impedance[ADS1299_CHANNELS];
int32_t   rms_Impedance_Integer[ADS1299_CHANNELS];

float32_t data_Signal_Float[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW];
int32_t   data_Signal_Integer[ADS1299_CHANNELS][ADS1299_SIGNAL_WINDOW];

/* counterData, BIOEXG_SETTINGS, filtre yapıları, emg_* dizileri
 * ve yardımcı makro/fonksiyonlar (CTX, PDEC, SETTINGS_BIT_*…)
 * main.c / main.h içinde tanımlı varsayılmıştır.
 */
extern volatile uint32_t counterData;

/*==============================================================
 *                       EXTI6  -  DRDY
 *=============================================================*/
void EXTI6_IRQHandler(void)
{
    int32_t  REALS[ADS1299_CHANNELS];
    uint32_t RDATA_STATUS;

    ads1299_read_data(&RDATA_STATUS, REALS);

    for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

        if (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(channel_idx)) {

            /* ham tamsayı örneği */
            data_Signal_Integer[channel_idx][counterData] = REALS[channel_idx];

            /* ölçeklenmiş float örneği – EMG için */
            data_Signal_Float[channel_idx][counterData] =
                ((float32_t)REALS[channel_idx]) * 8.9407e-5f;   /* 24 → 6 x gain düzeltmesi */
        }
    }

    counterData++;
    if (counterData == ADS1299_SIGNAL_WINDOW) {
        counterData = 0;
        NVIC_SetPendingIRQ(EXTI0_IRQn);
    }

    /* Bayrak temizleme */
    EXTI->PR1 |= EXTI_PR1_PR6;
    NVIC_ClearPendingIRQ(EXTI6_IRQn);
}

/*==============================================================
 *                       EXTI0  -  İşlem Bloğu
 *=============================================================*/
void EXTI0_IRQHandler(void)
{
    /* IRQ var-yok LED göstergesi (turuncu) */
    GPIOE->ODR ^= GPIO_PIN_10;

    /* ---------- 1) Ham veriyi (integer) yayınla ---------- */
    for (uint32_t window_idx  = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx) {

        CTX('D');          /* “Raw Data” etiketi */

        for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

            if (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(channel_idx)) {
                check_RN42_RTS();

                int32_t val = data_Signal_Integer[channel_idx][window_idx];
                if (val < 0) { CTX('-'); val = -val; }
                PDEC((uint32_t)val);
                CTX(' ');
            }
        }
        CTX('\n');
    }

    /* ---------- 2) İmpedans işlemi ---------- */
    for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

        if (BIOEXG_SETTINGS & SETTINGS_BIT_IMP(channel_idx)) {

            /* HP → BP süzgeçleri */
            arm_biquad_cascade_df2T_f32(&biquad_HP_Struct[channel_idx],
                                        data_Signal_Float[channel_idx],
                                        output_IIR_IMPEDANCE[channel_idx],
                                        ADS1299_SIGNAL_WINDOW);

            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx)
                output_IIR_IMPEDANCE[channel_idx][window_idx] *= biquad_HP_Output_Gain;

            arm_biquad_cascade_df2T_f32(&biquad_BP_Struct[channel_idx],
                                        output_IIR_IMPEDANCE[channel_idx],
                                        output_IIR[channel_idx],
                                        ADS1299_SIGNAL_WINDOW);

            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx)
                output_IIR[channel_idx][window_idx] *= biquad_BP_Output_Gain;

            /* RMS hesapla */
            float32_t acc = 0.0f;
            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx) {
                float32_t s = output_IIR[channel_idx][window_idx];
                acc += s * s;
            }
            acc /= ADS1299_SIGNAL_WINDOW;
            arm_sqrt_f32(acc, &rms_Impedance[channel_idx]);
            rms_Impedance_Integer[channel_idx] = (int32_t)(rms_Impedance[channel_idx] * 10000.0f);
        }
    }

    /* ---------- 3) EMG işleme (HP+BP → rektifikasyon → RMS → Envelope) ---------- */
    for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

        if (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(channel_idx)) {

            /* Filtreler */
            arm_biquad_cascade_df2T_f32(&biquad_HP_Struct[channel_idx],
                                        data_Signal_Float[channel_idx],
                                        output_IIR_IMPEDANCE[channel_idx],
                                        ADS1299_SIGNAL_WINDOW);

            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx)
                output_IIR_IMPEDANCE[channel_idx][window_idx] *= biquad_HP_Output_Gain;

            arm_biquad_cascade_df2T_f32(&biquad_BP_Struct[channel_idx],
                                        output_IIR_IMPEDANCE[channel_idx],
                                        output_IIR[channel_idx],
                                        ADS1299_SIGNAL_WINDOW);

            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx)
                output_IIR[channel_idx][window_idx] *= biquad_BP_Output_Gain;

            /* Rektifikasyon */
            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx) {
                float32_t s = output_IIR[channel_idx][window_idx];
                emg_rectified[channel_idx][window_idx] = (s < 0.0f) ? -s : s;
            }

            /* RMS */
            float32_t sum_sq = 0.0f;
            for (uint32_t window_idx = 0; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx) {
                float32_t r = emg_rectified[channel_idx][window_idx];
                sum_sq += r * r;
            }
            sum_sq /= ADS1299_SIGNAL_WINDOW;
            arm_sqrt_f32(sum_sq, &emg_rms[channel_idx]);

            /* Envelope (son EMG_ENVELOPE_WINDOW örneğin ortalaması) */
            float32_t env_sum = 0.0f;
            uint32_t  start   = (ADS1299_SIGNAL_WINDOW > EMG_ENVELOPE_WINDOW)
                                ? ADS1299_SIGNAL_WINDOW - EMG_ENVELOPE_WINDOW : 0;

            for (uint32_t window_idx = start; window_idx < ADS1299_SIGNAL_WINDOW; ++window_idx)
                env_sum += emg_rectified[channel_idx][window_idx];

            emg_envelope[channel_idx] = env_sum / EMG_ENVELOPE_WINDOW;
        }
    }

    /* ---------- 4) Sonuçları ilet ---------- */
    /* İmpedans */
    CTX('I');
    for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

        if (BIOEXG_SETTINGS & SETTINGS_BIT_IMP(channel_idx)) {

            int32_t val = rms_Impedance_Integer[channel_idx];
            if (val < 0) { CTX('-'); val = -val; }
            PDEC((uint32_t)val);
            CTX(' ');
        }
    }
    CTX('\n');

    /* EMG RMS */
    CTX('R');
    for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

        if (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(channel_idx)) {
            uint32_t rms_i = (uint32_t)(emg_rms[channel_idx] * 10000.0f);
            PDEC(rms_i);
            CTX(' ');
        }
    }
    CTX('\n');

    /* EMG Envelope */
    CTX('E');
    for (uint32_t channel_idx = 0; channel_idx < ADS1299_CHANNELS; ++channel_idx) {

        if (BIOEXG_SETTINGS & SETTINGS_BIT_CHANNEL(channel_idx)) {
            uint32_t env_i = (uint32_t)(emg_envelope[channel_idx] * 10000.0f);
            PDEC(env_i);
            CTX(' ');
        }
    }
    CTX('\n');

    /* Buton durumu & son paket */
    uint32_t status_button = !((GPIOD->IDR & GPIO_IDR_ID9) >> 9);
    if (status_button) GPIOE->BSRR = GPIO_PIN_7;
    else               GPIOE->BSRR = (uint32_t)GPIO_PIN_7 << 16U;

    CTX('B'); PDEC(status_button); CTX('\n');
    STX("; We're good:\n");

    /* Bayrak temizleme */
    EXTI->PR1 |= EXTI_PR1_PR0;
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

/*==============================================================
 *                       USART3
 *=============================================================*/
void USART3_IRQHandler(void)
{
    char c = CRX();

    if (c == 'S') {

        /* Ayar moduna gir */
        GPIOE->BSRR = GPIO_PIN_10;         /* Amber LED ON */
        GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16U; /* STOP */

        setting_mode();

        GPIOE->BSRR = (uint32_t)GPIO_PIN_10 << 16U; /* Amber LED OFF */
        GPIOB->BSRR = GPIO_PIN_6;                    /* START */

        counterData = 0;
        NVIC_ClearPendingIRQ(EXTI0_IRQn);
        NVIC_SetPendingIRQ(EXTI0_IRQn);

    } else if (c == 'O') {
        GPIOD->ODR ^= GPIO_PIN_10;
        /* (SOS blink örneği burada yorumlu bırakıldı) */
    }

    NVIC_ClearPendingIRQ(USART3_IRQn);
}
