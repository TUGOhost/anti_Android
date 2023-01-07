package com.tg.anti;

import static com.tg.android.anti.NativeLib.*;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {
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

        TextView emulator = findViewById(R.id.emulator);
        emulator.setText(AntiEmulator());

        TextView dualApp = findViewById(R.id.dualApp);
        dualApp.setText(AntiDualApp());
    }
}