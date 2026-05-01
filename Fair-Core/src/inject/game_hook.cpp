#include "game_hook.h"
#include <hook/MinHook.h>
#include <jni/jni.h>
#include "..\native\native_bridge.h"
#include "..\common\jvm\jvm_context.h"
#include "..\native\preload\preloader.h"
#include "..\common\utils.h"
typedef void(*Java_org_lwjgl_system_JNI_callP__J)(JNIEnv* env, jclass clazz, jlong lVar);
Java_org_lwjgl_system_JNI_callP__J original_nglFlush = NULL;
static LPVOID target = nullptr;
/// <summary>
/// 程序主入口！
/// </summary>
/// <param name="env"></param>
void Entry(JNIEnv* env) {
    //保存jvm状态 注入的时候
    JavaVM* jvm = nullptr;
    env->GetJavaVM(&jvm);
    jvm_context::set_jvm(jvm);
    //patch_jvm();
    //保存classloader
    jobject classloader = utils::jni_func::get_classloader_from_name(env, "Render thread");
    jvm_context::set_classloader(classloader);

    //加载preloader （注入Class）
   	utils::writeLog("Hook down"); 
    preloader::define_loader(env);

}


void nglFlush_Hook(JNIEnv* env, jclass clazz, jlong lVar) {
    original_nglFlush(env, clazz, lVar);
    MH_DisableHook(target);

    Entry(env);
    return;
}
bool game_hook::install_hook() {
    if (MH_Initialize() != MH_OK)
        return false;

    if (MH_CreateHookApiEx(L"lwjgl.dll",
            "Java_org_lwjgl_system_JNI_callP__J", 
            &nglFlush_Hook, 
            reinterpret_cast<LPVOID*>(&original_nglFlush), 
            &target) != MH_OK) {

        MessageBox(NULL, L"Failed to create hook for lwjgl.dll!", L"A", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (MH_EnableHook(target) != MH_OK) {
        MessageBox(NULL, L"Failed to enable hook!", L"A", MB_OK);
        return false;
    }
}


