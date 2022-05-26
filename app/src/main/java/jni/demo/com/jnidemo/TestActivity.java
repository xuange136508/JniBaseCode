package jni.demo.com.jnidemo;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.RSAPublicKeySpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.UUID;

import javax.crypto.Cipher;

import jni.demo.com.jnidemo.bean.Person;
import jni.demo.com.jnidemo.jni.JNativeInterface;
import jni.demo.com.jnidemo.jni.JniStudyDemo;
import jni.demo.com.jnidemo.utils.Base64Utils;
import jni.demo.com.jnidemo.utils.DeviceIdUtil;

public class TestActivity extends AppCompatActivity {


    private JNativeInterface mJniUtils;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mJniUtils = new JNativeInterface();


        findViewById(R.id.btn_test1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toast(getJavaPackageName());
                ((TextView) findViewById(R.id.tv_result1)).setText("包名：" + getJavaPackageName());

            }
        });


        findViewById(R.id.btn_test2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //toast(getSha1AndMd5Info());

                ((TextView) findViewById(R.id.tv_result2)).setText("c获取签名MD5：" + "\n" + getSha1AndMd5Info());
            }
        });

        findViewById(R.id.btn_test3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String md5 = JNativeInterface.getJniMd5().toLowerCase();
                toast("MD5 ==> " + md5);

                ((TextView) findViewById(R.id.tv_result3)).setText("java获取签名MD5：" + "\n" + md5);
            }
        });

        findViewById(R.id.btn_test4).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //c实现
                //String deviceId = JNativeInterface.getDeviceCode();
                //java实现
                //String deviceId = DeviceIdUtil.getDeviceId(TestActivity.this);
                //toast("设备码 ==> "+deviceId);//98EB00C3C24BD68A7A3C52E3BB4D1AF4
                //toast("长度 ==> "+deviceId.length());//32
                //((TextView)findViewById(R.id.tv_result4)).setText("设备码："+ "\n"+deviceId);



                /*
                String deviceId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
                toast("设备码 ==> "+deviceId);
                */
                /*
                TelephonyManager TelephonyMgr = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
                if (ActivityCompat.checkSelfPermission(TestActivity.this, Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
                    toast("无权限");
                    return;
                }
                String deviceId = TelephonyMgr.getDeviceId();
                toast("设备码 ==> "+deviceId);
                 */
                /*
                String deviceId = getUniqueId();
                toast("设备码 ==> "+deviceId);
                Log.i("=====","设备码长度 = "+ deviceId.length());*/


                // TODO: 2022/5/8 学习JNI开发
                String testString = JniStudyDemo.stringFromJNI();
                toast("测试 ==> "+testString);

                /** Java 数据传递给 native */
                /*new JniStudyDemo().testData(true,
                        (byte) 1,
                        ',',
                        (short) 3,
                        4,
                        3.3f,
                        2.2d,
                        "DevYK",
                        28,
                        new int[]{1, 2, 3, 4, 5, 6, 7},
                        new String[]{"1", "2", "4"},
                        new Person("阳坤"),
                        new boolean[]{false, true}
                );*/

            }
        });
    }










    //检查权限
    private void checkPermission(){
        PackageManager pm = getPackageManager();
        boolean permission = (PackageManager.PERMISSION_GRANTED ==
                pm.checkPermission("android.permission.READ_PHONE_STATE", "com.jni.base"));
        if (permission) {
            Toast.makeText(this,"已获得该权限",Toast.LENGTH_LONG).show();
        }
    }



    //由于国内厂商的定制Rom等因素以及Android 系统本身唯一ID设计问题，我们没有办法保证各种场景下都100%唯一
    //其他方案：服务器生成一串东西存在本地，存在不可篡改的路径
    /**
     * 获取Android设备码
     * */
    private String getDeviceCode() {
        String deviceCode = "";
        //国际移动设备识别码（International Mobile Equipment Identity，IMEI），即通常所说的手机序列号、手机“串号”。
        //可辨识的范围是全球，即全球范围内IMEI不会重复。一机一号，类似于人的身份证号。
        //获取IMEI 需要权限 READ_PHONE_STATE
        //10.0 之后就不给非系统应用 IMEI 等不可便标识，保证隐私安全
        //存在返回null或者000000的垃圾数据的情况，在部分pad上可能无法获取到DeviceId
        TelephonyManager TelephonyMgr = (TelephonyManager)getSystemService(TELEPHONY_SERVICE);
        String mImei = TelephonyMgr.getDeviceId();
        //Android ID，不需要权限,可能返回null,极个别设备获取不到数据或得到错误数据,低版本稳定
        //设备首次启动后系统会随机生成一个64位的数字，用16进制字符串的形式表示
        //恢复出厂或者刷机后会被重置
        String mAndroidID = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);

        return deviceCode;
    }

    private String getMacAddress() {
        //WLAN MAC地址,没有wifi的时候，我们是无法获得数据,需要ACCESS_WIFI_STATE
        WifiManager wm = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        String mWlanMAC = wm.getConnectionInfo().getMacAddress();

        //蓝牙 MAC地址，需要BLUETOOTH权限
        //需要设备具备蓝牙或者wifi硬件
        //String mBtMAC = BluetoothAdapter.getDefaultAdapter().getAddress();
        //UUID 数据唯一、不需要权限，重新安装APP，DeviceId值会改变，安装APP重新生成新的UUID

        //Android 6.0开始，使用该方法获取到的mac地址都为02:00:00:00:00:00。替代方案是通过读取系统文件/sys/class/net/wlan0/address来获取mac地址。
        /*
        try {
            String mac =  new BufferedReader(new FileReader(new File("/sys/class/net/wlan0/address"))).readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }*/
        //扫描网络接口获取Mac地址，暂时可行
        String macAddress = null;
        StringBuffer buf = new StringBuffer();
        NetworkInterface networkInterface = null;
        try {
            networkInterface = NetworkInterface.getByName("eth1");
            if (networkInterface == null) {
                networkInterface = NetworkInterface.getByName("wlan0");
            }
            if (networkInterface == null) {
                macAddress =  "02:00:00:00:00:02";
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
            macAddress =  "02:00:00:00:00:02";
        }

        return mWlanMAC;
    }


    /**
     * 获取硬件信息组构建UUID
        Build.BOARD  如：BLA  主板名称,无需权限,同型号设备相同
        Build.BRAND  如：HUAWEI  厂商名称,无需权限,同型号设备相同
        Build.HARDWARE  如：kirin970  硬件名称,无需权限,同型号设备相同
        Build......更多硬件信息，略
    */
    private String getUniqueId(){
        String serial = null;
        String mDevId = "35" +
                Build.BOARD.length()%10+ Build.BRAND.length()%10 +   // 主板 系统定制商
                Build.CPU_ABI.length()%10 + Build.DEVICE.length()%10 +  //CPU指令集 设备参数
                Build.DISPLAY.length()%10 + Build.HOST.length()%10 +  //显示设备号
                Build.ID.length()%10 + Build.MANUFACTURER.length()%10 +  //设备硬件id  厂商
                Build.MODEL.length()%10 + Build.PRODUCT.length()%10 +  //设备型号 产品名
                Build.TAGS.length()%10 + Build.TYPE.length()%10 +  //设备标签  设备版本类型
                Build.USER.length()%10 ; //13 位   //
        Log.i("=====","mDevId = "+ mDevId);
        try {
            //API>=9 使用serial号
            //设备序列号 serial：如：LKX7N18328000931,无需权限,极个别设备获取不到数据,
            //同一批次出厂的的设备有可能出现生成的内容可能是一样的
            serial = android.os.Build.class.getField("SERIAL").get(null).toString();
            Log.i("=====","serial = "+ serial);
            return new UUID(mDevId.hashCode(), serial.hashCode()).toString();
        } catch (Exception exception) {
            serial = "serial";
        }
        //读取设备的ROM版本号、厂商名、CPU型号和其他硬件信息来组合出一串15位的号码
        return new UUID(mDevId.hashCode(), serial.hashCode()).toString();
    }


    private String getSha1AndMd5Info(){
        return getSignMd5Str();
    }

    private String getJavaPackageName(){
        return JNativeInterface.getJniPackageName();
    }


    private void toast(String mDemo){
        Toast.makeText(this, mDemo,Toast.LENGTH_SHORT).show();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * MD5加密
     * @param byteStr 需要加密的内容
     * @return 返回 byteStr的md5值
     */
    public static String encryptionMD5(byte[] byteStr) {
        //MessageDigest 类为应用程序提供信息摘要算法的功能，如 MD5 或 SHA 算法。
        // 信息摘要是安全的单向哈希函数，它接收任意大小的数据，输出固定长度的哈希值。
        MessageDigest messageDigest = null;
        StringBuffer md5StrBuff = new StringBuffer();
        try {
            messageDigest = MessageDigest.getInstance("MD5");
            messageDigest.reset();
            messageDigest.update(byteStr);
            byte[] byteArray = messageDigest.digest();
            //return Base64.encodeToString(byteArray,Base64.NO_WRAP);
            for (int i = 0; i < byteArray.length; i++) {
                //为了显示一个byte型的单字节十六进制(两位十六进制表示)的编码，请使用：
                //Integer.toHexString((byteVar & 0x000000FF) | 0xFFFFFF00).substring(6)
                if (Integer.toHexString(0xFF & byteArray[i]).length() == 1) {
                    md5StrBuff.append("0").append(Integer.toHexString(0xFF & byteArray[i]));
                } else {
                    md5StrBuff.append(Integer.toHexString(0xFF & byteArray[i]));
                }
            }
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return md5StrBuff.toString();
    }

    /**
     * 获取app签名md5值,与“keytool -list -keystore D:\Desktop\app_key”‘keytool -printcert     *file D:\Desktop\CERT.RSA’获取的md5值一样
     */
    public String getSignMd5Str() {
        try {
            PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);
            Signature[] signs = packageInfo.signatures;
            Signature sign = signs[0];
            String signStr = encryptionMD5(sign.toByteArray());
            return signStr;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return "";
    }




//----------------------------------------RSA算法实现----------------------------------------------------
    private static String RSA = "RSA";

    /**
     * 随机生成RSA密钥对(默认密钥长度为1024)
     */
    public static KeyPair generateRSAKeyPair(){
        return generateRSAKeyPair(1024);
    }

    /**
     * 随机生成RSA密钥对密钥长度，范围：512～2048 一般1024
     */
    public static KeyPair generateRSAKeyPair(int keyLength) {
        try {
            KeyPairGenerator kpg = KeyPairGenerator.getInstance(RSA);
            kpg.initialize(keyLength);
            return kpg.genKeyPair();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 用公钥加密 <br>
     * 每次加密的字节数，不能超过密钥的长度值减去11
     *
     * @param data
     *            需加密数据的byte数据
     * @param pubKey
     *            公钥
     * @return 加密后的byte型数据
     */
    public static byte[] encryptData(byte[] data, PublicKey publicKey) {
        try {
            Cipher cipher = Cipher.getInstance(RSA);
            // 编码前设定编码方式及密钥
            cipher.init(Cipher.ENCRYPT_MODE, publicKey);
            // 传入编码数据并返回编码结果
            return cipher.doFinal(data);
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 用私钥解密
     *
     * @param encryptedData
     *            经过encryptedData()加密返回的byte数据
     * @param privateKey
     *            私钥
     * @return
     */
    public static byte[] decryptData(byte[] encryptedData, PrivateKey privateKey) {
        try {
            Cipher cipher = Cipher.getInstance(RSA);
            cipher.init(Cipher.DECRYPT_MODE, privateKey);
            return cipher.doFinal(encryptedData);
        } catch (Exception e) {
            return null;
        }
    }

    /**
     * 通过公钥byte[](publicKey.getEncoded())将公钥还原，适用于RSA算法
     *
     * @param keyBytes
     * @return
     * @throws NoSuchAlgorithmException
     * @throws InvalidKeySpecException
     */
    public static PublicKey getPublicKey(byte[] keyBytes) throws NoSuchAlgorithmException, InvalidKeySpecException {
        X509EncodedKeySpec keySpec = new X509EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance(RSA);
        PublicKey publicKey = keyFactory.generatePublic(keySpec);
        return publicKey;
    }

    /**
     * 通过私钥byte[]将公钥还原，适用于RSA算法
     *
     * @param keyBytes
     * @return
     * @throws NoSuchAlgorithmException
     * @throws InvalidKeySpecException
     */
    public static PrivateKey getPrivateKey(byte[] keyBytes) throws NoSuchAlgorithmException, InvalidKeySpecException {
        PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance(RSA);
        PrivateKey privateKey = keyFactory.generatePrivate(keySpec);
        return privateKey;
    }

    /**
     * 使用N、e值还原公钥
     *
     * @param modulus
     * @param publicExponent
     * @return
     * @throws NoSuchAlgorithmException
     * @throws InvalidKeySpecException
     */
    public static PublicKey getPublicKey(String modulus, String publicExponent)
            throws NoSuchAlgorithmException, InvalidKeySpecException {
        BigInteger bigIntModulus = new BigInteger(modulus);
        BigInteger bigIntPrivateExponent = new BigInteger(publicExponent);
        RSAPublicKeySpec keySpec = new RSAPublicKeySpec(bigIntModulus, bigIntPrivateExponent);
        KeyFactory keyFactory = KeyFactory.getInstance(RSA);
        PublicKey publicKey = keyFactory.generatePublic(keySpec);
        return publicKey;
    }

    /**
     * 使用N、d值还原私钥
     *
     * @param modulus
     * @param privateExponent
     * @return
     * @throws NoSuchAlgorithmException
     * @throws InvalidKeySpecException
     */
    public static PrivateKey getPrivateKey(String modulus, String privateExponent) throws NoSuchAlgorithmException, InvalidKeySpecException {
        BigInteger bigIntModulus = new BigInteger(modulus);
        BigInteger bigIntPrivateExponent = new BigInteger(privateExponent);
        RSAPublicKeySpec keySpec = new RSAPublicKeySpec(bigIntModulus, bigIntPrivateExponent);
        KeyFactory keyFactory = KeyFactory.getInstance(RSA);
        PrivateKey privateKey = keyFactory.generatePrivate(keySpec);
        return privateKey;
    }

    /**
     * 从字符串中加载公钥
     *
     * @param publicKeyStr
     *            公钥数据字符串
     * @throws Exception
     *             加载公钥时产生的异常
     */
    public static PublicKey loadPublicKey(String publicKeyStr) throws Exception {
        try {
            byte[] buffer = Base64Utils.decode(publicKeyStr);
            KeyFactory keyFactory = KeyFactory.getInstance(RSA);
            X509EncodedKeySpec keySpec = new X509EncodedKeySpec(buffer);
            return (RSAPublicKey) keyFactory.generatePublic(keySpec);
        } catch (NoSuchAlgorithmException e) {
            throw new Exception("无此算法");
        } catch (InvalidKeySpecException e) {
            throw new Exception("公钥非法");
        } catch (NullPointerException e) {
            throw new Exception("公钥数据为空");
        }
    }

    /**
     * 从字符串中加载私钥<br>
     * 加载时使用的是PKCS8EncodedKeySpec（PKCS#8编码的Key指令）。
     *
     * @param privateKeyStr
     * @return
     * @throws Exception
     */
    public static PrivateKey loadPrivateKey(String privateKeyStr) throws Exception {
        try
        {
            byte[] buffer = Base64Utils.decode(privateKeyStr);
            // X509EncodedKeySpec keySpec = new X509EncodedKeySpec(buffer);
            PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(buffer);
            KeyFactory keyFactory = KeyFactory.getInstance(RSA);
            return (RSAPrivateKey) keyFactory.generatePrivate(keySpec);
        } catch (NoSuchAlgorithmException e) {
            throw new Exception("无此算法");
        } catch (InvalidKeySpecException e) {
            throw new Exception("私钥非法");
        } catch (NullPointerException e) {
            throw new Exception("私钥数据为空");
        }
    }

    /**
     * 从文件中输入流中加载公钥
     *
     * @param in
     *            公钥输入流
     * @throws Exception
     *             加载公钥时产生的异常
     */
    public static PublicKey loadPublicKey(InputStream in) throws Exception {
        try {
            return loadPublicKey(readKey(in));
        } catch (IOException e) {
            throw new Exception("公钥数据流读取错误");
        } catch (NullPointerException e) {
            throw new Exception("公钥输入流为空");
        }
    }

    /**
     * 从文件中加载私钥
     *
     * @param keyFileName
     *            私钥文件名
     * @return 是否成功
     * @throws Exception
     */
    public static PrivateKey loadPrivateKey(InputStream in) throws Exception {
        try {
            return loadPrivateKey(readKey(in));
        } catch (IOException e) {
            throw new Exception("私钥数据读取错误");
        } catch (NullPointerException e) {
            throw new Exception("私钥输入流为空");
        }
    }

    /**
     * 读取密钥信息
     *
     * @param in
     * @return
     * @throws IOException
     */
    private static String readKey(InputStream in) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String readLine = null;
        StringBuilder sb = new StringBuilder();
        while ((readLine = br.readLine()) != null) {
            if (readLine.charAt(0) == '-') {
                continue;
            } else {
                sb.append(readLine);
                sb.append('\r');
            }
        }

        return sb.toString();
    }

    /**
     * 打印公钥信息
     *
     * @param publicKey
     */
    public static void printPublicKeyInfo(PublicKey publicKey) {
        RSAPublicKey rsaPublicKey = (RSAPublicKey) publicKey;
        System.out.println("----------RSAPublicKey----------");
        System.out.println("Modulus.length=" + rsaPublicKey.getModulus().bitLength());
        System.out.println("Modulus=" + rsaPublicKey.getModulus().toString());
        System.out.println("PublicExponent.length=" + rsaPublicKey.getPublicExponent().bitLength());
        System.out.println("PublicExponent=" + rsaPublicKey.getPublicExponent().toString());
    }

    public static void printPrivateKeyInfo(PrivateKey privateKey) {
        RSAPrivateKey rsaPrivateKey = (RSAPrivateKey) privateKey;
        System.out.println("----------RSAPrivateKey ----------");
        System.out.println("Modulus.length=" + rsaPrivateKey.getModulus().bitLength());
        System.out.println("Modulus=" + rsaPrivateKey.getModulus().toString());
        System.out.println("PrivateExponent.length=" + rsaPrivateKey.getPrivateExponent().bitLength());
        System.out.println("PrivatecExponent=" + rsaPrivateKey.getPrivateExponent().toString());

    }


    //-----------------------------------------------------------------------------------

    /* 密钥内容 base64 code */
    private static String PUCLIC_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCfRTdcPIH10gT9f31rQuIInLwe"
            + "\r" + "7fl2dtEJ93gTmjE9c2H+kLVENWgECiJVQ5sonQNfwToMKdO0b3Olf4pgBKeLThra" + "\r"
            + "z/L3nYJYlbqjHC3jTjUnZc0luumpXGsox62+PuSGBlfb8zJO6hix4GV/vhyQVCpG" + "\r"
            + "9aYqgE7zyTRZYX9byQIDAQAB" + "\r";
    private static String PRIVATE_KEY = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAJ9FN1w8gfXSBP1/"
            + "\r" + "fWtC4gicvB7t+XZ20Qn3eBOaMT1zYf6QtUQ1aAQKIlVDmyidA1/BOgwp07Rvc6V/" + "\r"
            + "imAEp4tOGtrP8vedgliVuqMcLeNONSdlzSW66alcayjHrb4+5IYGV9vzMk7qGLHg" + "\r"
            + "ZX++HJBUKkb1piqATvPJNFlhf1vJAgMBAAECgYA736xhG0oL3EkN9yhx8zG/5RP/" + "\r"
            + "WJzoQOByq7pTPCr4m/Ch30qVerJAmoKvpPumN+h1zdEBk5PHiAJkm96sG/PTndEf" + "\r"
            + "kZrAJ2hwSBqptcABYk6ED70gRTQ1S53tyQXIOSjRBcugY/21qeswS3nMyq3xDEPK" + "\r"
            + "XpdyKPeaTyuK86AEkQJBAM1M7p1lfzEKjNw17SDMLnca/8pBcA0EEcyvtaQpRvaL" + "\r"
            + "n61eQQnnPdpvHamkRBcOvgCAkfwa1uboru0QdXii/gUCQQDGmkP+KJPX9JVCrbRt" + "\r"
            + "7wKyIemyNM+J6y1ZBZ2bVCf9jacCQaSkIWnIR1S9UM+1CFE30So2CA0CfCDmQy+y" + "\r"
            + "7A31AkB8cGFB7j+GTkrLP7SX6KtRboAU7E0q1oijdO24r3xf/Imw4Cy0AAIx4KAu" + "\r"
            + "L29GOp1YWJYkJXCVTfyZnRxXHxSxAkEAvO0zkSv4uI8rDmtAIPQllF8+eRBT/deD" + "\r"
            + "JBR7ga/k+wctwK/Bd4Fxp9xzeETP0l8/I+IOTagK+Dos8d8oGQUFoQJBAI4Nwpfo" + "\r"
            + "MFaLJXGY9ok45wXrcqkJgM+SN6i8hQeujXESVHYatAIL/1DgLi+u46EFD69fw0w+" + "\r" + "c7o0HLlMsYPAzJw="
            + "\r";

    private void use(){
        // 加密
        String source = "anychat";
        String encryptContent = "";

        try {
            // 从字符串中得到公钥
            // PublicKey publicKey = RSAUtils.loadPublicKey(PUCLIC_KEY);
            // 从文件中得到公钥
            InputStream inPublic = getResources().getAssets().open("rsa_public_key.pem");
            PublicKey publicKey = loadPublicKey(inPublic);
            // 加密
            byte[] encryptByte = encryptData(source.getBytes(), publicKey);
            // 为了方便观察吧加密后的数据用base64加密转一下，要不然看起来是乱码,所以解密是也是要用Base64先转换
            String afterencrypt = Base64Utils.encode(encryptByte);

            //加密后：
            encryptContent = (afterencrypt);
        } catch (Exception e) {
            e.printStackTrace();
        }

        // 解密
        try {
            // 从字符串中得到私钥
            // PrivateKey privateKey = RSAUtils.loadPrivateKey(PRIVATE_KEY);
            // 从文件中得到私钥
            InputStream inPrivate = getResources().getAssets().open("pkcs8_rsa_private_key.pem");
            PrivateKey privateKey = loadPrivateKey(inPrivate);
            // 因为RSA加密后的内容经Base64再加密转换了一下，所以先Base64解密回来再给RSA解密
            byte[] decryptByte = decryptData(Base64Utils.decode(encryptContent), privateKey);
            String decryptStr = new String(decryptByte);

            //解密后：
            toast(decryptStr);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}



