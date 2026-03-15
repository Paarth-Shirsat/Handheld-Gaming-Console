#include "input.h"
#include "pin_defs.h"
#include "mpu6050.h"
#include "main.h"

extern ADC_HandleTypeDef JOYSTICK_ADC;

static ControlMode currentControlMode = CONTROL_JOYSTICK;
static bool lastJoystickButtonState = false;
static uint32_t lastDebounceTime = 0;
static const uint32_t debounceDelay = 200;

static int mpu_x, mpu_y, mpu_z;

static uint16_t readADC(uint32_t channel);
static bool isButtonPressed(GPIO_TypeDef *port, uint16_t pin);

void Input_Init(void) {
    currentControlMode = CONTROL_JOYSTICK;
    HAL_GPIO_WritePin(MPU_LED_PORT, MPU_LED_PIN, GPIO_PIN_RESET);
}

void Input_Update(void) {
    bool currentJoystickButtonState = isButtonPressed(JOYSTICK_BTN_PORT, JOYSTICK_BTN_PIN);

    if (currentJoystickButtonState && !lastJoystickButtonState) {
        if ((HAL_GetTick() - lastDebounceTime) > debounceDelay) {
            if (currentControlMode == CONTROL_JOYSTICK) {
                currentControlMode = CONTROL_MPU;
                HAL_GPIO_WritePin(MPU_LED_PORT, MPU_LED_PIN, GPIO_PIN_SET);
            } else if (currentControlMode == CONTROL_MPU) {
                currentControlMode = CONTROL_JOYSTICK;
                HAL_GPIO_WritePin(MPU_LED_PORT, MPU_LED_PIN, GPIO_PIN_RESET);
            }
            lastDebounceTime = HAL_GetTick();
        }
    }

    lastJoystickButtonState = currentJoystickButtonState;
}

InputDirection Input_GetDirection(void) {
    if (isButtonPressed(BTN_UP_PORT, BTN_UP_PIN)) return INPUT_UP;
    if (isButtonPressed(BTN_DOWN_PORT, BTN_DOWN_PIN)) return INPUT_DOWN;
    if (isButtonPressed(BTN_LEFT_PORT, BTN_LEFT_PIN)) return INPUT_LEFT;
    if (isButtonPressed(BTN_RIGHT_PORT, BTN_RIGHT_PIN)) return INPUT_RIGHT;

    if (currentControlMode == CONTROL_JOYSTICK) {
        uint16_t xVal = readADC(JOYSTICK_X_CHANNEL);
        uint16_t yVal = readADC(JOYSTICK_Y_CHANNEL);

        if (xVal < 4095 - JOYSTICK_THRESHOLD) return INPUT_UP;
        if (xVal > JOYSTICK_THRESHOLD) return INPUT_DOWN;
        if (yVal < 4095 - JOYSTICK_THRESHOLD) return INPUT_RIGHT;
        if (yVal > JOYSTICK_THRESHOLD) return INPUT_LEFT;

    } else if (currentControlMode == CONTROL_MPU) {
        MPU6050_Read_Accel(&mpu_x, &mpu_y, &mpu_z);

        if (mpu_x < MPU_ACCEL_X_DOWN_THRESHOLD) return INPUT_UP;
        if (mpu_x > MPU_ACCEL_X_UP_THRESHOLD) return INPUT_DOWN;
        if (mpu_y < MPU_ACCEL_Y_LEFT_THRESHOLD) return INPUT_RIGHT;
        if (mpu_y > MPU_ACCEL_Y_RIGHT_THRESHOLD) return INPUT_LEFT;
    }

    return INPUT_NONE;
}

bool Input_IsEnterPressed(void) {
    return isButtonPressed(BTN_ENTER_PORT, BTN_ENTER_PIN);
}

bool Input_IsEscPressed(void) {
    return isButtonPressed(BTN_ESC_PORT, BTN_ESC_PIN);
}

bool Input_IsJoystickButtonPressed(void) {
    return isButtonPressed(JOYSTICK_BTN_PORT, JOYSTICK_BTN_PIN);
}

ControlMode Input_GetControlMode(void) {
    return currentControlMode;
}

static uint16_t readADC(uint32_t channel) {
    ADC_ChannelConfTypeDef adc_config = {0};
    adc_config.Channel = channel;
    adc_config.Rank = 1;
    adc_config.SamplingTime = ADC_SAMPLETIME_56CYCLES;
    HAL_ADC_ConfigChannel(&JOYSTICK_ADC, &adc_config);
    HAL_ADC_Start(&JOYSTICK_ADC);
    HAL_ADC_PollForConversion(&JOYSTICK_ADC, HAL_MAX_DELAY);
    uint16_t val = HAL_ADC_GetValue(&JOYSTICK_ADC);
    HAL_ADC_Stop(&JOYSTICK_ADC);
    return val;
}

static bool isButtonPressed(GPIO_TypeDef *port, uint16_t pin) {
    return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET);
}
