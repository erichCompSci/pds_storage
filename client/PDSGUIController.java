package client;

import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Document;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import java.io.File;
import java.util.StringTokenizer;

/*
 * PDSGUIController.java
 *
 * Created on October 31, 2002, 6:47 PM
 */

/**
 *
 * @author  mansour
 * @version
 */
public class PDSGUIController {
	/** the PDS domain ID
	 */
	static client.pds.PDSDomainID m_domain_id;
	/**
	 * The data model for the context tree
	 */
	static private PDSContextTree m_context_model;
	/**
	 * The data model for the entity list
	 */
	static private PDSEntityList m_entity_model;
	/**
	 * The data model for the selected path widget
	 * on top of the entity list
	 */
	static private DefaultStyledDocument m_context_path_doc;
	/**
	 * The currently selected entity
	 * "" if no entities or no selected entities
	 */
	static private String m_selected_entity;
	/**
	 * Popmenu over the entity table
	 */
	static public PDSEntityPopupMenu m_menu;
	/**
	 * PDS domain information
	 */
	static private String m_domain;
	static private String m_type;
	static private String m_application;
	static private int m_version;
	static private String m_domainString;
	static private PDSPollThread m_poll_thread;
	/**
	 * Wrapper around system clipboard
	 */
	static public PDSEntityValueSelection m_systemClipboard;

	static private boolean m_init = false;

	/**
	 * Refresh the PDS data from the server
	 */
	static public void refresh() {

		String selected_context = getSelectedContextPath();
		m_context_model.refresh();
		// refresh model from server...looses current selection
		ContextTreeNode last = findLastValidContext(selected_context);
		setSelectedContextPath(last.m_full_path);
		PDSData.m_contextTree.setSelectionPath(new TreePath(last.getPath()));
		m_entity_model.setContextPath(getSelectedContextPath());

	}

	/**
	 * Find the valid portion of the path
	 * supplied in selected_context and select
	 * it on the tree
	 * in the context tree
	 * @param selected_context a fully qualified context name
	 * @return the portion of selected context that is still valid in the tree
	 */
	private static ContextTreeNode findLastValidContext(String selected_context) {
		String path = new String();
		StringTokenizer tokenizer = new StringTokenizer(selected_context, "/");
		ContextTreeNode node =
			(ContextTreeNode) m_context_model.getModel().getRoot();
		while (tokenizer.hasMoreElements()) {
			path += "/" + tokenizer.nextToken();
			ContextTreeNode child = getChildNode(node, path);
			if (child == null) {
				break;
			}
			node = child;
		}
		return node;
	}

	private static ContextTreeNode getChildNode(
		ContextTreeNode node,
		String token) {
		for (int i = 0; i < node.getChildCount(); i++) {
			ContextTreeNode child = (ContextTreeNode) node.getChildAt(i);
			if (token.equals(child.m_full_path)) {
				return child;
			}
		}
		return null;
	}

	/**
	 * Initialize connection to PDS server and initialize all
	 * variables of this class
	 */
	static public void init(
		String domain,
		String type,
		int version,
		String application) {
		if (m_init == true)
			throw new RuntimeException();

		m_domain = domain;
		m_type = type;
		m_version = version;
		m_application = application;

		m_domainString =
			new String(
				m_domain
					+ ":"
					+ m_type
					+ ":"
					+ Integer.toString(m_version)
					+ ":"
					+ m_application);
		m_domain_id =
			client.pds.PDSWrapper.open_domain(
				m_domain,
				m_type,
				version,
				m_application);
		// register for domain changes

		client.pds.PDSWrapper.register_for_domain_change(m_domain_id, null);
		m_poll_thread = new PDSPollThread("cm poll thread");
		m_poll_thread.start();

		m_menu = new PDSEntityPopupMenu();
		m_selected_entity = new String();
		m_systemClipboard = new PDSEntityValueSelection();
	}

	static public String getPDSDomainString() {

		return m_domainString;
	}

	static public void setSelectedEntity(int row) {
		m_selected_entity = "";
		if (row < 0)
			return;
		m_selected_entity =
			(String) getEntityList().getEntityTableModel().getValueAt(row, 0);
	}

	static public String getSelectedEntity() {
		return m_selected_entity;
	}

	static public void setSelectedContextPath(String path) {
		// reset the entity selection
		PDSGUIController.setSelectedEntity(-1);
		m_entity_model.setContextPath(path);

		// changed the displayed path
		Document doc = PDSGUIController.getContextPathModel();
		try {
			doc.remove(0, doc.getLength());
			doc.insertString(0, path, null);
		} catch (BadLocationException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Return the selected context path
	 * @return the selected context path or "" if no selection
	 */
	static public String getSelectedContextPath() {
		try {
			return getContextPathModel().getText(
				0,
				getContextPathModel().getLength());
		} catch (Exception ex) {
			ex.printStackTrace();
			return "";
		}
	}

	//** ACCESSOR MOETHODS FOR THE MODELs MAINTAINED BY THIS CLASS **//
	static public Document getContextPathModel() {
		if (m_context_path_doc == null) {
			m_context_path_doc = new DefaultStyledDocument();
			try {
				m_context_path_doc.insertString(0, "/", null);
			} catch (Exception e) {
			}

		}
		return m_context_path_doc;
	}

	static public TreeModel getContextTreeModel() {
		if (m_context_model == null)
			m_context_model = new PDSContextTree();
		return m_context_model.getModel();
	}

	static public PDSEntityList getEntityList() {
		if (m_entity_model == null) {
			m_entity_model = new PDSEntityList("/");
		}
		return m_entity_model;
	}

	/**
	 * Create a new PDS context
	 * @param context_path the path to the parent of the new context
	 * @param newContextName the name of the new context
	 */
	static void createNewContext(String context_path, String newContextName) {
		String full_context_name = new String();
		if ("/".equals(context_path))
			full_context_name = "/" + newContextName;
		else
			full_context_name = context_path + "/" + newContextName;

		client.pds.PDSWrapper.create_context(m_domain_id, full_context_name);
		//refresh();
	}

	/**
	 * Remove a PDS context
	 * @param context_name the full qualified context name
	 * Root context cannot be removed
	 */
	static void removeContext(String context_name) {

		if ("/".equals(context_name))
			return;

		String selected_context = getSelectedContextPath();
		client.pds.PDSWrapper.delete_context(m_domain_id, context_name);
		m_context_model.refresh();
		setSelectedContextPath(selected_context);
		m_entity_model.setContextPath(getSelectedContextPath());

	}

	/**
	 * Create a new entity
	 * @param context_path The entity's context
	 * @param newEntityName The entity name
	 * @param entityType  The entity type ()
	 * @param entityValue The entity's value
	 */
	static void createNewEntity(
		String context_path,
		String newEntityName,
		String entityType,
		String entityValue) {
		String full_entity_name = new String();
		if ("/".equals(context_path))
			full_entity_name = "/" + newEntityName;
		else
			full_entity_name = context_path + "/" + newEntityName;

		client.pds.PDSWrapper.create_entity(m_domain_id, full_entity_name);
		client.pds.PDSWrapper.set_entity_value(
			m_domain_id,
			full_entity_name,
			entityType,
			entityValue);
		if (PDSEntityValueTypes.OPAQUE_TYPE_STR.equals(entityType)) {
			File file = new File(entityValue);

			client.pds.PDSWrapper.set_entity_attr_value(
				m_domain_id,
				full_entity_name,
				PDSEntityTableModel.FILE_NAME_ATTR_ID,
				PDSEntityValueTypes.STRING_TYPE_STR,
				file.getName());
		}
		m_entity_model.setContextPath(context_path);
	}

	/**
	 * Remove an entity
	 * @param context_path The path to the entity's context
	 * @param entityName   The entity name
	 */
	static void removeEntity(String context_path, String entityName) {
		client.pds.PDSWrapper.delete_entity(
			m_domain_id,
			context_path + "/" + entityName);
		m_entity_model.setContextPath(context_path);
	}
}
