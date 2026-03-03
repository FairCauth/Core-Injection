#pragma once
#include <jni/jni.h>
#include <string>
namespace native_bridge {
	void register_native(JNIEnv* env, std::string target);

}