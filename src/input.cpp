#include <main.h>
unsigned long timerInput01 = millis();
unsigned long timerInput02 = millis();
unsigned long timerInput03 = millis();
unsigned long timerInput04 = millis();
unsigned long timerInput05 = millis();

void IRAM_ATTR isr_1() {
    Serial.println("bt_01");

    const int indexMod = 0;
    if (millis() - timerInput01 > 200 && millis() != timerInput01 && !digitalRead(14)) {
        timerInput01 = millis();
        if (queueCamMod != nullptr)
            xQueueSend(queueCamMod, &indexMod, 0);
    }
}

void IRAM_ATTR isr_2() {
    Serial.println("bt_02");
    const int bt_02_cmd = 1;
    if (millis() - timerInput02 > 200 && millis() != timerInput02 && !digitalRead(27)) {
        timerInput02 = millis();
        if (queueCamMod != nullptr)
            xQueueSend(queueCamMod, &bt_02_cmd, 0);
    }
}

void IRAM_ATTR isr_3() {
    Serial.println("bt_03");
    const int bt_03_cmd = 2;
    if (millis() - timerInput03 > 200 && millis() != timerInput03 && !digitalRead(26)) {
        timerInput03 = millis();
        if (queueCamMod != nullptr)
            xQueueSend(queueCamMod, &bt_03_cmd, portMAX_DELAY);
    }
}

void IRAM_ATTR isr_4() {
    Serial.println("bt_04");
    const int bt_04_cmd = 3;
    if (millis() - timerInput04 > 200 && millis() != timerInput04 && !digitalRead(25)) {
        timerInput04 = millis();
        if (queueCamMod != nullptr)
            xQueueSend(queueCamMod, &bt_04_cmd, portMAX_DELAY);
    }
}

void IRAM_ATTR isr_5() {
    Serial.println("bt_05");
    const int bt_05_cmd = 4;
    if (millis() - timerInput05 > 1000 && millis() != timerInput05 && !digitalRead(33)) {
        timerInput05 = millis();
        if (queueCamMod != nullptr)
            xQueueSend(queueCamMod, &bt_05_cmd, portMAX_DELAY);
    }
}
