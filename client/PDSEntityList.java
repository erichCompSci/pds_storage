package client;
/*
 * PDSEntityList.java
 *
 * Created on November 1, 2002, 2:20 PM
 */

/**
 *
 * @author  mansour
 * @version 
 */
public class PDSEntityList {
    private PDSEntityTableModel m_model;        
    private String m_context_path;
    private boolean m_init;
    /** Creates new PDSEntityList */
    public PDSEntityList(String context_path) {
        m_context_path = context_path;        
        m_init = false;
    }
    /**
     * Get the current context path pointed to by this list
     * @return /full/context/path/name
     */
    public String getContextPath() {
        return m_context_path;        
    }
    /** 
     * Get the internal DefaultListModel
     */
    public PDSEntityTableModel getEntityTableModel() {
        init();
        return m_model;
    }
    /**
     * Update this model with contents of a new context path
     */
    public void setContextPath(String path) {
        m_context_path = path;                
        m_model.setContextPath(path);                
    }
    /**
     * Initialize this object
     */
    private void init() {                        
        if (m_init == true)
            return;
        m_init = true;        
        m_model = new PDSEntityTableModel();                        
        setContextPath(m_context_path);        
    }   
}
