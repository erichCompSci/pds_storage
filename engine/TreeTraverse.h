#ifndef TREE_TRAVERSE_H
#define TREE_TRAVERSE_H


class Domain;
class Context;
class AbstractPDSTreeVisitor;

class PDSTreeTraverse
{

public:
  PDSTreeTraverse(AbstractPDSTreeVisitor& visitor);
  ~PDSTreeTraverse();

public:
  void traverseTree();

private:
  void visitEntities(Domain* d, Context* c, const std::string& path);
  void traverseDomain(Domain* d);


private:
  PDSTreeTraverse(const PDSTreeTraverse&);
  const PDSTreeTraverse& operator= (const PDSTreeTraverse&);

  AbstractPDSTreeVisitor& visitor_;  
};

#endif // TREE_TRAVERSE_H
