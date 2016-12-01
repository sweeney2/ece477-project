package com.example.magicmirror;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

public class NotificationService extends NotificationListenerService {
    Context context;
    public String ticker;
    public String pack;
    public String subtitle;
    public String title;
    public Bundle extras;

    @Override
    public IBinder onBind(Intent intent) {
        return super.onBind(intent);
    }

    @Override
    public void onCreate() {

        super.onCreate();
        context = getApplicationContext();
        Log.d("Notification", "Start Service");

    }

    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {

        pack = sbn.getPackageName();
        if (!pack.equals("com.android.vending") && !pack.equals("com.android.providers.downloads")) {

            if (sbn.getNotification().tickerText != null) {
                ticker = sbn.getNotification().tickerText.toString();
            } else {
                ticker = "No info available";
            }
            extras = sbn.getNotification().extras;
            title = extras.getString("android.title");
            if (extras.getCharSequence("android.text") != null) {
                subtitle = extras.getCharSequence("android.text").toString();
            } else {
                subtitle = "No info available";
            }

            Intent intent = new Intent("com.example.magicmirror.NOTIFICATION_INTENT");
            intent.putExtra("Notification_title", title);
            intent.putExtra("Notification_subtitle", subtitle);
            sendBroadcast(intent);

            Log.d("Notification", "Package: " + pack);
            Log.d("Notification", "Ticker: " + ticker);
            Log.d("Notification", "Title: " + title);
            Log.d("Notification", "Subtitle: " + subtitle);
        }

    }

    @Override
    public void onNotificationRemoved(StatusBarNotification sbn) {
        Log.d("Notification","Notification Removed");
        Intent intent = new Intent("com.example.magicmirror.NOTIFICATION_INTENT");
        intent.putExtra("Notification_title", "Removed");
        sendBroadcast(intent);

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d("Notification", "Stop Service");
    }
}
