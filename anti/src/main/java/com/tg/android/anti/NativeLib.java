package com.tg.android.anti;

public class NativeLib {

    // Used to load the 'anti' library on application startup.
    static {
        System.loadLibrary("anti");
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

    // anti emulator
    public static native String AntiEmulator();

    // anti dual app
    public static native String AntiDualApp();
}