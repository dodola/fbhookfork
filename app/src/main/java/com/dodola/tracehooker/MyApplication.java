package com.dodola.tracehooker;

import android.app.Application;

import com.facebook.profilo.provider.atrace.Atrace;

/**
 * Created by dodola on 2018/5/8.
 */
public class MyApplication extends Application {


    @Override
    public void onCreate() {
        super.onCreate();
        boolean b = Atrace.hasHacks();
        if (b) {
            Atrace.enableSystrace();
        }
    }
}
