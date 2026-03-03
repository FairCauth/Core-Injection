#include "natives.h"
#include <Windows.h>
#include <hook/MinHook.h>
#include "..\..\common\utils.h"
#include "..\..\common\jvm\jvm_context.h"
typedef BOOL(WINAPI* wglSwapBuffers_t)(HDC);
wglSwapBuffers_t originalSwapBuffers = nullptr;
static LPVOID target = nullptr;

static jclass native_class = nullptr;

JNIEnv* GetJNIEnv() {
    if (jvm_context::is_null()) return nullptr;

    JNIEnv* env = nullptr;

    jint result = jvm_context::get_jvm()->GetEnv((void**)&env, JNI_VERSION_1_8);

    if (result == JNI_EDETACHED) {

        if (jvm_context::get_jvm()->AttachCurrentThread((void**)&env, nullptr) != JNI_OK) {
            return nullptr;
        }
    }

    return env;
}

BOOL WINAPI HookedWglSwapBuffers(HDC hdc) {
    JNIEnv* env = GetJNIEnv();
    if (env) {
        //type = 1
        jmethodID id = env->GetStaticMethodID(native_class, "on_jni_call", "(I)V");

        env->CallStaticVoidMethod(native_class, id, 1);
    }
	return originalSwapBuffers(hdc);

}
void JNICALL native_functions::initSkia(JNIEnv* env, jclass clazz) {

    native_class = (jclass)env->NewGlobalRef(clazz);

    if (MH_CreateHookApiEx(L"opengl32.dll","wglSwapBuffers", &HookedWglSwapBuffers, reinterpret_cast<LPVOID*>(&originalSwapBuffers), &target) != MH_OK) {
        MessageBox(NULL, L"Failed to hook opengl32.dll", L"A", MB_OK);
        return;
    }
    if (MH_EnableHook(target) != MH_OK) {
        MessageBox(NULL, L"Failed to enable hook!", L"A", MB_OK);
        return;
    }
    utils::writeLog("opengl hooked ok");
}