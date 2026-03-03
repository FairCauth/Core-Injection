#include "..\utils.h"
#include <locale>
#include <codecvt>
using namespace std;
namespace utils::convert {
	string wstring2string(const wstring& str, const string& locale)
	{
		typedef codecvt_byname<wchar_t, char, mbstate_t> F;
		static wstring_convert<F> strCnv(new F(locale));
		return strCnv.to_bytes(str);
	}
	string wstring2utf8string(const wstring& str)
	{
		static wstring_convert<codecvt_utf8<wchar_t> > strCnv;
		return strCnv.to_bytes(str);
	}

	wstring utf8string2wstring(const string& str)
	{
		static wstring_convert< codecvt_utf8<wchar_t> > strCnv;
		return strCnv.from_bytes(str);
	}

}
namespace utils::jni_convert {
	jstring string2jstring(JNIEnv* env, const char* pat) {
		jclass strClass = env->FindClass("Ljava/lang/String;");
		if (strClass == nullptr)
			return nullptr;
		jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
		if (ctorID == nullptr)
			return nullptr;
		jbyteArray bytes = env->NewByteArray(strlen(pat));
		if (bytes == nullptr)
			return nullptr;
		env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
		jstring encoding = env->NewStringUTF("GB2312");
		if (encoding == nullptr) {
			env->DeleteLocalRef(bytes);
			return nullptr;
		}
		jstring str = (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
		env->DeleteLocalRef(bytes);
		env->DeleteLocalRef(encoding);
		return str;
	}

	std::string jstring2string(JNIEnv* env, jstring str) {
		const char* jnamestr = env->GetStringUTFChars(str, NULL);
		std::string stdFileName(jnamestr);
		return stdFileName;
	}
}
