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

    public String getAppName(String pack) {
        String appName = "";

        switch(pack) {
            case "com.google.android.gm":
                appName = "Gmail";
                break;
            case "com.google.android.apps.messaging":
                appName = "Messenger";
                break;
            case "com.google.android.talk":
                appName = "Hangouts";
                break;
            case "com.google.android.dailer":
                appName = "Phone";
                break;
            case "com.firstrowria.pushnotificationtester":
                appName = "Push Notification Tester";
                break;
            case "com.google.android.calendar":
                appName = "Calendar";
                break;
            case "com.groupme.android":
                appName = "GroupMe";
                break;
            case "com.snapchat.android":
                appName = "Snapchat";
                break;
            case "com.facebook.katana":
                appName = "Facebook";
                break;
            default:
                appName = "";
        }

        return appName;
    }

    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {

        pack = sbn.getPackageName();
        String appName = getAppName(pack);
        if (!appName.equals("")) {

            extras = sbn.getNotification().extras;
            title = extras.getString("android.title");
            if (extras.getCharSequence("android.text") != null) {
                subtitle = extras.getCharSequence("android.text").toString();
            } else {
                subtitle = "No info available";
            }

            Intent intent = new Intent("com.example.magicmirror.NOTIFICATION_INTENT");
            intent.putExtra("Notification_pack", pack);
            intent.putExtra("Notification_title", title);
            intent.putExtra("Notification_subtitle", subtitle);
            sendBroadcast(intent);
        }

    }

    @Override
    public void onNotificationRemoved(StatusBarNotification sbn) {
        Log.d("Notification","Notification Removed");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d("Notification", "Stop Service");
    }
}
