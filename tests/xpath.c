#include <libxml/parser.h> 
#include <libxml/xpath.h> 
xmlDocPtr getdoc (char *docname) 
{
  xmlDocPtr doc; 
  doc = xmlParseFile(docname); 
  if (doc == NULL ) { 
     fprintf(stderr,"Document not parsed successfully. \n"); 
     return NULL; 
  } 
  return doc; 
} 

xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath)
{ 
  xmlXPathContextPtr context; 
  xmlXPathObjectPtr result; 
  context = xmlXPathNewContext(doc); 
  result = xmlXPathEvalExpression(xpath, context); 
  if(xmlXPathNodeSetIsEmpty(result->nodesetval)) { 
    printf("No result\n");
    return NULL; 
  } 
  xmlXPathFreeContext(context); 
  return result; 
}
int
main(int argc, char* argv[]) 
{
  char *docname; 
  xmlDocPtr doc; 
  //xmlChar *xpath1 = "//PDSContext[PDSEntity[@name=\"e1\"]=\"1\" or PDSEntity[@name=\"e2\" and @dataType=\"Attr_Opaque\"]]";
  xmlChar* xpath1 = "//PDSDomain[@DomainName=\"1\" and @DomainType=\"1\" and @DomainVersion=\"1\" and @ApplicationName=\"1\"]//PDSContext";
  //xmlChar *xpath1 = "//PDSDomain[@DomainName=\"1\"]//PDSContext";
  xmlChar *xpath2 = "//PDSContext[PDSEntity[(@name=\"e1\" and @value=\"2\") or (@name=\"e1\" and @value=\"1\")]]";
 
  if (argc <= 1) { 
    printf("Usage: %s docname\n", argv[0]); 
    return(0); 
  } 
  docname = argv[1]; 
  doc = getdoc(docname);
  printf("%s\n", xpath1);
  check(doc, xpath1);
  printf("%s\n", xpath2);
  check(doc, xpath2);
  xmlFreeDoc(doc); 
  xmlCleanupParser(); 
  return (1); 
}

check(xmlDocPtr doc, xmlChar* xpath)
{
  xmlNodeSetPtr nodeset; 
  xmlXPathObjectPtr result; 
  int i; 
  result = getnodeset (doc, xpath);
  if (result) { 
    nodeset = result->nodesetval; 
    for (i=0; i < nodeset->nodeNr; i++) { 
      xmlChar* name = xmlGetProp(nodeset->nodeTab[i], (xmlChar*)"full_name");
      printf("name: %s\n", name);
    } 
  } 
  xmlXPathFreeObject (result); 
}

