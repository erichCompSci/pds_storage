/*
 *
 *  $Id: load.cc,v 1.35 2007-12-04 21:06:20 pmw Exp $
 *
 */

#include <stack>
#include "config.h"
extern "C"
{
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "atl.h"
#include "evpath.h"
#include "common/formats.h"
}


#include <sys/types.h>
#include <iostream>
#ifndef NO_XML
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#endif

#include "Entity.h"
#include "Domain.h"
#include "TreeTraverse.h"
#include "XmlDumpVisitor.h"

char* dump_repository_path_ = 0; /* path to dump repository */

extern "C" {
  extern int encode_base64(char *dest, char *source);
  extern int decode_base64(char *dest, char *source, size_t length);
}

#ifndef NO_XML
bool loadIt (xmlDocPtr);
Domain*  doDomainLoad (xmlDocPtr, xmlNodePtr);
Context* doContextLoad(Domain* domain, xmlDocPtr doc, xmlNodePtr contxtNode);
Entity*  doEntityLoad(Domain* domain, Context* context, xmlDocPtr doc, xmlNodePtr entityNode);
void processDomain(Domain* target_domain, xmlDocPtr doc, xmlNodePtr root);
void doAttributeLoad(ContextBindable* c, xmlDocPtr doc, xmlNodePtr cur);
#endif
void dump_content (FILE* fp);

//----------------------------------------------------------
int
dump_content_wrapper()
{
  FILE* f;
  char buf[100];

  if (0 == dump_repository_path_) {
    dump_repository_path_ = strdup("/tmp");
  }

  sprintf (buf, "%s/pdsd-%d.xml", dump_repository_path_, (int)getpid());
  f = fopen (buf, "w");
  if (f == NULL)
    {
      perror ("pdsd dump");
      return errno;
    }

  dump_content (f);
  fclose (f);
  return 0;
}

#ifndef NO_XML

bool loadIt (xmlDocPtr);
Domain*  doDomainLoad (xmlDocPtr, xmlNodePtr);
Context* doContextLoad(Domain* domain, xmlDocPtr doc, xmlNodePtr contxtNode);
Entity*  doEntityLoad(Domain* domain, xmlDocPtr doc, xmlNodePtr entityNode);
void processDomain(Domain* target_domain, xmlDocPtr doc, xmlNodePtr root);
void doAttributeLoad(ContextBindable* c, xmlDocPtr doc, xmlNodePtr cur);

//----------------------------------------------------------
int
bulk_load_from_URL (const char* url)
{
  xmlDocPtr doc;

  bool goodParse = true;
  bool goodLoad = false;
  
  xmlInitParser();
  doc = xmlParseFile (url);
  if (!doc)
    {
      cerr << "An error occured during parsing\n" << endl;
      goodParse = false;
    }

  // If the parse was successful, get the document data
  if (!goodParse)
    return false;

  goodLoad = loadIt (doc);

  return goodLoad;
}
//----------------------------------------------------------
const xmlChar PDSBulkLoadSetTag[] = "PDSBulkLoadSet";
//const xmlChar PDSBulkLoadTag[] = "PDSBulkLoad";
const xmlChar PDSDomainTag[] = "PDSDomain";
const xmlChar PDSContextTag[] = "PDSContext";
const xmlChar ContextNameTag[] = "full_name";
const xmlChar PDSEntityTag[] = "PDSEntity";
const xmlChar EntityNameTag[] = "full_name";
const xmlChar dataValueTag[] = "value";
const xmlChar dataTypeTag[] = "dataType";
const xmlChar dataLengthTag[] = "dataLength";
const xmlChar AttributeTag[] = "Attribute";
const xmlChar attrNameTag[] = "name";
const xmlChar attrIdTag[] = "attrID";
const xmlChar attrTypeTag[] = "attrType";
const xmlChar DomainNameTag[] = "DomainName";
const xmlChar DomainTypeTag[] = "DomainType";
const xmlChar DomainVersionTag[] = "DomainVersion";
const xmlChar ApplicationNameTag[] = "ApplicationName";
const xmlChar Attr_Int4Tag[] = "Attr_Int4";
const xmlChar Attr_Int8Tag[] = "Attr_Int8";
const xmlChar Attr_StringTag[] = "Attr_String";
const xmlChar Attr_OpaqueTag[] = "Attr_Opaque";
const xmlChar textTag[] = "text";
const xmlChar emptyString[] = "";
//--------------------------------------
// scan next list for first non-text node
//--------------------------------------
xmlNodePtr get_next_node(xmlNodePtr child)
{
  xmlNodePtr next = child;
  //while (next != 0 && xmlStrcmp(next->name, textTag) == 0) {
  while (next != 0 && 
         (next->type == XML_TEXT_NODE || next->type == XML_COMMENT_NODE)) {
    next = next->next;
  }
  return next;
}
//--------------------------------------
bool
loadIt (xmlDocPtr doc)
{      
  xmlNodePtr loadSetPtr = xmlDocGetRootElement (doc);
  if (xmlStrcmp (loadSetPtr->name, PDSBulkLoadSetTag) != 0)
    {
      cerr << "Load set element of wrong type (not PDSBulkLoadSet)" << endl;
      xmlFreeDoc (doc);
      return false;
    }

  /*
  xmlNodePtr loadSetMemberPtr = get_next_node(loadSetPtr->xmlChildrenNode);  
  while (loadSetMemberPtr != NULL) { // <PDSBulkLoad>
    if (xmlStrcmp (loadSetMemberPtr->name, PDSBulkLoadTag) != 0) {
      cerr << "Load element of wrong type (not PDSBulkLoad)" << endl;
      xmlFreeDoc (doc);
      return false;
    }    
  */
  xmlNodePtr domain = get_next_node(loadSetPtr->xmlChildrenNode);
  while (domain != NULL) { // <PDSDomain>
    if (xmlStrcmp (domain->name, PDSDomainTag) != 0) {
      cerr << "Load element of wrong type (not PDSDomain)" << endl;
      xmlFreeDoc (doc);
      return false;
    }    

    Domain* target_domain = doDomainLoad (doc, domain);
    
    // start loading domain entries 
    xmlNodePtr root = get_next_node(domain->xmlChildrenNode);                  
    processDomain(target_domain, doc, root);
    domain = get_next_node(domain->next);
  } // while, <PDSDomain>
  
  //loadSetMemberPtr = get_next_node(loadSetMemberPtr->next);  
  //} // <PDSBulkLoad>
  
  xmlFreeDoc (doc);
  return true;
}
//----------------------------------------------------------
Context*
doContextLoad(Domain* target_domain, xmlDocPtr doc, xmlNodePtr contextNode)
{
  xmlChar* contextName = xmlGetProp(contextNode, ContextNameTag);
  Context* new_context = target_domain->resolve_or_create_context (reinterpret_cast<const char*> (contextName));
  xmlFree(contextName);
  return new_context;
}
//----------------------------------------------------------
void processDomain(Domain* target_domain, xmlDocPtr doc, xmlNodePtr root)
{
  std::stack<xmlNodePtr> context_stack;
  context_stack.push(root);

  while(context_stack.size() > 0) {
    xmlNodePtr c = context_stack.top();
    context_stack.pop();

    Context* ctx = doContextLoad(target_domain, doc, c);


    xmlNodePtr child = get_next_node(c->xmlChildrenNode);
    while (child != 0) {
      if (xmlStrcmp (child->name, PDSEntityTag) == 0) {
          Entity* new_entity = doEntityLoad(target_domain, ctx, doc, child);
      } else if (xmlStrcmp (child->name, PDSContextTag) == 0) {
          context_stack.push(child);
      }
      child = get_next_node(child->next);      
    } // while, parse Domain 
  }
}
//----------------------------------------------------------
Entity*
doEntityLoad(Domain* target_domain, Context* target_context, 
             xmlDocPtr doc, xmlNodePtr entityNode)
{    
  xmlChar* entityName = xmlGetProp(entityNode, EntityNameTag);
  xmlChar* attrType   = xmlGetProp(entityNode, dataTypeTag);    
  
  
  
  // now that we have the key-name, we can get an entity pointer from the
  // Domain
  Entity* new_entity = target_domain->resolve_or_create_entity((char*)entityName);

  // Now we can just loop for entity data and attributes
  
  xmlChar* attrValue = xmlGetProp(entityNode, dataValueTag);  
  xmlChar* attrLength = 0;

  attr_value_type type = Attr_Undefined;
  if (xmlStrcmp (attrType, Attr_Int4Tag) == 0) {
    type = Attr_Int4;
  } else if (xmlStrcmp (attrType, Attr_Int8Tag) == 0) {
    type = Attr_Int8;    
  } else if (xmlStrcmp (attrType, Attr_StringTag) == 0) {
    type = Attr_String;    
  } else if (xmlStrcmp (attrType, Attr_OpaqueTag) == 0) {
    type = Attr_Opaque;
    attrLength = xmlGetProp (entityNode, dataLengthTag);  
  }
  xmlNodePtr next_node = entityNode->xmlChildrenNode;
  if (type == Attr_Opaque) {
    xmlFree (attrValue);  
    xmlNodePtr value_node = get_next_node(next_node);
    next_node = value_node->next;
    attrValue = xmlNodeListGetString(doc, value_node->xmlChildrenNode, 1); 
    int encoded_len = (int)(1.4 * atoi((char*)attrLength));
    char* temp = (char*) malloc(atoi((char*)attrLength) + 1);
    int length = decode_base64(temp, (char*)attrValue, encoded_len);
    new_entity->set_data(temp, length, type);
    free(temp);
    xmlFree(attrLength);
  }
  else
    new_entity->set_data(attrValue, xmlStrlen (attrValue), type);
  
  xmlFree(entityName);
  xmlFree (attrValue);  
  xmlFree (attrType);


  // process attributes
  xmlNodePtr child = get_next_node(next_node);
  doAttributeLoad(new_entity, doc, child);

  return new_entity;
}
//----------------------------------------------------------
void doAttributeLoad(ContextBindable* c, xmlDocPtr doc, xmlNodePtr cur)
{
  //cur = get_next_node(cur->next);  
  while (cur != NULL) {
    
    if (xmlStrcmp (cur->name, AttributeTag) != 0)
      continue;

    xmlChar* attrName = xmlGetProp (cur, attrNameTag);
    xmlChar* attrID   = xmlGetProp (cur, attrIdTag);
    xmlChar* attrType = xmlGetProp (cur, attrTypeTag);
    xmlChar* attrValue = xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);

    char* cAttrValue = (char*) attrValue;
    void* val = 0;
    attr_value_type real_attr_type;

    if (xmlStrcmp (attrType, Attr_Int4Tag) == 0) {
      val = reinterpret_cast<void*> (atoi (cAttrValue));
      real_attr_type = Attr_Int4;
    } else if (xmlStrcmp (attrType, Attr_Int8Tag) == 0) {
      val = reinterpret_cast<void*> (atoi (cAttrValue));
      real_attr_type = Attr_Int8;
    } else if (xmlStrcmp (attrType, Attr_StringTag) == 0) {
      val = strdup (cAttrValue);
      real_attr_type = Attr_String;
    }

    if (xmlStrcmp(attrName, emptyString) != 0)
      add_attr (c->attributes_, 
              attr_atom_from_string (reinterpret_cast<const char*> (attrName)),
              real_attr_type,
              val);
    else
      set_attr (c->attributes_, 
              atoi((char*)attrID),
              real_attr_type,
              val);
    xmlFree (attrName);
    xmlFree (attrID);
    xmlFree (attrType);
    xmlFree (attrValue);  
    cur = get_next_node(cur->next);  
  }
}
//----------------------------------------------------------
Domain*
doDomainLoad (xmlDocPtr doc, xmlNodePtr domainNode)
{
  xmlChar* domainName = xmlGetProp(domainNode, DomainNameTag);
  xmlChar* domainType = xmlGetProp(domainNode, DomainTypeTag);
  xmlChar* appName = xmlGetProp(domainNode, ApplicationNameTag);
  xmlChar* domainVersion = xmlGetProp(domainNode, DomainVersionTag);

  Domain* target = Domain::find_first_matching_domain ((char*)domainName, (char*)domainType, 
                                               atoi((char*)domainVersion), (char*)appName);

  if (target == NULL)
    target = new Domain ((char*)domainName,
                         (char*)domainType,
                         atoi((char*)domainVersion),
                         (char*)appName);

  xmlFree (domainName);
  xmlFree (domainType);
  xmlFree (appName);
  xmlFree (domainVersion);
    

  return target;
}                                    

void
dump_content (FILE* fp)
{    
  XmlDumpVisitor visitor (fp);
  PDSTreeTraverse traverse(visitor);
  traverse.traverseTree();
}

static
xmlDocPtr getdoc (char *docname)
{
  xmlDocPtr doc;
  doc = xmlParseFile(docname);
  if (doc == 0) {
     fprintf(stderr,"Document not parsed successfully. \n");
     return 0;
  }
  return doc;
}
static
xmlXPathObjectPtr getnodeset (xmlDocPtr doc, const char *xpath)
{
  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  xmlXPathObjectPtr result = xmlXPathEvalExpression((const xmlChar*)xpath, context);
  if(xmlXPathNodeSetIsEmpty(result->nodesetval)) {
    return 0;
  }
  xmlXPathFreeContext(context);
  return result;
}
static xmlDocPtr _xml_doc;
void reset_xml_doc()
{
  xmlFreeDoc(_xml_doc);
  _xml_doc = 0;
}
void cache_xml_doc()
{
  if (_xml_doc != 0) return;

  dump_content_wrapper();
  if (0 == dump_repository_path_) {
    fprintf(stderr, "-p not specified on command line\n");
    return;
  }

  char buf[100];
  sprintf (buf, "%s/pdsd-%d.xml", dump_repository_path_, (int)getpid());
 
  _xml_doc = getdoc(buf);
}
void xpath_query(const char* xpath, pds_domain_id_t domain_id, xpath_query_results_msg_ptr output)
{
  cache_xml_doc();

  char buf[100];
  Domain* d = objectId::get_domain_ptr_from_id(domain_id);
  string modified_xpath = "//PDSDomain[@DomainName=\"";
  modified_xpath += d->domain_name_;
  modified_xpath += "\" and @DomainType=\"";
  modified_xpath += d->domain_type_;
  modified_xpath += "\" and @DomainVersion=\"";
  sprintf(buf, "%d", d->domain_version_);
  modified_xpath += buf;
  modified_xpath += "\" and @ApplicationName=\""; 
  modified_xpath += d->application_name_;
  modified_xpath += "\"]";
  if (xpath != 0 && xpath[0] != '/')
    modified_xpath += "//";
  modified_xpath += xpath;
  xmlXPathObjectPtr result = getnodeset (_xml_doc, modified_xpath.c_str());
  if (result) {
    xmlNodeSetPtr nodeset = result->nodesetval;
    output->name_count = nodeset->nodeNr;
    output->name_list = (char**) malloc (output->name_count * sizeof (char*));
    for (int i=0; i < nodeset->nodeNr; i++) {
      xmlChar* name = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"full_name");
      output->name_list[i] = strdup((char*)name);
      xmlFree(name);
    }
  }
  xmlXPathFreeObject (result);
  xmlCleanupParser();
}

#else
int bulk_load_from_URL (const char* url) 
{
  fprintf(stderr, "No XML in this build of PDS-engine, sorry.\n");
  return 0;
}

void
dump_content (FILE* fp)
{
  fprintf(fp, "No XML in thisbuild of PDS-engine, sorry\n");
}
void xpath_query(const char* xpath, pds_domain_id_t domain_id, xpath_query_results_msg_ptr output)
{
  fprintf(stderr, "No XML in thisbuild of PDS-engine, sorry\n");
  output->name_list = 0;
  output->name_count = 0;
}
#endif                     

//-------------------------------------------------

/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.34.2.1  2007/01/05 22:40:35  pmw
 *  interim commit for evpath changes, new branch
 *
 *  Revision 1.34  2006/10/11 02:01:26  chaos
 *  Change so we don't have include of evpath.h in cmrpc.h
 *
 *  Revision 1.33  2005/02/22 19:25:32  pmw
 *  modifications to keep Entity's destructor from hamstringing Domain's
 *  destructor.  the entity list iterator in ~Domain() was being whacked
 *  by ~Entity().  If this functionality is needed, it needs to be
 *  rethought.
 *
 *  Revision 1.32  2004/11/05 22:02:55  mansour
 *  Improve PDS performance by using R/W lock instead of a mutex and also cache the XML document instead of re-creating it on each query.
 *
 *  Revision 1.31  2004/10/14 22:05:34  mansour
 *  Create a DTD schema for PDS
 *
 *  Revision 1.30  2003/11/09 04:11:24  mansour
 *  Set the default path for file dump to /tmp
 *
 *  Revision 1.29  2003/11/09 03:57:06  mansour
 *  Add wrapper API for building XPath queries
 *
 *  Revision 1.28  2003/11/03 18:12:41  mansour
 *  add a // qualifier to xpath queries if needed
 *
 *  Revision 1.27  2003/10/31 14:48:36  mansour
 *  added a body for xpath_query when compiling with NO_XML
 *
 *  Revision 1.26  2003/10/31 13:22:55  chaos
 *  make this compile without XML
 *
 *  Revision 1.25  2003/10/31 05:39:36  mansour
 *  Added support for xpath queries
 *
 *  Revision 1.24  2003/10/21 19:54:13  eisen
 *  Various tweaks and hacks designed to get this to compile cleanly with
 *  Forte 7.
 *
 *  Revision 1.23  2003/10/09 15:58:42  chaos
 *  Fix this again so it compiles without XML
 *
 *  Revision 1.22  2003/10/07 17:20:02  pmw
 *  fixed references to dump_server_content_wrapper
 *  re-insert errno.h inclusion
 *
 *  Revision 1.21  2003/10/07 15:13:44  chaos
 *  Tweak non-cm handler to answer changes in CM.  Make load.cc compile where we don't have XML and need a header to reference errno.
 *
 *  Revision 1.20  2003/10/06 14:08:06  mansour
 *  Move dump_server_content_wrapper to libengine
 *
 *  Revision 1.19  2003/10/05 11:41:23  pmw
 *  fix deprecated iostream header inclusion
 *
 *  Revision 1.18  2003/10/02 16:28:32  mansour
 *  irix-n32 compiler doesn't requires a special compile flag to recognize new style STL headers (e.g. <iostream>)
 *
 *  Revision 1.17  2003/10/02 16:02:06  mansour
 *  remove return from a void function
 *
 *  Revision 1.16  2003/10/02 14:44:14  eisen
 *  remove warnings, convert files from DOS mode
 *
 *  Revision 1.15  2003/10/02 03:32:54  eisen
 *  Move code in load.c inside the NO_XML #ifndef;  Fix DOS-mode line endings
 *
 *  Revision 1.14  2003/09/30 22:21:47  mansour
 *  dump and load opaque entities
 *
 *  Revision 1.13  2003/09/30 19:02:04  mansour
 *  Change the format of the XML dump document to match the PDS tree hierarchy
 *
 *  Revision 1.12  2003/09/29 17:31:01  mansour
 *  move the variable definition to pdsd.cc, this fixes a link problem on marquesas
 *
 *  Revision 1.11  2003/09/29 17:21:50  eisen
 *  Add return statement.
 *
 *  Revision 1.10  2003/09/29 00:57:25  mansour
 *  Added a checkpointing thread.
 *  Fix the code for XML dump and load
 *  Fix the code for get_full_name
 *
 *  Revision 1.9  2003/03/25 14:27:32  pmw
 *  load.cc now compiles with libxml code.  hasn't been debugged yet, I'll
 *  wait until somebody cares.
 *
 *  Revision 1.8  2003/03/25 00:49:12  pmw
 *  Re-did XML parsing to use libxml2 instead of Xerces.  doesn't build
 *  yet until I fix the configuration process to autodetect libxml.
 *
 *  Revision 1.7  2003/01/27 06:56:22  eisen
 *  Ugly hack so we can compile without Xerces.  Simply punt on loading and
 *  dumping in engine/load.cc.  I.E. we simply print out a message saying we
 *  can't do it.  (The only other option I could think of that was simple was
 *  not building the server at all.  This seemed slightly better.)  pds now
 *  compiles and builds on Irix6 in the n32 ABI (with the -LANG:std option).
 *
 *  Revision 1.6  2003/01/24 19:31:28  eisen
 *  Some tweaks to make pds compile with Forte CC (version 7 with the
 *  -library=stlport4 argument).
 *
 *  Revision 1.5  2002/08/07 18:51:45  eisen
 *  Widespread changes to modify how attribute handling works.  Stop using
 *  attr_atom_from_string() to get atom values.  Instead, those are set by the
 *  application.  Stop using get_attr() (api has changes).  Stop using
 *  xmit_object and start using the ATL-supplied encode/decode routines.
 *
 *  Revision 1.4  2002/08/05 22:49:25  pmw
 *  Changes to support gcc 3.0 and xerces-c 2.0
 *
 *  Revision 1.3  2002/03/28 04:44:31  pmw
 *  changed tag names for bulk load files from WPxxxx to PDSxxxx
 *
 *  Revision 1.2  2002/03/08 21:05:32  pmw
 *  Server can now dump its contents to an XML stream
 *
 *  Revision 1.1  2001/10/09 21:37:55  pmw
 *  configuration changes
 *
 *  Revision 1.2  2001/08/27 08:05:53  pmw
 *  fixes
 *
 *  Revision 1.1.1.1  2000/12/13 22:14:22  pmw
 *  imported sources
 *
 *  Revision 1.6  2000/12/06 01:57:01  pmw
 *  Fixed problems with memory leakage and poor server shutdown mechanics
 *
 *  Revision 1.5  2000/11/30 23:33:48  pmw
 *  fixed schizophrenia regarding management of strings stored as entity data.
 *  strings should! be always stored WITHOUT the terminating NULL character.
 *  wp_lookup_string allocates space for and tacks on this character when it
 *  is called.  If you're calling get_entity_data directly, well, ya better
 *  figure it out yourself.
 *
 *  Revision 1.4  2000/11/27 22:07:59  pmw
 *  uninitialized pointers causing segfaults on Linux when delete'd
 *
 *  Revision 1.3  2000/11/21 00:23:51  pmw
 *  Since I didn't have enough to do, a little code reorganization to keep
 *  things from getting dramatically out of hand.  Fixed a couple of
 *  bugs-in-waiting.
 *
 *  Revision 1.2  2000/11/19 23:28:29  pmw
 *  wp_server now supports bulk load of data with -f command-line option.  Also added
 *  command message so bulk loads can happen on-line.
 *
 *  Revision 1.1  2000/10/25 21:44:32  pmw
 *  Compiles after major revisions to support multiple WP services on
 *  client-side.  This should eventually allow a client to use services by
 *  linking in the library directly or by communicating using CMRPC.
 *
 *
 */
