#include <pthread.h>
#include <unistd.h>
#include "../Include/KittyMemory/MemoryPatch.h"
#include "../Include/ImGui.h"
#include "../Include/RemapTools.h"
#include "../Include/Logger.h" // Додали логгер

// Змінна для кнопки
bool wallhack = false;
MemoryPatch wallhackPatch;

void DrawMenu() {
    // Назва меню
    ImGui::Begin("HACK FOR SO2");

    // Створюємо кнопку-перемикач
    if (ImGui::Checkbox("Wallhack (Chams)", &wallhack)) {
        if (wallhack) {
            // Застосовуємо патч (оффсет 0x1079728 для Standoff 2)
            wallhackPatch = MemoryPatch::createWithHex("libil2cpp.so", 0x1079728, "C0 03 5F D6");
            if (wallhackPatch.Modify()) {
                LOGI("Wallhack Enabled!");
            }
        } else {
            // Відкат до оригіналу
            wallhackPatch.Restore();
            LOGI("Wallhack Disabled!");
        }
    }

    ImGui::End();
}

void *thread(void *) {
    LOGI("Main Thread Loaded");
    
    // Чекаємо, поки гра завантажить основну бібліотеку
    while (!MemoryPatch::getLibBaseAddr("libil2cpp.so")) {
        sleep(1);
    }
    
    // Даємо грі ще 2 секунди "продихнути"
    sleep(2); 
    
    // Запускаємо меню
    // Примітка: у деяких шаблонах функція називається InitModMenu або подібним чином
    // Переконайся, що назва збігається з твоїм шаблоном
    initModMenu((void *)DrawMenu);
    
    LOGI("Main thread done");
    return nullptr;
}

__attribute__((constructor))
void init() {
    pthread_t t;
    pthread_create(&t, nullptr, thread, nullptr);
}
