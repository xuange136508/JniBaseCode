package jni.demo.com.jnidemo.utils;

import android.content.Context;
import android.os.Build;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;

import java.net.NetworkInterface;
import java.net.SocketException;
import java.security.MessageDigest;
import java.util.Locale;
import java.util.UUID;

/**
 * 获取设备唯一标识工具类
 */
public class DeviceIdUtil {


    public static String getDeviceId(Context context) {
        StringBuilder sbDeviceId = new StringBuilder();

        //获得设备默认IMEI
        String imei = getIMEI(context);
        Log.i("=====","IMEI："+ imei);
        //获得AndroidId
        String androidid = getAndroidId(context);
        Log.i("=====","androidid："+ androidid);
        //获得设备序列号
        String serial = getSERIAL();
        Log.i("=====","serial："+ serial);
        //获取MAC地址
        String mac = getLocalMac().replace(":", "");
        Log.i("=====","MAC："+ serial);
        //获得硬件uuid（根据硬件相关属性生成uuid）
        String uuid = getDeviceUUID().replace("-", "");
        Log.i("=====","uuid："+ uuid);

        if (imei != null && imei.length() > 0) {
            sbDeviceId.append(imei);
            sbDeviceId.append("|");
        }
        if (androidid != null && androidid.length() > 0) {
            sbDeviceId.append(androidid);
            sbDeviceId.append("|");
        }
        if (serial != null && serial.length() > 0) {
            sbDeviceId.append(serial);
            sbDeviceId.append("|");
        }
        if (mac != null && mac.length() > 0) {
            sbDeviceId.append(mac);
            sbDeviceId.append("|");
        }
        if (uuid != null && uuid.length() > 0) {
            sbDeviceId.append(uuid);
        }
        Log.i("=====","StringBuilder ALL："+ sbDeviceId.toString());

        if (sbDeviceId.length() > 0) {
            try {
                byte[] hash = getHashByString(sbDeviceId.toString());
                String sha1 = bytesToHex(hash);
                if (sha1 != null && sha1.length() > 0) {
                    Log.i("=====","DeviceID："+ sha1);
                    return sha1;
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        return UUID.randomUUID().toString().replace("-", "");
    }

    /**
     * 获得设备的IMEI
     */
    private static String getIMEI(Context context) {
        try {
            TelephonyManager tm = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
            return tm.getDeviceId();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return "";
    }

    /**
     * 获得设备的AndroidId
     */
    private static String getAndroidId(Context context) {
        try {
            return Settings.Secure.getString(context.getContentResolver(),
                    Settings.Secure.ANDROID_ID);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return "";
    }

    /**
     * 获得设备序列号，个别设备无法获取
     */
    private static String getSERIAL() {
        try {
            return Build.SERIAL;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return "";
    }

    /**
     * 获得设备硬件uuid
     * 使用硬件信息，计算出一个随机数
     */
    private static String getDeviceUUID(){
        String serial = null;
        String mDevId = "35" +
                Build.BOARD.length()%10+ Build.BRAND.length()%10 +   // 主板 系统定制商
                Build.CPU_ABI.length()%10 + Build.DEVICE.length()%10 +  //CPU指令集 设备参数
                Build.DISPLAY.length()%10 + Build.HOST.length()%10 +  //显示设备号
                Build.ID.length()%10 + Build.MANUFACTURER.length()%10 +  //设备硬件id  厂商
                Build.MODEL.length()%10 + Build.PRODUCT.length()%10 +  //设备型号 产品名
                Build.TAGS.length()%10 + Build.TYPE.length()%10 +  //设备标签  设备版本类型
                Build.USER.length()%10 ;
        Log.i("=====","UniqueId = "+ mDevId);
        try {
            serial = android.os.Build.class.getField("SERIAL").get(null).toString();
            Log.i("=====","SERIAL = "+ serial);
            return new UUID(mDevId.hashCode(), serial.hashCode()).toString();
        } catch (Exception exception) {
            serial = "serial";
        }
        //读取设备的ROM版本号、厂商名、CPU型号和其他硬件信息来组合出一串15位的号码
        return new UUID(mDevId.hashCode(), serial.hashCode()).toString();
    }


    private static String getLocalMac() {
        String macAddress = null;
        StringBuffer buf = new StringBuffer();
        NetworkInterface networkInterface = null;
        try {
            networkInterface = NetworkInterface.getByName("eth1");
            if (networkInterface == null) {
                networkInterface = NetworkInterface.getByName("wlan0");
            }
            if (networkInterface == null) {
                return "";
            }
            byte[] addr = networkInterface.getHardwareAddress();
            for (byte b : addr) {
                buf.append(String.format("%02X:", b));
            }
            if (buf.length() > 0) {
                buf.deleteCharAt(buf.length() - 1);
            }
            macAddress = buf.toString();
        } catch (SocketException e) {
            e.printStackTrace();
            return "";
        }
        return macAddress;


    }

    private static byte[] getHashByString(String data)
    {
        try{
            MessageDigest  messageDigest = MessageDigest.getInstance("SHA1");
            messageDigest.reset();
            messageDigest.update(data.getBytes("UTF-8"));
            return messageDigest.digest();
        } catch (Exception e){
            return "".getBytes();
        }
    }

    private static String bytesToHex(byte[] data){
        StringBuilder sb = new StringBuilder();
        String stmp;
        for (int n = 0; n < data.length; n++){
            stmp = (Integer.toHexString(data[n] & 0xFF));
            if (stmp.length() == 1)
                sb.append("0");
            sb.append(stmp);
        }
        return sb.toString().toUpperCase(Locale.CHINA);
    }
}
