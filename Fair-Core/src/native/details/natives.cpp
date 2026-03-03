#include "natives.h"
#include <Windows.h>
#include <jni/jni.h>
#include <jni/jvmTi.h>
#include "..\..\common\jvm\jvm_context.h"

void JNICALL native_functions::test(JNIEnv* env, jclass clazz) {
	MessageBox(NULL, L"OKOKOK", L"A", MB_OK | MB_ICONWARNING);
}