/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class jwpClient */

#ifndef _Included_jwpClient
#define _Included_jwpClient
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     jwpClient
 * Method:    bindBlob
 * Signature: (Ljwp/jwpDomainId;Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_jwpClient_bindBlob
  (JNIEnv *, jclass, jobject, jstring, jbyteArray);

/*
 * Class:     jwpClient
 * Method:    bindInt
 * Signature: (Ljwp/jwpDomainId;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_jwpClient_bindInt
  (JNIEnv *, jclass, jobject, jstring, jint);

/*
 * Class:     jwpClient
 * Method:    bindString
 * Signature: (Ljwp/jwpDomainId;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jwpClient_bindString
  (JNIEnv *, jclass, jobject, jstring, jstring);

/*
 * Class:     jwpClient
 * Method:    createDomain
 * Signature: (Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)Ljwp/jwpDomainId;
 */
JNIEXPORT jobject JNICALL Java_jwpClient_createDomain
  (JNIEnv *, jclass, jstring, jstring, jint, jstring);

/*
 * Class:     jwpClient
 * Method:    done
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jwpClient_done
  (JNIEnv *, jclass);

/*
 * Class:     jwpClient
 * Method:    findMatchingDomains
 * Signature: (Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)[Ljwp/jwpDomainId;
 */
JNIEXPORT jobjectArray JNICALL Java_jwpClient_findMatchingDomains
  (JNIEnv *, jclass, jstring, jstring, jint, jstring);

/*
 * Class:     jwpClient
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jwpClient_init
  (JNIEnv *, jclass);

/*
 * Class:     jwpClient
 * Method:    lookupBlob
 * Signature: (Ljwp/jwpDomainId;Ljava/lang/String;[Ljava/lang/Byte;)I
 */
JNIEXPORT jint JNICALL Java_jwpClient_lookupBlob
  (JNIEnv *, jclass, jobject, jstring, jobjectArray);

/*
 * Class:     jwpClient
 * Method:    lookupInt
 * Signature: (Ljwp/jwpDomainId;Ljava/lang/String;Ljava/lang/Integer;)I
 */
JNIEXPORT jint JNICALL Java_jwpClient_lookupInt
  (JNIEnv *, jclass, jobject, jstring, jobject);

/*
 * Class:     jwpClient
 * Method:    lookupString
 * Signature: (Ljwp/jwpDomainId;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jwpClient_lookupString
  (JNIEnv *, jclass, jobject, jstring, jstring);

#ifdef __cplusplus
}
#endif
#endif