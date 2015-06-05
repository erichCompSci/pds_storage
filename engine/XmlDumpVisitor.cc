extern "C"
{
#include "atl.h"
}
#include "XmlDumpVisitor.h"
#include "Entity.h"
#include "Domain.h"
#include "Context.h"
#include <strings.h>

extern "C" {
  extern int encode_base64(char *dest, char *source);
  extern int decode_base64(char *dest, char *source, size_t length);
}


const char* domain_dump_str = "<PDSDomain\n"
                              "    DomainName=\"%s\" \n"
                              "    DomainType=\"%s\" \n"
                              "    DomainVersion=\"%d\" \n"
                              "    ApplicationName=\"%s\">\n";

/******************************************************
*******************************************************/
XmlDumpVisitor::XmlDumpVisitor(FILE* fp)
  :fp_(fp)
{
  fprintf (fp_, "<PDSBulkLoadSet>\n");
}
/******************************************************
*******************************************************/
XmlDumpVisitor::~XmlDumpVisitor()
{
  fprintf (fp_, "</PDSBulkLoadSet>");
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::start_domain(Domain& d)
{
  const char *dname = (d.domain_name_ ? d.domain_name_ : "");      	  
  const char* dtype = (d.domain_type_ ? d.domain_type_ : "");
  const char* appname = (d.application_name_ ? d.application_name_ : "");

  fprintf (fp_, domain_dump_str, dname, dtype, d.domain_version_, appname);
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::end_domain(Domain& d)
{
  fprintf(fp_, "</PDSDomain>\n");
}
static
string get_full_name(const string&path, ContextBindable& c)
{
  string full_name;
  if (strcmp(c.get_name(), "/") == 0)
    full_name = "/";
  else {
    full_name = path + "/";
    full_name += c.get_name();
  }
  return full_name;
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::start_context(Domain& d, Context& c, const std::string& full_name)
{
  fprintf (fp_, "<PDSContext full_name=\"%s\" name=\"%s\">\n", 
                  full_name.c_str(),
                  c.get_name());      
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::end_context(Domain& d, Context& c, const std::string& full_name)
{
  fprintf (fp_, "</PDSContext> <!-- %s -->\n", full_name.c_str());
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::start_entity(Domain& d, Context& c, Entity& e, const std::string& path)
{
  string full_name = get_full_name(path, e);

  fprintf (fp_, "<PDSEntity full_name=\"%s\" name=\"%s\" dataType=", 
                         full_name.c_str(),
                         e.get_name());    

//  fprintf (fp_, "<EntityData ");

  switch (e.data_type_)
    {
    case Attr_Undefined:
      fprintf (fp_, "\"Attr_Undefined\"");
      break;
    case Attr_Int4:
      fprintf (fp_, "\"Attr_Int4\"");
      break;
    case Attr_Int8:
      fprintf (fp_, "\"Attr_Int8\"");
      break;
    case Attr_String:                          
      fprintf (fp_, "\"Attr_String\"");
      break;
    case Attr_Opaque:      
      fprintf (fp_, "\"Attr_Opaque\" dataLength=\"%ld\"", e.data_size_);      
      break;
    }  
  if (e.data_type_ != Attr_Opaque) {
    fprintf (fp_, " value=\"%s\" ", e.data_);
  }
  else {
    fprintf (fp_, " value=\"encoded\" ");
  }
  fprintf (fp_, ">");
  if (e.data_type_ == Attr_Opaque) {
    char*temp = (char*) malloc((int)(e.data_size_*1.4));
    int len = encode_base64(temp, (char*)e.data_);
    fprintf (fp_, "<Value>");
    fwrite(temp, 1, len, fp_);        
    fprintf (fp_, "</Value>");
    free(temp);
  }
/*
  else
    fprintf (fp_, "%s", e.data_);
*/  
  //fprintf (fp_, "</PDSEntity> <!-- entity %s-->\n", e.get_full_name().c_str());

  dump_attribute_list(fp_, e);
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::end_entity(Domain& d, Context& c, Entity& e, const std::string& path)
{
  string full_name = get_full_name(path, e);

  fprintf (fp_, "</PDSEntity> <!-- %s -->\n", full_name.c_str());
}
/******************************************************
*******************************************************/
void 
XmlDumpVisitor::dump_attribute_list(FILE* fp, ContextBindable& e)
{
    int ac = attr_count (e.attributes_);
    for (int j = 0; j < ac; j++)
    {
      atom_t attr_id;
      attr_value_type attr_type;
      attr_value attr_value;                  	  
	    get_attr(e.attributes_, j, &attr_id, &attr_type, &attr_value);
      char* name = attr_string_from_atom (attr_id);
      if (name == 0) name = strdup("");
      fprintf (fp_, "\n<Attribute name=\"%s\" attrID=\"%d\" attrType=", name, attr_id);
      
      free(name);
      switch (attr_type)
        {
        case Attr_Undefined:
          fprintf (fp_, "\"Attr_Undefined\">%s",
                   (char*)attr_value);
          break;
        case Attr_Int4:
          fprintf (fp_, "\"Attr_Int4\">%d",
                   (int)(long)attr_value);
          break;
        case Attr_Int8:
          fprintf (fp_, "\"Attr_Int8\">%d",
                   (int)(long)attr_value);
          break;
        case Attr_String:
          fprintf (fp_, "\"Attr_String\">%s",
                   (char*)attr_value);
          break;
        case Attr_Opaque:
          fprintf (fp_, "\"Attr_Opaque\">");
          break;
        }
      fprintf (fp, "</Attribute>\n");
    }

}
