package client.pds;

import java.lang.reflect.Method;


public class PDSWrapper {
    native static public void start_service(String PDSHostName, int PDSPortNumber);
    native static public void stop_service();

    native static public PDSDomainID open_domain(
                                          String name,
                                          String type,
                                          int version,
                                          String application);
    native static public PDSContextID get_root_context(PDSDomainID domain);

    native static public String[] get_context_list(PDSDomainID domain,
                                                   PDSContextID context);

    native static public String[] get_context_list(PDSDomainID domain,
                                                   String parent_context_path);

    native static public String[] get_entity_list(PDSDomainID domain,
                                                   String context_path);

    native static public void create_context(PDSDomainID domain,
                                             String context_path);

    native static public void create_entity(PDSDomainID domain,
                                             String entity_name);
    native static public void delete_entity(PDSDomainID domain,
                                             String entity_name);
    native static public void delete_context(PDSDomainID domain,
                                             String context_name);

    native static public void set_entity_value(PDSDomainID domain,
                                               String entity_name,
                                               String entityType,
                                               String entityValue);

    native static public void set_entity_attr_value(PDSDomainID domain,
                                                    String entity_name,
                                                    int attr_id,
                                                    String attr_type,
                                                    String attr_value);

    /**
     *@param entityType an out param
     *@param entityValue an out param
     */
    native static public void get_entity_value(PDSDomainID domain,
                                               String entity_name,
                                               int mask,
                                               StringBuffer entityType,
                                               StringBuffer entityValue,
                                               StringBuffer entityLength);

    native static public void get_entity_attr_value(PDSDomainID domain,
                                                   String entity_name,
                                                   int attr_id,
                                                   StringBuffer attr_value);

    /**
     *@param value an out param
     */
    native static public void get_entity_blob_value(PDSDomainID domain,
                                                   String entity_name,
                                                   byte[] value);

	native static public String[]  xpath_query(PDSDomainID domain, String xpath);
	
    // change notification stuff...
    // this requires a callback and a separate thread to call
    // the CM_Poll

    native static public void cm_poll();
    native static public void register_for_domain_change(PDSDomainID domain, Method callback);

    static {    	
        System.loadLibrary("pdswrapper");
    }


}
