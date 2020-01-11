/*
 * Copyright (C) 2018 Baidu, Inc. All Rights Reserved.
 */
package com.dodola.tracehooker;

import android.os.Bundle;
import android.os.Trace;
import androidx.appcompat.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Trace.beginSection("test");
//        Debug.startMethodTracing();
        setContentView(R.layout.activity_main);
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText("hellllllllllllo hook");
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                for (int i = 0; i < 10; i++) {
                    Log.d("TT", "xx");
                }
            }
        });

//        Debug.stopMethodTracing();
        Trace.endSection();

    }

}
