#include "transformer_native.h"
#include "natives.h"

jboolean JNICALL native_functions::startup(JNIEnv* env, jclass clazz) {
    jvmtiCapabilities capabilities;
    memset(&capabilities, 0, sizeof(jvmtiCapabilities));
    capabilities.can_retransform_classes = true;
    capabilities.can_retransform_any_class = true;
    capabilities.can_redefine_any_class = true;
    capabilities.can_redefine_classes = true;
    capabilities.can_generate_all_class_hook_events = true;

    jvmtiError capError = jvm_context::get_jvmTi()->AddCapabilities(&capabilities);
    if (capError != JVMTI_ERROR_NONE)
        return false;

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.ClassFileLoadHook = &classFileLoadHook;
    jvm_context::get_jvmTi()->SetEventCallbacks(&callbacks, sizeof(callbacks));
    jvm_context::get_jvmTi()->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
    return true;
}

jint JNICALL native_functions::redefineClasses(JNIEnv* env, jclass cls, jclass targetClass, jbyteArray newClassBytes) {

    jsize length = env->GetArrayLength(newClassBytes);
    jbyte* bytes = env->GetByteArrayElements(newClassBytes, NULL);
    jvmtiClassDefinition classDef;
    memset(&classDef, 0, sizeof(jvmtiClassDefinition));
    classDef.klass = targetClass;
    classDef.class_byte_count = length;
    classDef.class_bytes = (unsigned char*)bytes;
    jvmtiError error = jvm_context::get_jvmTi()->RedefineClasses(1, &classDef);
    //sendMsg("Redefine class ");

    env->ReleaseByteArrayElements(newClassBytes, bytes, JNI_ABORT);
    return (jint)error;
}

jbyteArray JNICALL native_functions::getClassBytes(JNIEnv* env, jclass cls, jclass targetClass) {

    targetClazz = targetClass;
    class_bytes = nullptr;
    jclass* classes = (jclass*)allocate(sizeof(jclass));
    classes[0] = targetClass;
    jvm_context::get_jvmTi()->RetransformClasses(1, classes);
    while (!class_bytes)
    {
        continue;
    }
    jbyteArray outputArray = env->NewByteArray(class_bytes_len);
    env->SetByteArrayRegion(outputArray, 0, class_bytes_len, (jbyte*)class_bytes);
    jvm_context::get_jvmTi()->Deallocate((unsigned char*)classes);
    return outputArray;
}