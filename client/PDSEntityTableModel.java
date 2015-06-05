package client;
/*
 * PDSEntityTableModel.java
 *
 * Created on November 6, 2002, 11:13 AM
 */

/**
 *
 * @author  mansour
 * @version 1.0
 */
public class PDSEntityTableModel extends javax.swing.table.AbstractTableModel {

    public static final int FILE_NAME_ATTR_ID = 1;

    private String[] m_entity_names;
    private String[] m_entity_values;
    private int[] m_entity_types;
    private int[] m_entity_lengths;
    private String m_context_path;
    /******** IMPORTANT ************
     * These values MUST match the values
     * in pdsdefs.h
     */
    private final static int GetDataBuffer = 2;
    private final static int GetDataSize = 4;
    private final static int GetDataType = 8;

    public PDSEntityTableModel() {
        //System.out.println("entity table model");
        setContextPath("/");
    }

    public boolean isCellEditable(int row, int col) {
        return false;
    }
    /**
     * Get all entities in the given path and
     * also get all their values
     * @param context_path
     */
    public void setContextPath(String context_path) {

        m_context_path = context_path;
        m_entity_names = client.pds.PDSWrapper.get_entity_list(PDSGUIController.m_domain_id,
                m_context_path);
        m_entity_values = new String[m_entity_names.length];
        m_entity_types = new int[m_entity_names.length];
        m_entity_lengths = new int[m_entity_names.length];

        for (int i = 0; i < m_entity_names.length; i++) {
            StringBuffer type = new StringBuffer();
            StringBuffer value = new StringBuffer();
            StringBuffer length = new StringBuffer();
            String path = m_context_path;
            if (!"/".equals(m_context_path))
                path += "/";
            client.pds.PDSWrapper.get_entity_value(PDSGUIController.m_domain_id,
                    path + m_entity_names[i],
                    GetDataSize | GetDataType,
                    value,
                    type,
                    length);
            m_entity_types[i] = PDSEntityValueTypes.convert(type.toString());
            m_entity_lengths[i] = Integer.parseInt(length.toString());
            if (m_entity_types[i] != PDSEntityValueTypes.OPAQUE_TYPE) {
                client.pds.PDSWrapper.get_entity_value(PDSGUIController.m_domain_id,
                    path + m_entity_names[i],
                    0,
                    value,
                    type,
                    length);
                m_entity_values[i] = value.toString();
            }
            else {
                StringBuffer file_name = new StringBuffer();
                client.pds.PDSWrapper.get_entity_attr_value(PDSGUIController.m_domain_id,
                        path + m_entity_names[i],
                        FILE_NAME_ATTR_ID,
                        file_name);
                m_entity_values[i] = file_name.toString();
            }

            //System.out.println(m_entity_names[i] + ":" + type.toString() + ":" + length);
        }
        fireTableDataChanged();
    }

    public int getColumnCount() {
        return 2;
    }

    public String getColumnName(int col) {
        if (col == 0)
            return "Entity";
        else
            return "Value";
    }

    public int getRowCount() {
        return m_entity_names.length;
    }

    public Object getValueAt(int row, int col) {
        if (col == 0)
            return m_entity_names[row];
        else
            return m_entity_values[row];
    }

    public void setValueAt(java.lang.Object obj, int row, int col) {
        //
    }

    public int getRowLength(int row) {
        return m_entity_lengths[row];
    }

    public int getRowType(int row) {
        return m_entity_types[row];
    }
}
