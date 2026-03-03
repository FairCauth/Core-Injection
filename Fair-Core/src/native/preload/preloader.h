#pragma once
#include "jni/jni.h"
namespace preloader {
	void define_loader(JNIEnv* env);
	void call_java();
}

/*
 * Native bindings for:
 * cn.xgb0.preload.Preloader
 *
 * All methods are static native
 */
extern "C" {
    namespace preloader_native {

        /**
         * public static native void log(String string);
         * JNI signature: (Ljava/lang/String;)V
         */
        JNIEXPORT void JNICALL Java_com_fair_preload_Preloader_log(
            JNIEnv* env,
            jclass  clazz,
            jstring string
        );

        /**
         * public static native Class<?> defineClass(byte[] clazz);
         * JNI signature: ([B)Ljava/lang/Class;
         */
        JNIEXPORT jclass JNICALL Java_com_fair_preload_Preloader_defineClass(
            JNIEnv* env,
            jclass      clazz,
            jbyteArray  clazzBytes
        );

        /**
         * public static native void registerNatives();
         * JNI signature: ()V
         */
        JNIEXPORT void JNICALL Java_com_fair_preload_Preloader_registerNatives(
            JNIEnv* env,
            jclass  clazz,
            jstring target
        );

    }
}
