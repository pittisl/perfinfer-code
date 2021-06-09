#include <jni.h>
#include <pthread.h>
#include <android/log.h>
#define APPNAME "MyHackLib"

#include "hacklib.h"


std::string device_config_c;
static bool jni_keep_collect;
static std::vector<struct hacklib_datapoint> data;
static pthread_t t;
static JavaVM *mVM;
jobject g_obj;

void *hacklib_jni_thread_wrapper(void *ptr) {
    JNIEnv *env;
    mVM->AttachCurrentThread(&env, NULL);

    hacklib_data_keepcollect(data, &jni_keep_collect, true, env);
    mVM->DetachCurrentThread();
    return NULL;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_example_exampleapp_MyForegroundService_nativeStartCollect(JNIEnv *env, jobject thiz, jstring pathObj) {
    const char *device_config;
    device_config = env->GetStringUTFChars(pathObj, NULL);
    device_config_c = std::string(device_config);
    jni_keep_collect = true;
    g_obj = env->NewGlobalRef(thiz);
    pthread_create(&t, NULL, hacklib_jni_thread_wrapper, NULL);
    __android_log_print(ANDROID_LOG_ERROR, APPNAME, "MyHackLib: start recording...");
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_example_exampleapp_MyForegroundService_nativeStopCollect(JNIEnv *env, jobject thiz) {
    jni_keep_collect = false;
    pthread_join(t, NULL);
    __android_log_print(ANDROID_LOG_ERROR, APPNAME, "MyHackLib: stopped recording.");
}

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *aReserved) {
    // cache java vm
    mVM = vm;
    return JNI_VERSION_1_6;
}
