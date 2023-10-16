/*
**           .d888
**          d88P"
**          888
**  .d88b.  888888 .d8888b
** d88P"88b 888   d88P"
** 888  888 888   888
** Y88b 888 888   Y88b.
**  "Y88888 888    "Y8888P
**      888
** Y8b d88P
**  "Y88P"
*/

#include <jni.h>
#include "gfc_jvm.h"

int
gfc_jvm_run()
{
  JavaVM         *vm;
  JNIEnv         *env;
  JavaVMInitArgs  vm_args;
  jint            res;
  jclass          cls;
  jmethodID       mid;
  jstring         jstr;
  jobjectArray    main_args;

  vm_args.version  = JNI_VERSION_1_8;
  vm_args.nOptions = 0;
  res = JNI_CreateJavaVM(&vm, (void **)&env, &vm_args);
  if (res != JNI_OK) {
    printf("Failed to create Java VMn");
    return 1;
  }

  cls = (*env)->FindClass(env, "Main");
  if (cls == NULL) {
    printf("Failed to find Main classn");
    return 1;
  }

  mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
  if (mid == NULL) {
    printf("Failed to find main functionn");
    return 1;
  }

  jstr      = (*env)->NewStringUTF(env, "");
  main_args = (*env)->NewObjectArray(env, 1, (*env)->FindClass(env, "java/lang/String"), jstr);
  (*env)->CallStaticVoidMethod(env, cls, mid, main_args);

  return 0;
}
