#pragma once
#include <jni/jvmti.h>
#include <jni/jni.h>
namespace jvm_context {
	void set_jvm(JavaVM* jvm);

	void set_classloader(jobject jvm);
	jobject get_classloader();

	bool is_null();
	JavaVM* get_jvm();
	jvmtiEnv* get_jvmTi();

}