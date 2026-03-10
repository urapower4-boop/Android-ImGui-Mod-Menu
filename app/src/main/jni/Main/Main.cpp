#include "../Include/KittyMemory/MemoryPatch.h"
#include "../Include/ImGui.h"
#include "../Include/RemapTools.h"
#include "../Include/Drawing.h"
#include "../Include/Unity.h"

// Змінна для кнопки
bool wallhack = false;

void DrawMenu() {
    // Назва твого меню
    ImGui::Begin("HACK FOR SO2");

    // Створюємо кнопку-перемикач
    if (ImGui::Checkbox("Wallhack (Chams)", &wallhack)) {
        if (wallhack) {
            // Твій оффсет для Standoff 2
            MemoryPatch::createWithHex("libil2cpp.so", 0x1079728, "C0 03 5F D6").Modify();
        } else {
            // Відкат до стандартних значень (оригінальний байт)
            MemoryPatch::createWithHex("libil2cpp.so", 0x1079728, "FD 7B BF A9").Modify();
        }
    }

    ImGui::End();
}

void *thread(void *) {
    LOGI(OBFUSCATE("Main Thread Loaded: %d"), gettid());
    
    // Чекаємо 10 секунд, щоб гра встигла завантажити бібліотеки
    sleep(10); 
    
    initModMenu((void *)DrawMenu);
    LOGI("Main thread done");
    pthread_exit(0);
}

extern "C" {
    JavaVM *jvm = nullptr;
    JNIEnv *env = nullptr;

    __attribute__((visibility ("default")))
    jint loadJNI(JavaVM *vm) {
        jvm = vm;
        vm->AttachCurrentThread(&env, nullptr);
        LOGI("loadJNI(): Initialized");
        return JNI_VERSION_1_6;
    }
}

__attribute__((constructor))
void init() {
    LOGI("Loaded Mod Menu");
    pthread_t t;
    pthread_create(&t, nullptr, thread, nullptr);
    RemapTools::RemapLibrary("libLoader.so");
}
