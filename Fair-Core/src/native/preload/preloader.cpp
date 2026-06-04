#include "preloader.h"
#include "..\..\common\utils.h"
#include "preloader_class.h"
#include "..\native_bridge.h"
//#include "classes.h"
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
	utils::writeLog("Init!");//([[BLjava/lang/ClassLoader;)V
	jmethodID run = env->GetStaticMethodID(loaderClass, "init", "()V");
	//classesData, jvm_context::get_classloader()
	env->CallStaticVoidMethod(loaderClass, run);
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
JNIEXPORT jobject JNICALL preloader_native::Java_com_fair_preload_Preloader_getClassLoader(
	JNIEnv* env,
	jclass  clazz
) {
	return jvm_context::get_classloader();
}

#include "..\..\common\classpack\class_pack.h"
JNIEXPORT jobjectArray JNICALL preloader_native::Java_com_fair_preload_Preloader_getClassByte(
	JNIEnv* env,
	jclass  loaderClass, jint mc_ver
) {
	//jobjectArray classesData = (jobjectArray)env->CallStaticObjectMethod(loaderClass, env->GetStaticMethodID(loaderClass, "getByteArray", "(I)[[B"), (jint)classCount);
	//int cptr = 0;
	//for (jsize j = 0; j < classCount; j++)
	//{
	//	jbyteArray classByteArray = env->NewByteArray(classSizes[j]);
	//	env->SetByteArrayRegion(classByteArray, 0, classSizes[j], (jbyte*)(classes + cptr));
	//	cptr += classSizes[j];
	//	env->SetObjectArrayElement(classesData, j, classByteArray);
	//}
	//utils::writeLog("[LOAD] " + minecraft_env::mc_ver + minecraft_env::get_mc_ver());
	std::string v;

	switch (mc_ver) {
	case 0:
		v = "noobf";
		break;

	case 1:
		v = "vanilla";
		break;

	case 2:
		v = "forge";
		break;

	case 3:
		v = "fabric";
		break;

	default:
		v = "unknown";
		break;
	}
	ClassPack pack = loadClassPack(utils::others::get_self_path() + "\\" + v +".pack");

	jobjectArray classesData = (jobjectArray)env->CallStaticObjectMethod(
		loaderClass,
		env->GetStaticMethodID(loaderClass, "getByteArray", "(I)[[B"),
		(jint)pack.classes.size()
	);

	for (jsize j = 0; j < (jsize)pack.classes.size(); j++) {
		const auto& bytes = pack.classes[j];

		jbyteArray classByteArray = env->NewByteArray((jsize)bytes.size());

		env->SetByteArrayRegion(
			classByteArray,
			0,
			(jsize)bytes.size(),
			reinterpret_cast<const jbyte*>(bytes.data())
		);

		env->SetObjectArrayElement(classesData, j, classByteArray);

		env->DeleteLocalRef(classByteArray);
	}

	return classesData;
}
