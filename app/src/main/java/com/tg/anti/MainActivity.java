package com.tg.anti;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.annotation.RequiresApi;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.lang.reflect.Field;
import java.nio.file.Files;
import java.nio.file.Paths;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView frida = findViewById(R.id.frida);
        frida.setText(AntiFrida());
        TextView xposed = findViewById(R.id.xposed);
        xposed.setText(AntiXposed());

        TextView root = findViewById(R.id.root);
        root.setText(AntiRoot());

        TextView debug = findViewById(R.id.debug);
        debug.setText(AntiDebug());

        TextView memDump = findViewById(R.id.memDump);
        memDump.setText(AntiMemDump());

        TextView virtualApp = findViewById(R.id.virtualApp);
        virtualApp.setText(AntiVirtualApp());

        TextView emulator = findViewById(R.id.emulator);
        emulator.setText(AntiEmulator());

        TextView dualApp = findViewById(R.id.dualApp);
        dualApp.setText(AntiDualApp());

        TextView magisk = findViewById(R.id.magisk);
        magisk.setText(AntiMagisk());
    }


    // anti frida
    public static native String AntiFrida();

    // anti xposed/like xposed
    public static native String AntiXposed();

    // anti root
    public static native String AntiRoot();

    // anti debug
    public static native String AntiDebug();

    // anti mem dump
    public static native String AntiMemDump();

    // anti virtual app
    public static native String AntiVirtualApp();

    // anti emulator
    public static native String AntiEmulator();

    // anti dual app
    public static native String AntiDualApp();

    // anti magisk
    public static native String AntiMagisk();

    public static native int haveSu();

    public static native int haveMagicMount();

    public static native int findMagiskdSocket();

    public static native int testIoctl();

    //static native String getPropsHash();


    @RequiresApi(api = Build.VERSION_CODES.O)
    boolean isDualAppEx(String dataDir) {
        try {
            String simpleName = "wtf_jack";
            String testPath = dataDir + File.separator + simpleName;
            FileOutputStream fos = new FileOutputStream(testPath);
            FileDescriptor fileDescriptor = fos.getFD();
            Field fid_descriptor = fileDescriptor.getClass().getDeclaredField("descriptor");
            fid_descriptor.setAccessible(true);
            // 获取到 fd
            int fd = (Integer) fid_descriptor.get(fileDescriptor);
            // fd 反查真实路径
            String fdPath = String.format("/proc/self/fd/%d", fd);
            String realPath = Files.readSymbolicLink(Paths.get(fdPath)).toString();
            if (!realPath.substring(realPath.lastIndexOf(File.separator))
                    .equals(File.separator + simpleName)){
                // 文件名被修改
                return true;
            }
            // 尝试访问真实路径的父目录
            String fatherDirPath = realPath.replace(simpleName, "..");
            //Log.d(TAG, "isDualAppEx: " + fatherDirPath);
            File fatherDir = new File(fatherDirPath);
            if (fatherDir.canRead()) {
                // 父目录可访问
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return true;
        }
        return false;
    }
}