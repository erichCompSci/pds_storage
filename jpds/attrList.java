// -*- mode: Java; c-basic-offset: 2; -*-
package jwp;

import java.lang.*;
import java.util.*;

public class attrList
{

  static final int Attr_Undefined = 1000;
  static final int Attr_Int4 = 1001;
  static final int Attr_Int8 = 1002;
  static final int Attr_String = 1003;
  static final int Attr_Opaque = 1004;
  static final int Attr_Atom = 1005;
  static final int Attr_List = 1006;

  attrList()
  {
    myInit();
  }

  public final native void myInit();

  protected native void finalize();

  public native int addAttr (String attrName, String attrValue);
  public native int addAttr (String attrName, int attrValue);
  public native int addAttr (String attrName, long attrValue);
  public native int setAttr (String attrName, String attrValue);
  public native int setAttr (String attrName, int attrValue);
  public native int setAttr (String attrName, long attrValue);
  
  public native Object queryAttr (String attrName, int attrType);

}
