package com.tg.anti;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

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

    //public static native int haveSu();

    //public static native int haveMagicMount();

    //public static native int findMagiskdSocket();

    //public static native int testIoctl();

    //static native String getPropsHash();
}