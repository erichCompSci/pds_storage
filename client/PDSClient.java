package client;

/*
 * PDSClient.java
 *
 * Created on November 13, 2002, 4:13 PM
 */

/**
 *
 * @author  mansour
 */
public class PDSClient extends javax.swing.JDialog {
    /** A return status code - returned if Cancel button has been pressed */
    public static final int RET_CANCEL = 0;
    /** A return status code - returned if OK button has been pressed */
    public static final int RET_OK = 1;

    /** Creates new form PDSClient */
    public PDSClient(java.awt.Frame parent, boolean modal) {
        super(parent, modal);
        initComponents();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    private void initComponents() {//GEN-BEGIN:initComponents
        jPanel1 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        jLabel5 = new javax.swing.JLabel();
        jPanel4 = new javax.swing.JPanel();
        m_domainName = new javax.swing.JTextField();
        m_domainType = new javax.swing.JTextField();
        m_domainVersion = new javax.swing.JTextField();
        m_domainApplication = new javax.swing.JTextField();
        m_pathToBrowser = new javax.swing.JTextField();
        jPanel2 = new javax.swing.JPanel();
        m_okButton = new javax.swing.JButton();
        m_cancelButton = new javax.swing.JButton();

        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                closeDialog(evt);
            }
        });

        m_domainApplication.setColumns(30);
        m_domainType.setColumns(30);
        m_domainName.setColumns(30);
        m_domainVersion.setColumns(30);
        m_pathToBrowser.setColumns(30);
        jPanel1.setLayout(new java.awt.BorderLayout());

        jPanel3.setLayout(new java.awt.GridLayout(5, 0, 10, 0));

        jLabel1.setText("PDS Domain");
        jPanel3.add(jLabel1);

        jLabel2.setText("Type");
        jPanel3.add(jLabel2);

        jLabel3.setText("Version");
        jPanel3.add(jLabel3);

        jLabel4.setText("Application");
        jPanel3.add(jLabel4);

        jLabel5.setText("Browser");
        jPanel3.add(jLabel5);

        jPanel1.add(jPanel3, java.awt.BorderLayout.WEST);

        jPanel4.setLayout(new java.awt.GridLayout(5, 0, 5, 0));

        jPanel4.add(m_domainName);

        jPanel4.add(m_domainType);

        jPanel4.add(m_domainVersion);

        jPanel4.add(m_domainApplication);

        jPanel4.add(m_pathToBrowser);

        jPanel1.add(jPanel4, java.awt.BorderLayout.CENTER);

        getContentPane().add(jPanel1, java.awt.BorderLayout.CENTER);

        m_okButton.setText("Connect");
        m_okButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connect(evt);
            }
        });
        getRootPane().setDefaultButton(m_okButton);
        jPanel2.add(m_okButton);
        m_cancelButton.setText("Cancel");
        m_cancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                close(evt);
            }
        });

        jPanel2.add(m_cancelButton);
        getContentPane().add(jPanel2, java.awt.BorderLayout.SOUTH);

        pack();
    }//GEN-END:initComponents

    /** Closes the dialog */
    private void closeDialog(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_closeDialog
        setVisible(false);
        dispose();
    }//GEN-LAST:event_closeDialog

    private void close(java.awt.event.ActionEvent evt) {
        setVisible(false);
        dispose();
        System.exit(0);
    }

    private void connect(java.awt.event.ActionEvent evt) {
        String domain = m_domainName.getText();
        String type = m_domainType.getText();
        int version = 0;
        try {
            version = Integer.parseInt(m_domainVersion.getText());
        } catch (Exception e) {
            return;
        }
        String application = m_domainApplication.getText();
        PDSEntityPopupMenu.m_pathToBrowser = m_pathToBrowser.getText();
        PDSGUIController.init(domain, type, version, application);
        new PDSData().show();
        setVisible(false);
        dispose();
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        if (args.length < 2) {
            System.out.println("Usage: java PDSData <PDS host> <PDS port>");
            return;
        }

        client.pds.PDSWrapper.start_service(args[0], Integer.parseInt(args[1]));
        PDSClient client = new PDSClient(new javax.swing.JFrame(), true);
        if (args.length > 2)
            client.m_domainName.setText(args[2]);
        if (args.length > 3)
            client.m_domainType.setText(args[3]);
        if (args.length > 4)
            client.m_domainVersion.setText(args[4]);
        if (args.length > 5)
            client.m_domainApplication.setText(args[5]);

        client.m_pathToBrowser.setText(PDSEntityPopupMenu.m_pathToBrowser);

        client.show();

    }


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JTextField m_domainName;
    private javax.swing.JTextField m_domainType;
    private javax.swing.JTextField m_domainVersion;
    private javax.swing.JTextField m_domainApplication;
    private javax.swing.JTextField m_pathToBrowser;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JButton m_okButton;
    private javax.swing.JButton m_cancelButton;
    // End of variables declaration//GEN-END:variables

}
