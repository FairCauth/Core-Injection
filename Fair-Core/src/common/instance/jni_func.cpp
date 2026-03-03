#include "..\utils.h"
#include "..\jvm\jvm_context.h"
namespace utils::jni_func {
	jobject get_classloader(JNIEnv* env) {
		jclass threadClass = env->FindClass("java/lang/Thread");
		jmethodID getAllStackTracesMethodID = env->GetStaticMethodID(threadClass, "getAllStackTraces", "()Ljava/util/Map;");
		jobject stackTraceMap = env->CallStaticObjectMethod(threadClass, getAllStackTracesMethodID);

		jclass mapClass = env->FindClass("java/util/Map");
		jmethodID keySetMethodID = env->GetMethodID(mapClass, "keySet", "()Ljava/util/Set;");
		jobject keySet = env->CallObjectMethod(stackTraceMap, keySetMethodID);
		jclass setClass = env->FindClass("java/util/Set");
		jmethodID iteratorMethodID = env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
		jobject iterator = env->CallObjectMethod(keySet, iteratorMethodID);
		jclass iteratorClass = env->FindClass("java/util/Iterator");
		jmethodID hasNextMethodID = env->GetMethodID(iteratorClass, "hasNext", "()Z");
		jmethodID nextMethodID = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
		jboolean hasNext;
		jobject thread = NULL;
		jobject contextClassLoader = NULL;
		jobject finalCl = NULL;
		while ((hasNext = env->CallBooleanMethod(iterator, hasNextMethodID)) == JNI_TRUE) {
			thread = env->CallObjectMethod(iterator, nextMethodID);
			jclass threadClass = env->GetObjectClass(thread);
			jmethodID getNameMethodID = env->GetMethodID(threadClass, "getName", "()Ljava/lang/String;");//thread.getName()

			jstring name = (jstring)env->CallObjectMethod(thread, getNameMethodID);
			const char* nameStr = env->GetStringUTFChars(name, NULL);
			//loader name
			jmethodID getContextClassLoaderMethodID = env->GetMethodID(threadClass, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
			contextClassLoader = env->CallObjectMethod(thread, getContextClassLoaderMethodID);
			if (contextClassLoader == nullptr) continue;
			jclass contextClassLoaderClass = env->GetObjectClass(contextClassLoader);
			jmethodID getClass = env->GetMethodID(contextClassLoaderClass, "getClass", "()Ljava/lang/Class;");//thread.getContextClassLoader().getClass()
			jobject s = env->CallObjectMethod(contextClassLoader, getClass);
			if (s == nullptr) continue;
			jclass sd = env->GetObjectClass(s);
			jmethodID as = env->GetMethodID(sd, "getName", "()Ljava/lang/String;");//thread.getContextClassLoader().getClass().getName()
			jstring n = (jstring)env->CallObjectMethod(s, as);
			if (n == nullptr) continue;
			const char* nameStrs = env->GetStringUTFChars(n, NULL);
			std::cout << nameStr << " " << nameStrs << std::endl;
			std::string a = std::string(nameStr);
			std::string b = std::string(nameStrs);
			std::size_t found = b.rfind("TransformingClassLoader");

			if (found != std::string::npos) {
				finalCl = contextClassLoader;
			}
			//if (strcmp(nameStr, str) == 0) {
			//	 getContextClassLoaderMethodID = env->GetMethodID(threadClass, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
			//	contextClassLoader = env->CallObjectMethod(thread, getContextClassLoaderMethodID);
			//}
			env->ReleaseStringUTFChars(name, nameStr);
			env->DeleteLocalRef(threadClass);
			env->DeleteLocalRef(name);
			env->DeleteLocalRef(contextClassLoaderClass);
			env->DeleteLocalRef(n);
			env->DeleteLocalRef(sd);
			if (finalCl != NULL)
				break;
		}

		env->DeleteLocalRef(iteratorClass);
		env->DeleteLocalRef(setClass);
		env->DeleteLocalRef(mapClass);
		env->DeleteLocalRef(thread);
		env->DeleteLocalRef(keySet);
		env->DeleteLocalRef(stackTraceMap);

		return finalCl;
	}

	jobject get_classloader_from_name(JNIEnv* env, const char* str) {
		jclass threadClass = env->FindClass("java/lang/Thread");
		jmethodID getAllStackTracesMethodID = env->GetStaticMethodID(threadClass, "getAllStackTraces", "()Ljava/util/Map;");
		jobject stackTraceMap = env->CallStaticObjectMethod(threadClass, getAllStackTracesMethodID);

		jclass mapClass = env->FindClass("java/util/Map");
		jmethodID keySetMethodID = env->GetMethodID(mapClass, "keySet", "()Ljava/util/Set;");
		jobject keySet = env->CallObjectMethod(stackTraceMap, keySetMethodID);
		jclass setClass = env->FindClass("java/util/Set");
		jmethodID iteratorMethodID = env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
		jobject iterator = env->CallObjectMethod(keySet, iteratorMethodID);
		jclass iteratorClass = env->FindClass("java/util/Iterator");
		jmethodID hasNextMethodID = env->GetMethodID(iteratorClass, "hasNext", "()Z");
		jmethodID nextMethodID = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
		jboolean hasNext;
		jobject thread = NULL;
		jobject contextClassLoader = NULL;
		while ((hasNext = env->CallBooleanMethod(iterator, hasNextMethodID)) == JNI_TRUE) {
			thread = env->CallObjectMethod(iterator, nextMethodID);
			jclass threadClass = env->GetObjectClass(thread);
			jmethodID getNameMethodID = env->GetMethodID(threadClass, "getName", "()Ljava/lang/String;");
			jstring name = (jstring)env->CallObjectMethod(thread, getNameMethodID);
			const char* nameStr = env->GetStringUTFChars(name, NULL);
			if (strcmp(nameStr, str) == 0) {
				jmethodID getContextClassLoaderMethodID = env->GetMethodID(threadClass, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
				contextClassLoader = env->CallObjectMethod(thread, getContextClassLoaderMethodID);
			}
			env->ReleaseStringUTFChars(name, nameStr);
			env->DeleteLocalRef(threadClass);
			env->DeleteLocalRef(name);
			if (contextClassLoader != NULL)
				break;
		}
		env->DeleteLocalRef(iteratorClass);
		env->DeleteLocalRef(setClass);
		env->DeleteLocalRef(mapClass);
		env->DeleteLocalRef(thread);
		env->DeleteLocalRef(keySet);
		env->DeleteLocalRef(stackTraceMap);

		return contextClassLoader;
	}

	jclass define_class(JNIEnv* env, jbyteArray classByte, jobject classLoader) {

		jclass clClass = env->FindClass("java/lang/ClassLoader");
		jmethodID defineClass = env->GetMethodID(clClass, "defineClass", "([BII)Ljava/lang/Class;");
		jclass clazz = (jclass)env->CallObjectMethod(classLoader, defineClass, classByte, 0, env->GetArrayLength(classByte));
		jthrowable throwable = env->ExceptionOccurred();
		if (throwable) {
			env->ExceptionClear();
			return NULL;
		}
		return clazz;
	}
	jclass define_class0(JNIEnv* env, const jbyte* classByte, jobject classLoader, jsize len) {
		return env->DefineClass(NULL, classLoader, classByte, len);
	}
	jclass find_class_global(const char* signature) {
		if (jvm_context::is_null()) {
			MessageBox(NULL, L"is_null", L"A", MB_OK | MB_ICONWARNING);
			return nullptr;
		}
		jclass* loadedClasses;
		jint loadedClassesCount = 0;
		//"Lcom/leave/ink/natives/AgentNative;"
		jvm_context::get_jvmTi()->GetLoadedClasses(&loadedClassesCount, &loadedClasses);
		for (jint i = 0; i < loadedClassesCount; i++)
		{
			char* sig;
			jvm_context::get_jvmTi()->GetClassSignature(loadedClasses[i], &sig, NULL);
			std::cout << sig << std::endl;

			if (!strcmp(sig, signature))
			{
				return loadedClasses[i];
			}
		}
		return nullptr;
	}
}
