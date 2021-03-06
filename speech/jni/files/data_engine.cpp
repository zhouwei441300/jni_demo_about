#include <jni.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include<map>

#include "util.h"

#include "com_sogou_speech_utils_DataEngine.h"

using namespace std;

struct Param {
    jobject data;
};

// 全局变量  处理JNIEnv jobject不能跨线程共享传递的问题
JavaVM *g_jvm = NULL;

// 如果每条线程使用的数据都很多 可以将数据打包转换为jobject 存放在全局的map中 在创建线程的时候
// 给每条线程一个标记 key, 线程拿着这个key在全局map里面去取值
// jobject g_obj = NULL;

map<string, Param> callbacks;

//当动态库被加载时这个函数被系统自动调用
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
     LOGI("JNI_OnLoad!");

     JNIEnv* env = NULL;
     jint result = -1;

     //获取JNI版本
     if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
     {
         LOGI("GetEnv failed!");
         return result;
     }

     return JNI_VERSION_1_4;
}

JNIEXPORT jint JNICALL Java_com_sogou_speech_utils_DataEngine_add__IF(JNIEnv *env, jclass jclazz, jint a, jfloat b){
    return (int)(a+b);
}


JNIEXPORT jint JNICALL Java_com_sogou_speech_utils_DataEngine_add___3I(JNIEnv * env, jclass jclazz, jintArray arr){
    int total = 0;
    jint* int_arr = env->GetIntArrayElements(arr,NULL);
    jsize len = env->GetArrayLength(arr);
    for(int i = 0;i<len;i++){
        total+=int_arr[i];
    }

    // env->ReleaseIntArrayElements(arr,int_arr,JNI_ABORT);

    return total;
}


JNIEXPORT jstring JNICALL Java_com_sogou_speech_utils_DataEngine_getToken(JNIEnv *env, jclass jclazz){
   char res[] = "hello world";
   return env->NewStringUTF(res);
}


JNIEXPORT jstring JNICALL Java_com_sogou_speech_utils_DataEngine_getPwd(JNIEnv * env, jclass jclazz, jstring jstr){
    const char* inputStr = env->GetStringUTFChars(jstr,NULL);
    char defaultName[] = "Tom";

    int res = strcasecmp(inputStr,defaultName);
    char *ret = NULL;
    if(res == 0){
        ret = "success";
    }else{
        ret = "fail";
    }

    env->ReleaseStringUTFChars(jstr,inputStr);  //释放局部引用

    return env->NewStringUTF(ret);
}


JNIEXPORT jint JNICALL Java_com_sogou_speech_utils_DataEngine_checkout(JNIEnv *env, jclass jclazz, jobject jobj){
    //获得jfieldID 以及 该字段的初始值
    jfieldID  nameFieldId ;

    jclass cls = env->GetObjectClass(jobj);  //获得Java层该对象实例的类引用，即HelloJNI类引用

    nameFieldId = env->GetFieldID(cls , "name" , "Ljava/lang/String;"); //获得属性句柄
    jstring javaNameStr = (jstring)env->GetObjectField(jobj ,nameFieldId);  // 获得该属性的值
    const char * c_javaName = env->GetStringUTFChars(javaNameStr , NULL);  //转换为 char *类型
    std::string str_name = c_javaName ;
    env->ReleaseStringUTFChars(javaNameStr , c_javaName);  //释放局部引用

    nameFieldId = env->GetFieldID(cls , "pwd" , "Ljava/lang/String;"); //获得属性句柄
    jstring javaPwdStr = (jstring)env->GetObjectField(jobj ,nameFieldId);  // 获得该属性的值
    const char * c_javaPwd = env->GetStringUTFChars(javaPwdStr , NULL);  //转换为 char *类型
    std::string str_pwd = c_javaPwd ;
    env->ReleaseStringUTFChars(javaPwdStr , c_javaPwd);  //释放局部引用

    int ret  = 0;
    if(str_name.compare("123") == 0 && str_pwd.compare("456") == 0){
        ret = 0;
    }else{
        ret = -1;
    }

    LOGI("%s",c_javaPwd);

    return ret;
}

JNIEXPORT void JNICALL Java_com_sogou_speech_utils_DataEngine_init(JNIEnv *env, jobject jobj, jintArray jarr){

     //方法一
     /*
       jint temp1 = 45,temp2 = 20;
       (*env)->SetIntArrayRegion(env,javaArray,1,1,&temp1);
       (*env)->SetIntArrayRegion(env,javaArray,2,1,&temp2);
              */

      //方法二：
      jint * temp = (jint*) env->GetIntArrayElements(jarr, 0);
      jsize len = env->GetArrayLength(jarr);
      for(int i = 0;i<len;i++){
          temp[i] = i;
      }
      env->SetIntArrayRegion(jarr, 0,len,temp);
}

JNIEXPORT jboolean JNICALL Java_com_sogou_speech_utils_DataEngine_verify
  (JNIEnv *env, jobject jobj, jshortArray jarr){
    jshort* short_arr = env->GetShortArrayElements(jarr,NULL);
    jsize len = env->GetArrayLength(jarr);
    int total = 0;
    for(int i = 0;i<len;i++){
        total+=short_arr[i];
    }

    jboolean ret = false;
    if(total == 100){
        ret = true;
    }

    return ret;

}

JNIEXPORT jshortArray JNICALL Java_com_sogou_speech_utils_DataEngine_vad(JNIEnv *env, jobject jobj, jshortArray jarr){
    jshort* short_arr = env->GetShortArrayElements(jarr,NULL);
    jsize len = env->GetArrayLength(jarr);

    jshortArray ret = env->NewShortArray(len);
    jshort buf[1] = {0};

    for(int i = 0;i<len;i++){
        buf[0] = short_arr[i]+100;
        env->SetShortArrayRegion(ret,i,1,buf);
    }

    return ret;
}

JNIEXPORT jobject JNICALL Java_com_sogou_speech_utils_DataEngine_getAccount(JNIEnv *env, jobject jobj){
    jclass clazz = env->FindClass("com/sogou/speech/domain/Account");
    jmethodID account_init = env->GetMethodID(clazz,"<init>","()V");
    jobject account = env->NewObject(clazz, account_init);


    jfieldID  m_fid_time = env->GetFieldID(clazz,"time","J");
    env->SetLongField(account,m_fid_time,110L);

    jfieldID  m_fid_name = env->GetFieldID(clazz,"name","Ljava/lang/String;");
    jstring cName = env->NewStringUTF("jni name"); //构造一个jstring对象
    env->SetObjectField(account,m_fid_name,cName);

    jfieldID  m_fid_pwd = env->GetFieldID(clazz,"pwd","Ljava/lang/String;");
    jstring cPwd = env->NewStringUTF("jni name"); //构造一个jstring对象
    env->SetObjectField(account,m_fid_pwd,cPwd);

    return account;
}


JNIEXPORT jint JNICALL Java_com_sogou_speech_utils_DataEngine_checkout(JNIEnv * env, jclass jclazz, jobject account, jobject callback){

    jclass clazz  = env->GetObjectClass(account); //获得account类引用

    jfieldID timeFieldID = env->GetFieldID(clazz,"time","J"); //获得得Student类的属性id
    jfieldID nameFieldID = env->GetFieldID(clazz,"name","Ljava/lang/String;"); // 获得属性ID
    jfieldID pwdFieldID = env->GetFieldID(clazz,"pwd","Ljava/lang/String;"); // 获得属性ID

    jlong time = env->GetLongField(account , timeFieldID);  //获得属性值
    LOGI("time: %d",time);

    jstring name = (jstring)env->GetObjectField(account , nameFieldID);//获得属性值
    const char * c_name = env->GetStringUTFChars(name ,NULL);//转换成 char *
    LOGI("name: %s",c_name);
    string str_name = c_name ;
    env->ReleaseStringUTFChars(name,c_name); //释放引用

    jstring pwd = (jstring)env->GetObjectField(account , pwdFieldID);//获得属性值
    const char * c_pwd = env->GetStringUTFChars(pwd ,NULL);//转换成 char *
    LOGI("pwd: %s",c_pwd);
    string str_pwd = c_pwd;
    env->ReleaseStringUTFChars(pwd,c_pwd); //释放引用

    jclass cls = env->GetObjectClass(callback);
    jmethodID callbackId = env->GetMethodID(cls,"callback","(ILjava/lang/String;)V");
    jstring msg = env->NewStringUTF("I am a Callback!");
    env->CallVoidMethod(callback,callbackId,-1,msg);

    return 0;
}

JNIEXPORT void JNICALL Java_com_sogou_speech_utils_DataEngine_catchException(JNIEnv *env, jobject jobj){

    throwException(env,"java/lang/IllegalArgumentException","thrown from C++ code");

    LOGI("pwd: %s","exception ...");
}



/*
头文件
    #include<pthread.h>

函数声明
    int pthread_create(pthread_t *tidp,const pthread_attr_t *attr,
    (void*)(*start_rtn)(void*),void *arg);

    第一个参数为指向线程标识符的指针。
    第二个参数用来设置线程属性。
    第三个参数是线程运行函数的起始地址。
    最后一个参数是运行函数的参数。
*/

// 线程执行的方法
void *thread_func(void *arg)
{
     if(NULL == arg) return NULL;

     LOGI("thread_func %s","run ...");

     jobject callback = (jobject)arg;

     JNIEnv *env = NULL;
     //Attach主线程
     if(g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK){
         LOGI("AttachCurrentThread() failed");
         if(NULL != env){
            env->DeleteGlobalRef(callback);
         }
         return NULL;
     }

     //找到对应的类
     jclass cls = env->GetObjectClass(callback);
     if(NULL != cls){
        //获得类中的方法
        jmethodID callbackId = env->GetMethodID(cls,"callback","(ILjava/lang/String;)V");
        if (callbackId != NULL){
            //最后调用java中的方法
            jstring msg = env->NewStringUTF("thread async success Callback!");
            env->CallVoidMethod(callback,callbackId,0,msg);
        }else{
            LOGI("GetMethodID() Error.....");
            if(g_jvm->DetachCurrentThread() != JNI_OK)
            {
                LOGE("DetachCurrentThread() failed");
            }

            if(NULL != env){
                env->DeleteGlobalRef(callback);
                callback = NULL;
            }

            throwException(env,"java/lang/RuntimeException","not found callback method");
        }
     }else{
          if(g_jvm->DetachCurrentThread() != JNI_OK)
          {
              LOGE("DetachCurrentThread() failed");
          }

          if(NULL != env){
              env->DeleteGlobalRef(callback);
              callback = NULL;
          }

          throwException(env,"java/lang/RuntimeException","not found class");
     }

    if(NULL != env && NULL != callback){
        env->DeleteGlobalRef(callback);
        callback = NULL;
    }
    return NULL;
}

JNIEXPORT void JNICALL Java_com_sogou_speech_utils_DataEngine_getHtml(JNIEnv * env, jclass jclazz, jobject callback){
    //保存全局JVM以便在子线程中使用
    if(NULL == g_jvm){
        env->GetJavaVM(&g_jvm);
    }

    //  转换为全局对象 注意用完后释放
    jobject globalCallback = env->NewGlobalRef(callback);

    pthread_t thread_id;
    int err = pthread_create(&thread_id, NULL, thread_func, globalCallback);

    LOGI("pthread_create");

    if (err != 0){
         // 线程创建失败 回调
         jclass cls = env->GetObjectClass(callback);
         if(NULL != cls){
            //获得类中的方法
            jmethodID callbackId = env->GetMethodID(cls,"callback","(ILjava/lang/String;)V");
            if (callbackId != NULL){
                //最后调用java中的方法
                jstring msg = env->NewStringUTF("thread async faild Callback!");
                env->CallVoidMethod(callback,callbackId,-1,msg);
            }
         }
    }

     // 该方法会导致 主线程在字线程结束之后才能接着运行 类似java里面的join方法 最好是不要使用
     // pthread_join(thread_id, NULL);

}



