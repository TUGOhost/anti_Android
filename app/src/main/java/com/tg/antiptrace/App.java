package com.tg.antiptrace;

import android.app.Application;

public class App extends Application {
    static String str;
    static {
        System.loadLibrary("native-lib");
    }

    // anti frida
    public static native String stringFromJNI();
    public static native String stringFromTime();
    //public static native String stringFromFile();
    public static native String stringFromTrick();
    //public static native String stringFromVm();
    //public static native String stringFromPtrace();
    public static native String stringFromBkpt();
    public static native String stringFromFork();
    public static native String stringFromSignal();

    // anti dual app
    public static native String antiDualAppForSystem();
    public static native String antiDualAppForUser();
}
