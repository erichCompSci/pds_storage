package client;

import java.awt.datatransfer.*;
import java.awt.*;

/*
 * PDSPopupMenu.java
 *
 * Created on November 11, 2002, 10:04 PM
 */

/**
 *
 * @author  mansour
 * @version
 */
public class PDSEntityValueSelection implements ClipboardOwner {
    /** Creates new PDSPopupMenu */
    public PDSEntityValueSelection() {      
    }
    
    public void toClipboard(String selection) {        
       SecurityManager sm = System.getSecurityManager();
        if (sm != null) {
            try {
                sm.checkSystemClipboardAccess();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        Toolkit tk = Toolkit.getDefaultToolkit();
        System.out.println(selection  +" "  + selection.length());
        StringSelection st = new StringSelection(selection);
        Clipboard cp = tk.getSystemSelection();
       if (cp == null)
            cp = tk.getSystemClipboard();
        cp.setContents(st, null);             
    }
    public void lostOwnership(Clipboard clip, Transferable tr) {
        System.out.println("Lost clipboard ownership!");
    }
}