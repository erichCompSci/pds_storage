#ifndef _ENGINE_GUARD_H_
#define _ENGINE_GUARD_H_



class ReaderGuard
{
public:
  ReaderGuard();
  ~ReaderGuard();

  static void initialize();
private:
  ReaderGuard(const ReaderGuard&);
  const ReaderGuard& operator= (const ReaderGuard&);
  
};

class WriterGuard
{
public:
  WriterGuard();
  ~WriterGuard();

  static void initialize();
private:
  WriterGuard(const WriterGuard&);
  const WriterGuard& operator= (const WriterGuard&);
  
};
#endif
