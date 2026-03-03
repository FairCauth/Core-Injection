#include "native_bridge.h"
#include "details/natives.h"
#include "..\common\utils.h"
#include "..\common\jvm\jvm_context.h"
static JNINativeMethod g_methods[] = {
    {
        "startup",
        "()Z",
        (void*)native_functions::startup
    },
    {
        "redefineClasses",
        "(Ljava/lang/Class;[B)I",
        (void*)native_functions::redefineClasses
    },
    {
        "getClassBytes",
        "(Ljava/lang/Class;)[B",
        (void*)native_functions::getClassBytes
    }
};

void native_bridge::register_native(JNIEnv* env, std::string target) {
    //保存jvm状态 在idea里启动的时候
    JavaVM* jvm = nullptr;
    env->GetJavaVM(&jvm);
    jvm_context::set_jvm(jvm);
    //"Lcom/test/mod/natives/CoreNative;"
    jclass cls = utils::jni_func::find_class_global(target.c_str());
    if (!cls) {
        MessageBox(NULL, L"Failed to find NATIVECLASS", L"A", MB_OK | MB_ICONWARNING);
        return;
    }
   // MessageBox(NULL, L"find NATIVECLASS!!!!!!!!", L"A", MB_OK | MB_ICONWARNING);

    if (env->RegisterNatives(
        cls,
        g_methods,
        sizeof(g_methods) / sizeof(g_methods[0])) != JNI_OK) {

        MessageBox(NULL, L"Failed to RegisterNatives", L"A", MB_OK | MB_ICONWARNING);
        return;
    }

}