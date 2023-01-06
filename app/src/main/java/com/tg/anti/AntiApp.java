package com.tg.anti;

import android.app.Application;
import android.content.Context;

public class AntiApp extends Application {

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);

        xcrash.XCrash.init(this);
    }
}
