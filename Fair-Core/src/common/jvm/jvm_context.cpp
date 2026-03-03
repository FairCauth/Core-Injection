#include "jvm_context.h"
namespace jvm_context {
	static JavaVM* g_vm = nullptr;
	static jvmtiEnv* g_jvmti = nullptr;

	static jobject g_classloader = nullptr;

	void set_jvm(JavaVM* jvm) {
		if (g_vm == nullptr) {
			g_vm = jvm;
			jvm->GetEnv((void**)&g_jvmti, JVMTI_VERSION_1_2);
		}

	}
	void set_classloader(jobject cl) {
		g_classloader = cl;
	}
	jobject get_classloader() {
		return g_classloader;
	}
	bool is_null() {
		return g_vm == nullptr || g_jvmti == nullptr;
	}
	JavaVM* get_jvm() {
		return g_vm;
	}
	jvmtiEnv* get_jvmTi() {
		return g_jvmti;
	}

}