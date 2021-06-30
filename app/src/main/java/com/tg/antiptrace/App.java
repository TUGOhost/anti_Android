package com.tg.antiptrace;

import android.app.Application;

public class App extends Application {
    static String str;

    static {
        System.loadLibrary("native-lib");
    }
    // anti frida
    public static native String AntiFrida();

    // anti xposed/like xposed

    // anti root

    // anti debug

    // anti memdump

    // anti virtual app

    // anti emulator

    // anti dual app
}
