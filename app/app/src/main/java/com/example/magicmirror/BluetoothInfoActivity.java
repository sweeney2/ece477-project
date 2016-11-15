package com.example.magicmirror;

import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class BluetoothInfoActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_info);

        Intent intent = getIntent();
        String btDeviceName = (String) intent.getSerializableExtra("btDeviceName");
        String btDeviceAddress = (String) intent.getSerializableExtra("btDeviceAddress");
        ArrayList<String> btDeviceUuids = intent.getStringArrayListExtra("btDeviceUUIDs");

        TextView name = (TextView) findViewById(R.id.name_val);
        TextView address = (TextView) findViewById(R.id.address_val);

        name.setText(btDeviceName);
        address.setText(btDeviceAddress);

        ListView listview = (ListView) findViewById(R.id.uuid_list);
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, btDeviceUuids);
        listview.setAdapter(arrayAdapter);
    }
}
