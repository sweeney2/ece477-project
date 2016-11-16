package com.example.magicmirror;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class BluetoothInfoActivity extends AppCompatActivity {

    public Button connect;
    public BluetoothDevice btDevice;
    public int connectionstatus  = R.string.bluetooth_disconnected;
    public BluetoothGatt mBluetoothGatt;

    private int mConnectionState = STATE_DISCONNECTED;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_info);

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

    public void connectBluetoothLE(View view) {
        if (mConnectionState == STATE_DISCONNECTED) {
            connectionstatus = R.string.bluetooth_connecting;
            updateConnectionStatus();
            Button connect = (Button) findViewById(R.id.connect);
            connect.setEnabled(false);
            mBluetoothGatt = btDevice.connectGatt(this, false, mGattCallback);
        }
        else if (mConnectionState == STATE_CONNECTED) {
            mBluetoothGatt.disconnect();
            connectionstatus = R.string.bluetooth_disconnected;
            mConnectionState = STATE_DISCONNECTED;
            mBluetoothGatt.close();
            mBluetoothGatt = null;
            updateConnectionStatus();
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
                        Log.d("BluetoothConnection", "Connected");

                    }

                    else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                        mConnectionState = STATE_DISCONNECTED;
                        connectionstatus = R.string.bluetooth_disconnected;
                        Log.d("BluetoothConnection", "Disconnected");
                    }
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
}
