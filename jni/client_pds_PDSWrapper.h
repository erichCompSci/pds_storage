/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class client_pds_PDSWrapper */

#ifndef _Included_client_pds_PDSWrapper
#define _Included_client_pds_PDSWrapper
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     client_pds_PDSWrapper
 * Method:    start_service
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_start_1service
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    stop_service
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_stop_1service
  (JNIEnv *, jclass);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    open_domain
 * Signature: (Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)Lclient/pds/PDSDomainID;
 */
JNIEXPORT jobject JNICALL Java_client_pds_PDSWrapper_open_1domain
  (JNIEnv *, jclass, jstring, jstring, jint, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_root_context
 * Signature: (Lclient/pds/PDSDomainID;)Lclient/pds/PDSContextID;
 */
JNIEXPORT jobject JNICALL Java_client_pds_PDSWrapper_get_1root_1context
  (JNIEnv *, jclass, jobject);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_context_list
 * Signature: (Lclient/pds/PDSDomainID;Lclient/pds/PDSContextID;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_get_1context_1list__Lclient_pds_PDSDomainID_2Lclient_pds_PDSContextID_2
  (JNIEnv *, jclass, jobject, jobject);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_context_list
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_get_1context_1list__Lclient_pds_PDSDomainID_2Ljava_lang_String_2
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_entity_list
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_get_1entity_1list
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    create_context
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_create_1context
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    create_entity
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_create_1entity
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    delete_entity
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_delete_1entity
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    delete_context
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_delete_1context
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    set_entity_value
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_set_1entity_1value
  (JNIEnv *, jclass, jobject, jstring, jstring, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    set_entity_attr_value
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_set_1entity_1attr_1value
  (JNIEnv *, jclass, jobject, jstring, jint, jstring, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_entity_value
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;ILjava/lang/StringBuffer;Ljava/lang/StringBuffer;Ljava/lang/StringBuffer;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_get_1entity_1value
  (JNIEnv *, jclass, jobject, jstring, jint, jobject, jobject, jobject);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_entity_attr_value
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;ILjava/lang/StringBuffer;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_get_1entity_1attr_1value
  (JNIEnv *, jclass, jobject, jstring, jint, jobject);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    get_entity_blob_value
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_get_1entity_1blob_1value
  (JNIEnv *, jclass, jobject, jstring, jbyteArray);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    xpath_query
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_xpath_1query
  (JNIEnv *, jclass, jobject, jstring);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    cm_poll
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_cm_1poll
  (JNIEnv *, jclass);

/*
 * Class:     client_pds_PDSWrapper
 * Method:    register_for_domain_change
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/reflect/Method;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_register_1for_1domain_1change
  (JNIEnv *, jclass, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
