#ifndef TREE_VISITOR_H_
#define TREE_VISITOR_H_

#include <string>

class Domain;
class Entity;
class Context;

class AbstractPDSTreeVisitor
{
public:
  AbstractPDSTreeVisitor() {}
  virtual ~AbstractPDSTreeVisitor() {}

  virtual void start_domain(Domain& d) = 0;
  virtual void end_domain(Domain& d) = 0;
  
  virtual void start_context(Domain& d, Context& c, const std::string& path) = 0;
  virtual void end_context(Domain& d, Context& c, const std::string& path) = 0;
  
  virtual void start_entity(Domain& d, Context& c, Entity& e, const std::string& path) = 0;
  virtual void end_entity(Domain& d, Context& c, Entity& e, const std::string& path) = 0;
   

private:
  AbstractPDSTreeVisitor(const AbstractPDSTreeVisitor&);
  const AbstractPDSTreeVisitor& operator=(const AbstractPDSTreeVisitor&);
};

#endif // TREE_VISITOR_H_

