package client;


/**
 * Created by IntelliJ IDEA.
 * User: mansour
 * Date: Feb 4, 2003
 * Time: 8:25:20 PM
 * To change this template use Options | File Templates.
 */
public class PDSPollThread extends Thread {
    PDSPollThread(String str) {
        super(str);
    }
    public void run() {
        while(true) {
            if (nap(5000))
                return;

            client.pds.PDSWrapper.cm_poll();
        }
    }
    /**
     * Sleep for millisec duration
     * @param millisec amount of time to sleep in millisecs
     * @return true if we get interrupted, false otherwise
     */
    private boolean nap(long millisec) {
        try {
            Thread.sleep(millisec);
        } catch (InterruptedException ex) {
            return true;
        }
        return false;
    }
}
