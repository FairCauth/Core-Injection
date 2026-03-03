#pragma once
#include <jni/jvmti.h>
#include <jni/jni.h>
#include <Windows.h>

#include "..\..\common\jvm\jvm_context.h"
static jclass targetClazz = nullptr;
static unsigned char* class_bytes;
static int class_bytes_len;

void* allocate(jlong size) {
	void* resultBuffer = nullptr;
	jvm_context::get_jvmTi()->Allocate(size, (unsigned char**)&resultBuffer);
	return resultBuffer;
}
	
void JNICALL classFileLoadHook(jvmtiEnv* jvmti_env, JNIEnv* env,
	jclass class_being_redefined, jobject loader,
	const char* name, jobject protection_domain,
	jint class_data_len, const unsigned char* class_data,
	jint* new_class_data_len, unsigned char** new_class_data) {

	if (targetClazz && class_being_redefined && env->IsSameObject(class_being_redefined, targetClazz))
	{
		class_bytes = (unsigned char*)class_data;
		class_bytes_len = class_data_len;
	}
}