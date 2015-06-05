// -*- mode: JDE; c-basic-offset: 2; -*-
package jwp;

import java.lang.*;
import java.util.*;

public class jwpClient
{    
  public static native int init();
  public static native int done();
  
  public static native int lookupString (jwpDomainId domainId,
                                         String name,
                                         String value);

  public static native int bindString (jwpDomainId domainId,
                                String name,
                                String value);

  public static native int lookupInt (jwpDomainId domainId,
                                      String name,
                                      Integer value);

  public static native int bindInt (jwpDomainId domainId,
                                    String name,
                                    int value);

  public static native int lookupBlob (jwpDomainId domainId,
                                       String name,
                                       Byte[] value);

  public static native int bindBlob (jwpDomainId domainId,
                                     String name,
                                     byte[] value);

  public static native jwpDomainId createDomain (String domainName,
                                                 String domainType,
                                                 int appVersion,
                                                 String appName);

  public static native jwpDomainId[] findMatchingDomains (String domainName,
                                                          String domainType,
                                                          int appVersion,
                                                          String appName);

    static
    {
        System.loadLibrary ("jwpClient");
    }

}


