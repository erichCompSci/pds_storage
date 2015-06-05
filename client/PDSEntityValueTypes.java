package client;

/**
 * Created by IntelliJ IDEA.
 * User: mansour
 * Date: Nov 13, 2002
 * Time: 3:01:26 PM
 * To change this template use Options | File Templates.
 */

public class PDSEntityValueTypes {
    public static final int UNKNOWN_TYPE = 0;
    public static final int OPAQUE_TYPE = 1;
    public static final int STRING_TYPE = 2;
    public static final int INT4_TYPE = 3;
    public static final int INT8_TYPE = 4;

    public static final String UNKNOWN_TYPE_STR = "Unknown";
    public static final String OPAQUE_TYPE_STR = "Blob";
    public static final String STRING_TYPE_STR = "String";
    public static final String INT4_TYPE_STR = "Int4";
    public static final String INT8_TYPE_STR = "Int8";

    static public String convert(int type) {
        switch (type) {
            case OPAQUE_TYPE:
                return OPAQUE_TYPE_STR;
            case STRING_TYPE:
                return STRING_TYPE_STR;
            case INT4_TYPE:
                return INT4_TYPE_STR;
            case INT8_TYPE:
                return INT8_TYPE_STR;
            default:
                return UNKNOWN_TYPE_STR;
        }
    }
    static public int convert (String type) {
        if (OPAQUE_TYPE_STR.equals(type))
            return OPAQUE_TYPE;
        if (STRING_TYPE_STR.equals(type))
            return STRING_TYPE;
        if (INT4_TYPE_STR.equals(type))
            return INT4_TYPE;
        if (INT8_TYPE_STR.equals(type))
            return INT8_TYPE;

        return UNKNOWN_TYPE;
    }
}
