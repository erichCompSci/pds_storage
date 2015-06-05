#ifndef XML_DUMP_VISITOR_H_
#define XML_DUMP_VISITOR_H_

#include <stdio.h>


#include "TreeVisitor.h"

class Domain;
class Entity;
class Context;
class ContextBindable;

class XmlDumpVisitor : public AbstractPDSTreeVisitor
{
public:
  XmlDumpVisitor(FILE* fp);
  ~XmlDumpVisitor();

  void start_domain(Domain& d);
  void end_domain(Domain& d);
  
  void start_context(Domain& d, Context& c, const std::string& path);
  void end_context(Domain& d, Context& c, const std::string& path);
  
  void start_entity(Domain& d, Context& c, Entity& e, const std::string& path);
  void end_entity(Domain& d, Context& c, Entity& e, const std::string& path);
   

private:
  XmlDumpVisitor(const XmlDumpVisitor&);
  const XmlDumpVisitor& operator=(const XmlDumpVisitor&);

  void dump_attribute_list(FILE* fp, ContextBindable& e);
  FILE* fp_;

};

#endif // XML_DUMP_VISITOR_H_
