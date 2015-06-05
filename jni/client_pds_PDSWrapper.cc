#include "client_pds_PDSWrapper.h"
#include "agent/pds.h"
#include "atl.h"
#include "evpath.h"


#include <string.h>
#include <stdlib.h>
#include <assert.h>
//#include <iostream>
//#include <fstream>

//using std::ifstream;

static pds_service g_directory;

#define PDSCLIENT_FILE_NAME ATL_CHAR_CONS('C', 'I', 'P', 'N')

extern "C" {

jobject domain_id_c2j(JNIEnv* env, pds_domain_id_t& c_domain_id);
pds_domain_id_t domain_id_j2c(JNIEnv* env, jobject j_domain_id);
jobject context_id_c2j(JNIEnv*env, pds_context_id_t c_context_id);
pds_context_id_t context_id_j2c(JNIEnv*env, jobject j_context_id);
void free_stringArray(char** bindings);
jobjectArray stringArray_c2j(JNIEnv* env, char**bindings);
void setStringBuffer(JNIEnv* env, jobject sb, const char* str);
/*
 * Class:     pds_PDSWrapper
 * Method:    start_service
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_start_1service
  (JNIEnv *env, jclass, jstring host, jint port)
{
    attr_list contact_attrs;

    printf("Started the service\n");
    const char *str = env->GetStringUTFChars(host, 0);
    contact_attrs = create_attr_list();
    set_attr (contact_attrs, attr_atom_from_string("PDS_IP_HOST"), Attr_String, strdup (str) );
    set_attr (contact_attrs, attr_atom_from_string("PDS_IP_PORT"), Attr_Int4, (attr_value*)port);
    g_directory = pds_service_open (contact_attrs);
    env->ReleaseStringUTFChars(host, str);
    printf("Started the service\n");
}

/*
 * Class:     pds_PDSWrapper
 * Method:    stop_service
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_stop_1service
  (JNIEnv *, jclass)
{
    pds_service_close(g_directory);
}

/*
 * Class:     pds_PDSWrapper
 * Method:    open_domain
 * Signature: (Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)Lclient_pds/PDSDomainID;
 */
JNIEXPORT jobject JNICALL Java_client_pds_PDSWrapper_open_1domain
  (JNIEnv *env, jclass, jstring domain, jstring type, jint version, jstring application)
{
    const char *dom_str = env->GetStringUTFChars(domain, 0);
    const char *type_str = env->GetStringUTFChars(type, 0);
    const char *app_str = env->GetStringUTFChars(application, 0);
    pds_domain_id_t c_domain_id = pds_open_domain (g_directory,
                                 dom_str,
                                 type_str,
                                 version,
                                 app_str);
    
    env->ReleaseStringUTFChars(domain, dom_str);
    env->ReleaseStringUTFChars(type, type_str);
    env->ReleaseStringUTFChars(application, app_str);
    jobject rv = domain_id_c2j(env, c_domain_id);
    return rv;
}
/*
 * Class:     pds_PDSWrapper
 * Method:    get_root_context
 * Signature: (Lclient_pds/PDSDomainID;)Lclient_pds/PDSContextID;
 */
JNIEXPORT jobject JNICALL Java_client_pds_PDSWrapper_get_1root_1context
  (JNIEnv *env, jclass, jobject j_domain_id)
{
     assert(j_domain_id != 0);
     pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
     pds_context_id_t c_root_context_id = pds_get_root_context(c_domain_id);
     return context_id_c2j(env, c_root_context_id);
}
/*
 * Class:     pds_PDSWrapper
 * Method:    get_context_list
 * Signature: (Lclient_pds/PDSDomainID;Lclient_pds/PDSContextID;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_get_1context_1list__Lclient_pds_PDSDomainID_2Lclient_pds_PDSContextID_2
  (JNIEnv *env, jclass, jobject j_domain_id, jobject j_context_id)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    pds_context_id_t c_context_id = context_id_j2c(env, j_context_id); 


    char** bindings = pds_get_binding_list_id(c_domain_id, c_context_id, OnlyContextBindings);
    jobjectArray rv = stringArray_c2j(env, bindings);
    free_stringArray(bindings);
    return rv;
}
/*
 * Class:     pds_PDSWrapper
 * Method:    get_entity_list
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_get_1entity_1list
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_context_path)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_context_path = env->GetStringUTFChars(j_context_path, 0);

    char** bindings = pds_get_binding_list(c_domain_id, c_context_path, OnlyEntityBindings);

    jobjectArray rv = stringArray_c2j(env, bindings);
    env->ReleaseStringUTFChars(j_context_path, c_context_path);
    free_stringArray(bindings);
    return rv;
}


/*
 * Class:     pds_PDSWrapper
 * Method:    get_context_list
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_get_1context_1list__Lclient_pds_PDSDomainID_2Ljava_lang_String_2
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_context_path)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_context_path = env->GetStringUTFChars(j_context_path, 0);
    
    char** bindings = pds_get_binding_list(c_domain_id, c_context_path, OnlyContextBindings);

    jobjectArray rv = stringArray_c2j(env, bindings);
    env->ReleaseStringUTFChars(j_context_path, c_context_path);
    free_stringArray(bindings);
    return rv;
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    create_context
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_create_1context
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_context_name)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_context_name = env->GetStringUTFChars(j_context_name, 0);
    pds_create_context(c_domain_id, c_context_name, null_pds_context_id);
    env->ReleaseStringUTFChars(j_context_name, c_context_name);
}

/***********************************************/

/*
 * Class:     pds_PDSWrapper
 * Method:    create_entity
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_create_1entity
  (JNIEnv *env , jclass, jobject j_domain_id, jstring j_entity_name)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);
    

    attr_list attrs = create_attr_list();
    pds_create_entity(c_domain_id, c_entity_name, null_pds_context_id, 0, attrs);
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    delete_entity
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_delete_1entity
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_entity_name)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);

    pds_remove_entity(c_domain_id, c_entity_name, null_pds_context_id);
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    delete_context
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_delete_1context
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_context_name)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_context_name = env->GetStringUTFChars(j_context_name, 0);
    
    int res = pds_remove_context(c_domain_id, c_context_name, null_pds_context_id);
    env->ReleaseStringUTFChars(j_context_name, c_context_name);
}

/***********************************************/
long get_file_size(FILE* file)
{
    fseek(file, 0, 0);
    long size= 0;
    while (fgetc(file) != EOF)
      size++;

    fseek(file, 0, 0);
    return size;
}
/***********************************************/
/* Class:     pds_PDSWrapper
 * Method:    set_entity_value
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_set_1entity_1value
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_entity_name, jstring j_entity_type, jstring j_entity_value)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);
    const char* c_entity_value = env->GetStringUTFChars(j_entity_value, 0);
    const char* c_entity_type = env->GetStringUTFChars(j_entity_type, 0);
    
    pds_entity_data_t data;
    if (strcmp(c_entity_type, "Int4") == 0) {
        data.data = (unsigned char*)c_entity_value;
        data.data_size = strlen(c_entity_value);
        data.data_type = Attr_Int4;
        pds_set_entity_data(c_domain_id, c_entity_name, null_pds_context_id, &data, 0);
    
    } else if (strcmp(c_entity_type, "Int8") == 0) {
        data.data = (unsigned char*)c_entity_value;
        data.data_size = strlen(c_entity_value);
        data.data_type = Attr_Int8;
        pds_set_entity_data(c_domain_id, c_entity_name, null_pds_context_id, &data, 0);
    
    } else if (strcmp(c_entity_type, "String") == 0) {
        data.data = (unsigned char*)c_entity_value;
        data.data_size = strlen(c_entity_value);
        data.data_type = Attr_String;
        pds_set_entity_data(c_domain_id, c_entity_name, null_pds_context_id, &data, 0);
    
    } else if (strcmp(c_entity_type, "Blob") == 0) {
        long size =0;
        char * buffer =0;
        FILE *file;
        file=fopen(c_entity_value, "rb");
        if (file == 0) 
            printf("error opening file %s\n", c_entity_value);
        else {
            size = get_file_size(file);
            buffer = new char [size];
            fread(buffer, size, 1, file);
            fclose(file);
            file = 0;
            data.data = (unsigned char*)buffer;
            data.data_size = size;
            data.data_type = Attr_Opaque;        
            attr_list attrs = create_attr_list();   
            //set_attr (attrs, PDSCLIENT_FILE_NAME, Attr_String, strdup (c_entity_value) );
            pds_set_entity_data(c_domain_id, c_entity_name, null_pds_context_id, &data, 0);
            //pds_set_entity_attributes(c_domain_id, c_entity_name, null_pds_context_id, attrs, 0);
            delete [] buffer;
        }
    }          
    env->ReleaseStringUTFChars(j_entity_type, c_entity_type);
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
    env->ReleaseStringUTFChars(j_entity_value, c_entity_value);
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    set_entity_attr_value
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_set_1entity_1attr_1value
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_entity_name, jint j_attr_id, jstring j_attr_type, jstring j_attr_value)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);
    const char* c_attr_type = env->GetStringUTFChars(j_attr_type , 0);
    const char* c_attr_value = env->GetStringUTFChars(j_attr_value, 0);

    attr_list attrs = create_attr_list();
    if (strcmp(c_attr_type, "Int4") == 0) {
        set_attr(attrs, j_attr_id, Attr_Int4, (attr_value*) atoi(c_attr_value));
    
    } else if (strcmp(c_attr_type, "Int8") == 0) {
        set_attr(attrs, j_attr_id, Attr_Int8, (attr_value*) atol(c_attr_value));
    
    } else if (strcmp(c_attr_type, "String") == 0) {
        set_attr(attrs, j_attr_id, Attr_String, strdup(c_attr_value));
    
    } else if (strcmp(c_attr_type, "Blob") == 0) {
        printf("not implemented yet!");
    }

    pds_set_entity_attributes(c_domain_id, c_entity_name, null_pds_context_id, attrs, 0);

    free_attr_list(attrs);
    
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
    env->ReleaseStringUTFChars(j_attr_type, c_attr_type);
    env->ReleaseStringUTFChars(j_attr_value, c_attr_value);
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    get_entity_attr_value
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;ILjava/lang/StringBuffer;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_get_1entity_1attr_1value
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_entity_name, jint j_attr_id, jobject j_attr_value)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);
  
    
    attr_list attrs = create_attr_list();
    pds_get_entity_attributes(c_domain_id, c_entity_name, null_pds_context_id, attrs);
    

    attr_value_type type;
    attr_value value;
    int ret = query_attr(attrs, j_attr_id, &type, &value);
    
    
    char buffer[1000];
    if (type == Attr_String) {
        sprintf(buffer, "%s", (char*)value);
    } else if (type == Attr_Int4) {
        sprintf(buffer, "%d", (int*)value);
    } else if (type == Attr_Int8) {
        sprintf(buffer, "%lu", (unsigned long*)value);
    } else if (type == Attr_Opaque) {
        sprintf(buffer, "OPAQUE!!");
    } 
    setStringBuffer(env, j_attr_value, buffer); 
    
    free_attr_list(attrs);
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    get_entity_value
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;ILjava/lang/StringBuffer;Ljava/lang/StringBuffer;Ljava/lang/StringBuffer;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_get_1entity_1value
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_entity_name, jint j_mask, jobject j_entity_value, jobject j_entity_type, jobject j_entity_length)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id); 
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);
    pds_entity_data_t data;
    pds_get_entity_data(c_domain_id, c_entity_name, 
                        null_pds_context_id, &data, j_mask);
    // copy the data
    char buffer[10];
    switch(data.data_type) {
        case Attr_String:
            sprintf(buffer, "String");
            break;
        case Attr_Opaque:
            sprintf(buffer, "Blob");
            break;
        case Attr_Int4:
            sprintf(buffer, "Int4");
            break;
        case Attr_Int8:
            sprintf(buffer, "Int8");
            break;
        default:
            sprintf(buffer, "Unknown");
    }
    if (data.data_size > 0 
        && ( j_mask == 0 || (j_mask & GetDataBuffer))) {
       data.data[data.data_size] = 0;
       setStringBuffer(env, j_entity_value, (const char*) data.data);
    }
    setStringBuffer(env, j_entity_type, buffer); 
    sprintf(buffer, "%lu", data.data_size);
    setStringBuffer(env, j_entity_length, buffer); 
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
}
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    get_entity_blob_value
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_get_1entity_1blob_1value
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_entity_name, jbyteArray j_opaque_value)
{
  pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id);
    const char* c_entity_name = env->GetStringUTFChars(j_entity_name, 0);
    pds_entity_data_t data;
    pds_get_entity_data(c_domain_id, c_entity_name,
                        null_pds_context_id, &data, 0);

    env->SetByteArrayRegion(j_opaque_value, 0, data.data_size, (jbyte*)data.data);
    env->ReleaseStringUTFChars(j_entity_name, c_entity_name);
}

/***********************************************/

/*
 * Class:     client_pds_PDSWrapper
 * Method:    xpath_query
 * Signature: (Lclient/pds/PDSDomainID;Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_client_pds_PDSWrapper_xpath_1query
  (JNIEnv *env, jclass, jobject j_domain_id, jstring j_xpath)
{
    pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id);
    const char* c_xpath = env->GetStringUTFChars(j_xpath, 0);
    char** list = pds_query_by_xpath(c_domain_id, c_xpath);
    jobjectArray rv = stringArray_c2j(env, list);
    free_stringArray(list);
    env->ReleaseStringUTFChars(j_xpath, c_xpath);
    return rv;
}

static CManager g_cm;
int domain_event_handler (CManager cm, void* , void* , attr_list );
/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    cm_poll
 * Signature: ()V
 */
//// *************************************///////
//// This static can only be used in 
//// domain_event_handler.
//// It is set in cm_poll and is only valid
//// for the duration of that call.
//// ************************************//////
static JNIEnv *g_env_for_domain_change_events = 0;

JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_cm_1poll
  (JNIEnv *env, jclass)
{
  g_env_for_domain_change_events = env;
  CMpoll_network(g_cm);
  g_env_for_domain_change_events = 0;
}
/***********************************************/
int
domain_event_handler (CManager cm, void* event, void* client_data, attr_list event_list)
{
  if (0 == g_env_for_domain_change_events)
     return 0;
  jclass cls = g_env_for_domain_change_events->FindClass("client/PDSGUIController");
  jmethodID mid = g_env_for_domain_change_events->GetStaticMethodID(cls, "refresh", "()V");
  if (mid == 0) {
     printf("[pds change notification] could not find java callback method\n");
     return 0;
  }
  //printf("Calling callback method\n");
  g_env_for_domain_change_events->CallStaticVoidMethod(cls, mid);
  return 1;
}

/***********************************************/
/*
 * Class:     pds_PDSWrapper
 * Method:    register_for_domain_change
 * Signature: (Lclient_pds/PDSDomainID;Ljava/lang/reflect/Method;)V
 */
JNIEXPORT void JNICALL Java_client_pds_PDSWrapper_register_1for_1domain_1change
  (JNIEnv *env, jclass, jobject j_domain_id, jobject callback)
{ 
   g_cm = CManager_create();
   pds_domain_id_t c_domain_id = domain_id_j2c(env, j_domain_id);
   pds_register_for_domain_changes(g_cm, c_domain_id, domain_event_handler, 0);
}
/*********************************************/
void setStringBuffer(JNIEnv* env, jobject sb, const char* str)
{
    if (str == 0)
       return;
    jstring newString = env->NewStringUTF(str);
    jclass clazz= env->GetObjectClass(sb);
    assert(clazz != 0);

    jmethodID method = env->GetMethodID(clazz, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    assert(method != 0);

    env->CallObjectMethod(sb, method, newString); 
}
/***********************************************/
void free_stringArray(char** bindings)
{
  for (int i=0; bindings[i] != 0; i++)
    free(bindings[i]);
  free(bindings);
}

/***********************************************/
jobjectArray stringArray_c2j(JNIEnv* env, char**bindings)
{
    int size = 0;
    int i;
    for (i=0; bindings[i] != 0; i++)
      size++;

    jstring j_string = env->NewStringUTF("");
    assert(j_string != 0);

    jclass stringClass = env->FindClass("java/lang/String");
    assert(stringClass != 0);
    jobjectArray rv = env->NewObjectArray(size,
                                       stringClass,
                                       j_string);
    for (i=0; bindings[i] != 0; i++) {
      pds_context_id_t c_cur_context_id;
      jstring j_context = env->NewStringUTF(bindings[i]);
      env->SetObjectArrayElement(rv, i, j_context);
    }
    return rv;
}
/***********************************************/
jobject domain_id_c2j(JNIEnv* env, pds_domain_id_t& c_domain_id)
{
    jclass domainClass = env->FindClass("client/pds/PDSDomainID");
    assert(domainClass != 0);

    jfieldID idField = env->GetFieldID(domainClass, "_id", "[B");
    assert(idField != 0);

    jmethodID idMethod = env->GetMethodID(domainClass, "<init>", "()V");
    jobject j_domain_id = env->NewObject(domainClass, idMethod);

    jbyteArray ba = (jbyteArray) env->GetObjectField(j_domain_id, idField);
    assert(ba != 0);

    jbyte* myBa = env->GetByteArrayElements(ba, 0);
    assert(myBa != 0);
    for (int i=0; i<32; i++) {
        myBa[i] = (jbyte) c_domain_id.id[i];
    }
    env->ReleaseByteArrayElements(ba, myBa, 0);

    return j_domain_id;
}
/***********************************************/
pds_domain_id_t domain_id_j2c(JNIEnv* env, jobject j_domain_id)
{
    assert(j_domain_id != 0);
    jclass domainClass = env->FindClass("client/pds/PDSDomainID");
    assert(domainClass != 0);

    jfieldID idField = env->GetFieldID (domainClass, "_id", "[B");
    assert(idField != 0);

    jbyteArray ba = (jbyteArray) env->GetObjectField(j_domain_id, idField);
    assert(ba != 0);

    jbyte* myBa = env->GetByteArrayElements (ba, 0);
    assert(myBa != 0);

    pds_domain_id_t c_domain_id;
    for (int i = 0; i < 32; i++) {
        c_domain_id.id[i] = (unsigned char)myBa[i];
    }

    env->ReleaseByteArrayElements (ba, myBa, 0);
    return c_domain_id;
}
/***********************************************/
jobject context_id_c2j(JNIEnv*env, pds_context_id_t c_context_id)
{
    jclass contextClass = env->FindClass("client/pds/PDSContextID");
    assert(contextClass != 0);

    jfieldID idField = env->GetFieldID(contextClass, "_id", "[B");
    assert(idField != 0);

    jmethodID idMethod = env->GetMethodID(contextClass, "<init>", "()V");
    jobject j_context_id = env->NewObject(contextClass, idMethod);

    jbyteArray ba = (jbyteArray) env->GetObjectField(j_context_id, idField);
    assert(ba != 0);

    jbyte* myBa = env->GetByteArrayElements(ba, 0);
    assert(myBa != 0);

    for (int i=0; i<32; i++) {
        myBa[i] = (jbyte) c_context_id.id[i];
    }
    env->ReleaseByteArrayElements(ba, myBa, 0);

   return j_context_id;

}
/***********************************************/
pds_context_id_t context_id_j2c(JNIEnv*env, jobject j_context_id)
{
    pds_context_id_t c_context_id;
    jclass contextClass = env->GetObjectClass(j_context_id);
    assert(contextClass != 0);

    jfieldID idField = env->GetFieldID (contextClass, "_id", "[B");
    assert(idField != 0);

    jbyteArray ba = (jbyteArray) env->GetObjectField (j_context_id, idField);
    assert(ba != 0);

    jbyte* myBa = env->GetByteArrayElements (ba, 0);
    assert(myBa != 0);

    for (int i = 0; i < 32; i++) {
        c_context_id.id[i] = (unsigned char)myBa[i];
    }

    env->ReleaseByteArrayElements (ba, myBa, 0);
    return c_context_id;
}

}
