/*
 * Copyright (C) 2018 Baidu, Inc. All Rights Reserved.
 */
package com.dodola.tracehooker;

import android.app.Application;
import android.util.Log;

/**
 * Created by dodola on 2018/5/8.
 */
public class MyApplication extends Application {


    @Override
    public void onCreate() {
        super.onCreate();
        boolean b = Atrace.hasHacks();
        if (b) {
            Log.d("tt", "==========enable systraceeeeeeee=======");
            Atrace.enableSystrace();
        }

    }
}
