#include "preloader.h"
#include "..\..\common\utils.h"
#include "preloader_class.h"
#include "..\native_bridge.h"
#include "classes.h"
#include "..\..\common\jvm\jvm_context.h"
void preloader::define_loader(JNIEnv* env) {

	jbyteArray jarray = env->NewByteArray(preloader_classSizes);
	env->SetByteArrayRegion(jarray, 0, preloader_classSizes, (jbyte*)preloader_class);

	jclass loaderClass = utils::jni_func::define_class(env, jarray, jvm_context::get_classloader());
	if (!loaderClass)
	{
		std::cout << "Loader class already loaded." << std::endl;

		//idea启动或mod启动 不执行加载
		return;
	}
	//MessageBox(NULL, L"准备注入", L"A", MB_OK | MB_ICONWARNING);
	utils::writeLog("define preloader success!");
	//注入jar
	jobjectArray classesData = (jobjectArray)env->CallStaticObjectMethod(loaderClass, env->GetStaticMethodID(loaderClass, "getByteArray", "(I)[[B"), (jint)classCount);
	int cptr = 0;
	for (jsize j = 0; j < classCount; j++)
	{
		jbyteArray classByteArray = env->NewByteArray(classSizes[j]);
		env->SetByteArrayRegion(classByteArray, 0, classSizes[j], (jbyte*)(classes + cptr));
		cptr += classSizes[j];
		env->SetObjectArrayElement(classesData, j, classByteArray);
	}
	utils::writeLog("Run!");
	jmethodID run = env->GetStaticMethodID(loaderClass, "run", "([[BLjava/lang/ClassLoader;)V");
	env->CallStaticVoidMethod(loaderClass, run, classesData, jvm_context::get_classloader());
	utils::writeLog("Call!");
}
void preloader::call_java() {

}
JNIEXPORT void JNICALL preloader_native::Java_com_fair_preload_Preloader_registerNatives(JNIEnv* env,jclass clazz, jstring target) {
	utils::writeLog("Register natives!");
	native_bridge::register_native(env, utils::jni_convert::jstring2string(env, target));
}
JNIEXPORT jclass JNICALL preloader_native::Java_com_fair_preload_Preloader_defineClass(JNIEnv* env, jclass clazz, jbyteArray  clazzBytes) {
	return utils::jni_func::define_class(env, clazzBytes, jvm_context::get_classloader());
}
JNIEXPORT void JNICALL preloader_native::Java_com_fair_preload_Preloader_log(JNIEnv* env, jclass clazz, jstring string) {
	utils::writeLog(utils::jni_convert::jstring2string(env, string));
}
