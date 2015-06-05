package client;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.*;
import java.io.*;

/*
 * PDSEntityPopupMenu.java
 *
 * Created on November 11, 2002, 10:31 PM
 */

/**
 *
 * @author  mansour
 * @version 1.0
 *
 */
public class PDSEntityPopupMenu {

    static public JTable m_entityTable;
    static public JPopupMenu m_popupMenu;

    static private Action m_copyValue;
    static private Action m_saveBlob;
    static private Action m_editValue;
    static private Action m_launchValue;

    static public String m_pathToBrowser = "netscape";

    /** Creates new PDSEntityPopupMenu */
    public PDSEntityPopupMenu() {
        m_popupMenu = new JPopupMenu();
        init();
    }
    /**
     * Initialize the static vars of this class
     * menu actions are intiialized once here
     * popupmenu is re-configured every time
     * based on the context using these actions.
     *
     */
    private void init() {

        m_editValue = new AbstractAction("Edit Value ...") {
            public void actionPerformed(ActionEvent e) {
                PDSEntityPopupMenu.editValue();
            }
        };
        m_copyValue = new AbstractAction("Copy Value") {
            public void actionPerformed(ActionEvent e) {
                PDSEntityPopupMenu.copyValue();
            }
        };
        m_saveBlob = new AbstractAction("Save ...") {
            public void actionPerformed(ActionEvent e) {
                saveBlob();
            }
        };
        m_launchValue = new AbstractAction("View ...") {
            public void actionPerformed(ActionEvent e) {
                viewBlob();
            }
        };
    }
    /**
     * View a blob
     * Saves the blob in the temp directory and launches
     * a browser to view the saved file.
     */
    private void viewBlob() {
        int row = m_entityTable.getSelectedRow();
        if (row == -1) {
            System.out.println("no selection");
            return;
        }
        String entity = (String) PDSGUIController.getEntityList().getEntityTableModel().getValueAt(row, 0);
        String file_name = (String) PDSGUIController.getEntityList().getEntityTableModel().getValueAt(row, 1);
        file_name = System.getProperty("java.io.tmpdir")
                  + System.getProperty("file.separator")
                  + file_name;

        int length = PDSGUIController.getEntityList().getEntityTableModel().getRowLength(row);
        try {

            File file = new File (file_name);
            byte[] value = new byte[length];

            client.pds.PDSWrapper.get_entity_blob_value(PDSGUIController.m_domain_id,
                    PDSGUIController.getSelectedContextPath() + "/" + entity,
                    value);

            FileOutputStream os = new FileOutputStream(file);
            os.write(value);
            os.close();
            try {
                Runtime.getRuntime().exec(new String[] {m_pathToBrowser, file_name});
            } catch (Exception e) {
                e.printStackTrace();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    /**
     * Edit the entity data
     * Launches the EditEntityDialog to
     * handle the edit
     */
    private static void editValue() {
        int row = m_entityTable.getSelectedRow();
        if (row == -1) {
            System.out.println("no selection");
            return;
        }
        int entityType = PDSGUIController.getEntityList().getEntityTableModel().getRowType(row);
        String entityName = (String) PDSGUIController.getEntityList().getEntityTableModel().getValueAt(row, 0);
        String entityValue = (String) PDSGUIController.getEntityList().getEntityTableModel().getValueAt(row, 1);
        new PDSEditEntityDialog(new JFrame(), true, entityName, PDSEntityValueTypes.convert(entityType), entityValue).show();
    }
    /**
     * Saves the entity's data of type blob
     * Brings up a file chooser to select the
     * file name.
     */
    static public void saveBlob() {
        int row = m_entityTable.getSelectedRow();
        if (row == -1) {
            System.out.println("no selection");
            return;
        }
        String entity = (String) PDSGUIController.getEntityList().getEntityTableModel().getValueAt(row, 0);
        int length = PDSGUIController.getEntityList().getEntityTableModel().getRowLength(row);
        File file = null;
        try {
            JFileChooser chooser = new JFileChooser();
            int returnVal = chooser.showOpenDialog(m_popupMenu.getParent());
            if (returnVal != JFileChooser.APPROVE_OPTION)
                return;
            file = chooser.getSelectedFile();
            byte[] value = new byte[length];

            client.pds.PDSWrapper.get_entity_blob_value(PDSGUIController.m_domain_id,
                    PDSGUIController.getSelectedContextPath() + "/" + entity,
                    value);

            FileOutputStream os = new FileOutputStream(file);
            os.write(value);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * Event handler for mouse events on the entity table
     * Checks if a row is selected, and if so builds the
     * correspoding popup menu and shows it.
     * @param evt
     */
    static void mayShowPopup(java.awt.event.MouseEvent evt) {
        if (evt.isPopupTrigger()) {
            int x = evt.getX();
            int y = evt.getY();
            Point p = new Point(x, y);
            int row = m_entityTable.rowAtPoint(p);
            int col = m_entityTable.columnAtPoint(p);
            m_entityTable.setRowSelectionInterval(row, row);
            m_entityTable.setColumnSelectionInterval(col, col);

            m_popupMenu.removeAll();
            m_popupMenu.add(m_editValue);
            PDSEntityTableModel entity_model = (PDSEntityTableModel) m_entityTable.getModel();
            if (entity_model.getRowType(row) == PDSEntityValueTypes.OPAQUE_TYPE) {
                m_popupMenu.add(m_saveBlob);
                m_popupMenu.add(m_launchValue);
            }
            else
                m_popupMenu.add(m_copyValue);

            m_popupMenu.show(evt.getComponent(), evt.getX(), evt.getY());
        }
    }

    /**
     * Copy the selected entity data to the system
     * clipboard
     */
    static void copyValue() {
        int row = m_entityTable.getSelectedRow();
        if (row == -1) {
            System.out.println("no selection");
            return;
        }
        String sel = (String) PDSGUIController.getEntityList().getEntityTableModel().getValueAt(row, 1);
        PDSGUIController.m_systemClipboard.toClipboard(sel);
    }
}
