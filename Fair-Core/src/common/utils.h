#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <jni/jni.h>
namespace utils {
	void writeLog(const std::string& message);
	namespace convert {
		std::string wstring2string(const std::wstring& str, const std::string& locale);
		std::string wstring2utf8string(const std::wstring& str);
		std::wstring utf8string2wstring(const std::string& str);

	}
	namespace jni_convert {
		jstring string2jstring(JNIEnv* env, const char* pat);
		std::string jstring2string(JNIEnv* env, jstring str);
	}
	namespace jni_func {
		jclass define_class(JNIEnv* env, jbyteArray class_byte, jobject classloader);
		jclass define_class0(JNIEnv* env, const jbyte* class_byte, jobject classloader, jsize len);
		jobject get_classloader(JNIEnv* env);
		jobject get_classloader_from_name(JNIEnv* env, const char* thread_name);
		jclass find_class_global(const char* signature);
	}
	namespace others {
		bool inject_dll(DWORD processID, const char* dllPath);
		DWORD get_process_id(const char* processName);
		std::string get_self_path();
		std::string get_self_name();
	}
}