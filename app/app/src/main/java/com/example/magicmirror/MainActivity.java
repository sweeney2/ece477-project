package com.example.magicmirror;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.ParcelUuid;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {
    static final int REQUEST_ENABLE_BT = 1;
    public BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    private ListView listview;
    public static final int MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION = 1;
    public boolean permissionGranted;
    public DeviceScan scanBluetooth;
    public UpdateUiElements update = new UpdateUiElements(this);
    public Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        button = (Button) findViewById(R.id.scan);
        listview = (ListView) findViewById(R.id.list);

        listview.setOnItemClickListener(new AdapterView.OnItemClickListener(){
            @Override
            public void onItemClick(AdapterView<?> adapter, View v, int position, long x){
                BluetoothDevice btDevice = (BluetoothDevice) adapter.getItemAtPosition(position);

                Intent intent = new Intent(MainActivity.this, BluetoothInfoActivity.class);
                if (btDevice.getName() != null) {
                    intent.putExtra("btDeviceName", btDevice.getName());
                }
                else {
                    intent.putExtra("btDeviceName", "Unknown");
                }

                if (btDevice.getAddress() != null) {
                    intent.putExtra("btDeviceAddress", btDevice.getAddress());
                }
                else {
                    intent.putExtra("btDeviceAddress", "Unknown");
                }

                intent.putExtra("btDevice", btDevice);
                startActivity(intent);
            }
        });

        permission();
        // Ensures Bluetooth is available on the device and it is enabled. If not,
        // displays a dialog requesting user permission to enable Bluetooth.
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        if (mBluetoothAdapter.isEnabled()) {
            update.statusMessage = R.string.bluetooth_enabled;
            update.updateStatusMessage();
        }
        else {
            update.statusMessage = R.string.bluetooth_disabled;
            update.updateStatusMessage();
        }
    }

    public void permission() {
        // Here, thisActivity is the current activity
        if (ContextCompat.checkSelfPermission(MainActivity.this,
                Manifest.permission.ACCESS_COARSE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(MainActivity.this,
                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION);
        }
        else {
            permissionGranted = true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        if (requestCode == MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION) {
            // If request is cancelled, the result arrays are empty.
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // permission was granted
                permissionGranted = true;
            } else {
                // permission denied
                permissionGranted = false;
            }
        }
    }

    public void scanBluetoothLE(View view) {

        if (mBluetoothAdapter.isEnabled()) {
            button.setEnabled(false);
            update.statusMessage = R.string.bluetooth_enabled;
            update.updateStatusMessage();

            scanBluetooth = new DeviceScan(mBluetoothAdapter, MainActivity.this);
            scanBluetooth.execute(permissionGranted);
        }
        else {
            update.statusMessage = R.string.bluetooth_disabled;
            update.updateStatusMessage();
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (requestCode == REQUEST_ENABLE_BT) {
            // Make sure the request was successful
            if (resultCode == RESULT_OK) {
                //Bluetooth is Enabled
                update.statusMessage = R.string.bluetooth_enabled;
                update.updateStatusMessage();
            }
            else {
                update.statusMessage = R.string.bluetooth_disabled;
                update.updateStatusMessage();
            }
        }
    }

}
