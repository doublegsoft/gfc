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
gfc_jvm_main(const char* joptions, const char* clazz, int argc, const char* argv[])
{
  JavaVM*           vm;
  JNIEnv*           env;
  JavaVMInitArgs    vm_args;
  jint              res;
  jclass            cls;
  jmethodID         mid;
  jstring           jstr;
  jobjectArray      main_args;

  JavaVMOption options[1];
  options[0].optionString     = (char*) joptions;
  vm_args.options             = options;
  vm_args.nOptions            = 1;
  vm_args.version             = JNI_VERSION_1_8;
  res = JNI_CreateJavaVM(&vm, (void **)&env, &vm_args);
  if (res != JNI_OK) {
    printf("Failed to create Java VM\n");
    return 1;
  }

  cls = (*env)->FindClass(env, clazz);
  if (cls == NULL) {
    printf("Failed to find Main class\n");
    return 1;
  }

  mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
  if (mid == NULL) {
    printf("Failed to find main functionn");
    return 1;
  }

  int argidx = 0;
  jstr      = (*env)->NewStringUTF(env, "hello");
  main_args = (*env)->NewObjectArray(env, argc, (*env)->FindClass(env, "java/lang/String"), jstr);

  for (argidx = 0; argidx < argc; argidx++)
  {
    jstring str = (*env)->NewStringUTF(env, argv[argidx]);
    (*env)->SetObjectArrayElement(env, main_args, argidx, str);
  }

  (*env)->CallStaticVoidMethod(env, cls, mid, main_args);

  fflush(stdout);
  return 0;
}
