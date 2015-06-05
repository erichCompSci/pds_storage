/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>

extern "C"
{
#include "atl.h"
}

#include "attrList.h"

static attr_list list;

/*
 * Class:     attrList
 * Method:    addAttr
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_attrList_addAttr__Ljava_lang_String_2I
  (JNIEnv *env, jobject o, jstring attrname, jint val)
{
  int rv;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  rv = add_attr (list, attr_atom_from_string (c_attrname), Attr_Int4, (attr_value) val);
  env->ReleaseStringUTFChars (attrname, c_attrname);
  return rv;
}

/*
 * Class:     attrList
 * Method:    addAttr
 * Signature: (Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL Java_attrList_addAttr__Ljava_lang_String_2J
  (JNIEnv *env, jobject o, jstring attrname, jlong val)
{
  int rv;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  rv = add_attr (list, attr_atom_from_string (c_attrname), Attr_Int8, (attr_value) val);
  env->ReleaseStringUTFChars (attrname, c_attrname);
  return rv;
}

/*
 * Class:     attrList
 * Method:    addAttr
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_attrList_addAttr__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject o, jstring attrname, jstring val)
{
  int rv;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  const char *c_attrval = env->GetStringUTFChars (val, (jboolean*)0);
  rv = add_attr (list, attr_atom_from_string (c_attrname), Attr_String, (attr_value) c_attrval);
  env->ReleaseStringUTFChars (attrname, c_attrname);
  env->ReleaseStringUTFChars (val, c_attrval);
  return rv;
}


/*
 * Class:     attrList
 * Method:    finalize
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_attrList_finalize (JNIEnv *env, jobject o)
{
  free_attr_list (list);
}

/*
 * Class:     attrList
 * Method:    myInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_attrList_myInit
  (JNIEnv *env, jobject o)
{
  list = create_attr_list();
}

/*
 * Class:     attrList
 * Method:    queryAttr
 * Signature: (Ljava/lang/String;I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_attrList_queryAttr
  (JNIEnv *env, jobject o, jstring attrname, jint attrtype)
{
  jobject robj;
  jclass clazz;
  jmethodID method;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  attr_value_type real_type;
  void* real_ptr;
  int i;
  long l;
  const char* s;

  query_attr (list, attr_atom_from_string (c_attrname), &real_type, &real_ptr);

  switch (real_type)
    {
    case Attr_Int4:
      i = *(reinterpret_cast<int*>(real_ptr));
      clazz = env->FindClass ("java/lang/Integer");
      method = env->GetMethodID (clazz, "<init>", "void(V)");
      robj = env->NewObject (clazz, method, i);
      break;
    case Attr_Int8:
      l = *(reinterpret_cast<long*>(real_ptr));
      clazz = env->FindClass ("java/lang/Long");
      method = env->GetMethodID (clazz, "<init>", "void(V)");
      robj = env->NewObject (clazz, method, l);
      break;
    case Attr_String:
      s = reinterpret_cast<const char*>(real_ptr);
      robj = env->NewStringUTF (s);
      break;
    default:
      robj = NULL;
    }

  env->ReleaseStringUTFChars (attrname, c_attrname);
  return robj;
}


/*
 * Class:     attrList
 * Method:    setAttr
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_attrList_setAttr__Ljava_lang_String_2I
  (JNIEnv *env, jobject o, jstring attrname, jint val)
{
  int rv;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  rv = set_attr (list, attr_atom_from_string (c_attrname), Attr_Int4, (attr_value) val);
  env->ReleaseStringUTFChars (attrname, c_attrname);
  return rv;
}

/*
 * Class:     attrList
 * Method:    setAttr
 * Signature: (Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL Java_attrList_setAttr__Ljava_lang_String_2J
  (JNIEnv *env, jobject o, jstring attrname, jlong val)
{
  int rv;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  rv = set_attr (list, attr_atom_from_string (c_attrname), Attr_Int8, (attr_value) val);
  env->ReleaseStringUTFChars (attrname, c_attrname);
  return rv;
}

/*
 * Class:     attrList
 * Method:    setAttr
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_attrList_setAttr__Ljava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject o, jstring attrname, jstring val)
{
  int rv;
  const char *c_attrname = env->GetStringUTFChars (attrname, (jboolean*)0);
  const char *c_attrval = env->GetStringUTFChars (val, (jboolean*)0);
  rv = set_attr (list, attr_atom_from_string (c_attrname), Attr_String, (attr_value) c_attrval);
  env->ReleaseStringUTFChars (attrname, c_attrname);
  env->ReleaseStringUTFChars (val, c_attrval);
  return rv;
}

