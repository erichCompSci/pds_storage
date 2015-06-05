package client;

/*
 * PSDNewContextDialog.java
 *
 * Created on November 3, 2002, 12:20 PM
 */

/**
 *
 * @author  mansour
 */
public class PDSNewContextDialog extends javax.swing.JDialog {
    /** A return status code - returned if Cancel button has been pressed */
    public static final int RET_CANCEL = 0;
    /** A return status code - returned if OK button has been pressed */
    public static final int RET_OK = 1;

    /** Creates new form PSDNewContextDialog */
    public PDSNewContextDialog(java.awt.Frame parent, boolean modal) {
        super(parent, modal);
        initComponents();
    }

    /** @return the return status of this dialog - one of RET_OK or RET_CANCEL */
    public int getReturnStatus() {
        return returnStatus;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    private void initComponents() {//GEN-BEGIN:initComponents
        jPanel5 = new javax.swing.JPanel();
        m_label1 = new javax.swing.JLabel();
        m_parentPath = new javax.swing.JLabel(PDSGUIController.getEntityList().getContextPath());
        buttonPanel = new javax.swing.JPanel();
        m_okButton = new javax.swing.JButton();
        m_cancelButton = new javax.swing.JButton();
        m_inputPanel = new javax.swing.JPanel();
        m_label = new javax.swing.JLabel();
        m_contextName = new javax.swing.JTextField();
        
        setModal(true);
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                closeDialog(evt);
            }
        });
        
        jPanel5.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));
        
        m_label1.setText("Path:       ");
        jPanel5.add(m_label1);
        
        jPanel5.add(m_parentPath);
        
        getContentPane().add(jPanel5, java.awt.BorderLayout.NORTH);
        
        buttonPanel.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.RIGHT));
        
        getRootPane().setDefaultButton(m_okButton);
        m_okButton.setText("OK");
        m_okButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                m_okButtonActionPerformed(evt);
            }
        });
        
        buttonPanel.add(m_okButton);
        
        m_cancelButton.setText("Cancel");
        m_cancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                m_cancelButtonActionPerformed(evt);
            }
        });
        
        buttonPanel.add(m_cancelButton);
        
        getContentPane().add(buttonPanel, java.awt.BorderLayout.SOUTH);
        
        m_inputPanel.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.LEFT));
        
        m_label.setText("Context Name:      ");
        m_inputPanel.add(m_label);
        
        m_contextName.setPreferredSize(new java.awt.Dimension(200, 27));
        m_contextName.setMinimumSize(new java.awt.Dimension(80, 27));
        m_inputPanel.add(m_contextName);
        
        getContentPane().add(m_inputPanel, java.awt.BorderLayout.CENTER);
        
        pack();
    }//GEN-END:initComponents

    private void m_okButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_m_okButtonActionPerformed
        doClose(RET_OK);
    }//GEN-LAST:event_m_okButtonActionPerformed

    private void m_cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_m_cancelButtonActionPerformed
        doClose(RET_CANCEL);
    }//GEN-LAST:event_m_cancelButtonActionPerformed

    /** Closes the dialog */
    private void closeDialog(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_closeDialog
        doClose(RET_CANCEL);
    }//GEN-LAST:event_closeDialog

    private void doClose(int retStatus) {
        returnStatus = retStatus;
        if (retStatus == RET_OK) {
            PDSGUIController.createNewContext(m_parentPath.getText(), m_contextName.getText());
        }
        setVisible(false);
        dispose();
    }

    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
        new PDSNewContextDialog(new javax.swing.JFrame(), true).show();
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel jPanel5;
    private javax.swing.JLabel m_label1;
    private javax.swing.JLabel m_parentPath;
    private javax.swing.JPanel buttonPanel;
    private javax.swing.JButton m_okButton;
    private javax.swing.JButton m_cancelButton;
    private javax.swing.JPanel m_inputPanel;
    private javax.swing.JLabel m_label;
    private javax.swing.JTextField m_contextName;
    // End of variables declaration//GEN-END:variables

    private int returnStatus = RET_CANCEL;
}
