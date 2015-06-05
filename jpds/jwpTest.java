import java.lang.*;
import java.util.*;
import jwp.*;

public class jwpTest
{

    public static void main (String argv[])
    {
        jwp.jwpDomainId newDomainId;
        String s1 = null;

        jwp.jwpClient.init();

        newDomainId = jwp.jwpClient.createDomain ("jwp", "test", 1, "jwpTest");

        jwp.jwpClient.bindString (newDomainId, "question", "answer");
        
        jwp.jwpClient.lookupString (newDomainId, "question", s1);

        System.out.println ("s1 = " + s1);

        jwp.jwpClient.done();
    }

}
