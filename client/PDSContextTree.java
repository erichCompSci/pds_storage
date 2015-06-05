package client;

/*
 * FilePathsTree.java
 *
 * Created on October 7, 2001, 8:54 AM
 *
 * @author Mohamed S. Mansour
 * @version 1.0
 *
 * This class extends the basic Swing JTree
 *
 */
import java.util.Hashtable;

import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.DefaultMutableTreeNode;

import client.pds.PDSWrapper;

class ContextTreeNode extends DefaultMutableTreeNode {
	public String m_full_path;
	public ContextTreeNode(String context, String full_path) {
		super(context);
		m_full_path = full_path;
	}
}
public class PDSContextTree {

	DefaultTreeModel m_model;
	boolean m_init;
	/** Creates new FilePathsTree */
	public PDSContextTree() {
		m_model = null;
		m_init = false;
	}

	/** Creates new PDSGUIController */
	private void init() {
		if (m_init == true)
			return;
		m_init = true;
		ContextTreeNode root = new ContextTreeNode("/", "/");
		root.setUserObject("/");
		m_model = new DefaultTreeModel(root);
		refresh();
	}
	/**
	 * Refresh the PDS context tree from the server
	 */
	public void refresh() {

		// remove all nodes from the tree
		ContextTreeNode root = (ContextTreeNode) m_model.getRoot();
		root.removeAllChildren();
		String[] context_list =
			PDSWrapper.xpath_query(
				PDSGUIController.m_domain_id,
				"//PDSContext");
		// browse the PDS hierarchy and add all nodes recursively
		/*
		String[] context_list = client.pds.PDSWrapper.get_context_list(PDSGUIController.m_domain_id,
		"/");                
		*/
		Hashtable ctxt_table = new Hashtable();

		for (int i = 0; i < context_list.length; i++) {
			addNode(ctxt_table, context_list[i]);
		}
		m_model.reload();
	}
	/**
	 *
	 */
	public DefaultTreeModel getModel() {
		init();
		return m_model;
	}
	/** Adds a node to the tree
	 * adds the label to the root level
	 *@return the new node
	 */
	private void addNode(Hashtable ctxt_table, String label) {

		if ("/".equals(label)) {
			ContextTreeNode root = (ContextTreeNode) m_model.getRoot();
			ctxt_table.put(label, root);
			return;
		}
		String my_label = label.substring(label.lastIndexOf('/') + 1);
		String parent_label;
		if (label.lastIndexOf('/') == 0)
			parent_label = "/";
		else
			parent_label = label.substring(0, label.lastIndexOf('/'));

		ContextTreeNode parent = (ContextTreeNode) ctxt_table.get(parent_label);
		ContextTreeNode node = new ContextTreeNode(my_label, label);
		ctxt_table.put(label, node);

		m_model.insertNodeInto(node, parent, parent.getChildCount());
		//else {
		//ContextTreeNode root = (ContextTreeNode)m_model.getRoot();
		//m_model.insertNodeInto(node, root, root.getChildCount());
		//}        

	}
	/**
	 * Recurse and retrieve all child contexts
	 * @param parent
	 * @param path
	 */
	/*
	private void getChildContext(ContextTreeNode parent, String path) {
	    String[] context_list = client.pds.PDSWrapper.get_context_list(PDSGUIController.m_domain_id,
	    path);
	    for (int i=0; i<context_list.length; i++) {
	        ContextTreeNode node = addNode(parent, context_list[i]);
	        getChildContext(node, path+"/"+context_list[i]);
	    }
	}
	*/
}
