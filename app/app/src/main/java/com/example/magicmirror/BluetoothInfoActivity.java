package com.example.magicmirror;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.UUID;

import static com.example.magicmirror.R.id.time;

public class BluetoothInfoActivity extends AppCompatActivity {

    public Button connect;
    public BluetoothDevice btDevice;
    public int connectionstatus  = R.string.bluetooth_disconnected;
    public BluetoothGatt mBluetoothGatt;
    private NotificationReceiver notificationReceiver;

    private int mConnectionState = STATE_DISCONNECTED;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    public List<BluetoothGattCharacteristic> gattCharacteristics;
    public BluetoothGattCharacteristic app_titleCharacteristic;
    public BluetoothGattCharacteristic message_titleCharacteristic;
    public BluetoothGattCharacteristic message_subtitleCharacteristic;
    public String app_title;
    public String message_title;
    public String message_subtitle;
    public int sendVar;
    public String uuid;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_info);


        //Help from: https://github.com/kpbird/NotificationListenerService-Example/blob/master/NLSExample/src/main/java/com/kpbird/nlsexample/MainActivity.java
        notificationReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.example.magicmirror.NOTIFICATION_INTENT");
        registerReceiver(notificationReceiver, filter);

        Intent intent = getIntent();
        String btDeviceName = (String) intent.getSerializableExtra("btDeviceName");
        String btDeviceAddress = (String) intent.getSerializableExtra("btDeviceAddress");
        btDevice = intent.getExtras().getParcelable("btDevice");

        TextView name = (TextView) findViewById(R.id.name_val);
        TextView address = (TextView) findViewById(R.id.address_val);
        TextView status = (TextView) findViewById(R.id.status);

        name.setText(btDeviceName);
        address.setText(btDeviceAddress);
        status.setText(connectionstatus);

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(notificationReceiver);
        if (mBluetoothGatt != null) {
            mBluetoothGatt.disconnect();
            mBluetoothGatt.close();
            mBluetoothGatt = null;
        }
    }

    public void connectBluetoothLE(View view) {
        if (mConnectionState == STATE_DISCONNECTED) {
            connectionstatus = R.string.bluetooth_connecting;
            updateConnectionStatus();
            Button connect = (Button) findViewById(R.id.connect);
            connect.setEnabled(false);
            mBluetoothGatt = btDevice.connectGatt(this, false, mGattCallback);
        }
        else if (mConnectionState == STATE_CONNECTED) {
            if (mBluetoothGatt != null) {
                mBluetoothGatt.disconnect();
                connectionstatus = R.string.bluetooth_disconnected;
                mConnectionState = STATE_DISCONNECTED;
                mBluetoothGatt.close();
                mBluetoothGatt = null;
                updateConnectionStatus();
            }
        }
    }

    private final BluetoothGattCallback mGattCallback =
            new BluetoothGattCallback() {
                @Override
                public void onConnectionStateChange(BluetoothGatt gatt, int status,
                                                    int newState) {

                    if (newState == BluetoothProfile.STATE_CONNECTED) {
                        mConnectionState = STATE_CONNECTED;
                        connectionstatus = R.string.bluetooth_connected;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                updateConnectionStatus();
                                Button connect = (Button) findViewById(R.id.connect);
                                connect.setEnabled(true);
                            }
                        });
                        gatt.discoverServices();
                        Log.d("BluetoothConnection", "Connected");
                    }

                    else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                        mConnectionState = STATE_DISCONNECTED;
                        connectionstatus = R.string.bluetooth_disconnected;
                        Log.d("BluetoothConnection", "Disconnected");
                    }
                }

                @Override
                public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        if (mBluetoothGatt != null) {
                            BluetoothGattService bluetoothLEservice = mBluetoothGatt.getService(UUID.fromString("11223344-5566-7788-9900-aabbccddeeff"));

                            if (bluetoothLEservice == null) {
                                Log.d("Notification", "No Services Found");
                            }
                            else {
                                Log.d("GattService", "Service");
                                Log.d("GattService", "UUID: " + bluetoothLEservice.getUuid().toString());
                                app_titleCharacteristic = bluetoothLEservice.getCharacteristic(UUID.fromString("01020304-0506-0708-0900-0a0b0c0d0e0f"));
                                if (app_titleCharacteristic == null) {
                                    Log.d("GattCharacteristic", "App Title Characteristic Not Found");
                                }
                                message_titleCharacteristic = bluetoothLEservice.getCharacteristic(UUID.fromString("11112222-3333-4444-5555-666677778888"));
                                if (app_titleCharacteristic == null) {
                                    Log.d("GattCharacteristic", "Message Title Characteristic Not Found");
                                }
                                message_subtitleCharacteristic = bluetoothLEservice.getCharacteristic(UUID.fromString("99990000-aaaa-bbbb-cccc-ddddeeeeffff"));
                                if (app_titleCharacteristic == null) {
                                    Log.d("GattCharacteristic", "Message Subtitle Characteristic Not Found");
                                }
                            }
                        }
                        else {
                            Log.d("Notification", "No Gatt Connection Found");
                        }

                    }
                }

                @Override
                public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status ) {
                    String complete = Integer.toString(status);
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        complete = "Success";
                    }
                    else if (status == BluetoothGatt.GATT_FAILURE) {
                        complete = "Failure";
                    }
                    if (sendVar < 3) {
                        sendVar += 1;
                        sendNotification();
                    }
                    Log.d("WriteCharacteristic", complete);
                }
            };

    public void updateConnectionStatus() {
        TextView status = (TextView) findViewById(R.id.status);
        Button connect = (Button) findViewById(R.id.connect);

        status.setText(connectionstatus);

        if (connectionstatus == R.string.bluetooth_connected) {
            connect.setText(R.string.button_disconnect);
        }
        else if (connectionstatus == R.string.bluetooth_disconnected) {
            connect.setText(R.string.button_connect);
        }
    }

    public void sendNotification () {

        if (app_titleCharacteristic != null && sendVar == 1) {
            app_titleCharacteristic.setValue(app_title);
            mBluetoothGatt.writeCharacteristic(app_titleCharacteristic);
        }
        if (message_titleCharacteristic != null && sendVar == 2) {
            message_titleCharacteristic.setValue(message_title);
            mBluetoothGatt.writeCharacteristic(message_titleCharacteristic);
        }
        if (message_subtitleCharacteristic != null && sendVar == 3) {
            message_subtitleCharacteristic.setValue(message_subtitle);
            mBluetoothGatt.writeCharacteristic(message_subtitleCharacteristic);
        }
    }

    class NotificationReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (mConnectionState == STATE_CONNECTED) {
                //Send Notification over bluetooth
                // Loops through available Characteristics.
                app_title = intent.getStringExtra("Notification_pack");
                message_title = intent.getStringExtra("Notification_title");
                message_subtitle = intent.getStringExtra("Notification_subtitle");
                sendVar = 1;

                if (app_title.length() > 20) {
                    app_title = app_title.substring(0,17) + "...";
                }
                if (message_title.length() > 20) {
                    message_title = message_title.substring(0,17) + "...";
                }
                if (message_subtitle.length() > 20) {
                    message_subtitle = message_subtitle.substring(0,17) + "...";
                }

                sendNotification();
            }
        }

    }
}
