package jni.demo.com.jnidemo;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.math.BigInteger;
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

import javax.crypto.Cipher;

import jni.demo.com.jnidemo.jni.JNativeInterface;
import jni.demo.com.jnidemo.utils.Base64Utils;

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
                ((TextView)findViewById(R.id.tv_result1)).setText("包名："+getJavaPackageName());

              }
        });


        findViewById(R.id.btn_test2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //toast(getSha1AndMd5Info());

                ((TextView)findViewById(R.id.tv_result2)).setText("c获取的签名MD5："+ "\n"+getSha1AndMd5Info());
            }
        });

        findViewById(R.id.btn_test3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String md5 = JNativeInterface.getJniMd5().toLowerCase();
                toast("MD5 ==> "+md5);

                ((TextView)findViewById(R.id.tv_result3)).setText("java获取的签名MD5："+ "\n"+md5);
            }
        });


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



