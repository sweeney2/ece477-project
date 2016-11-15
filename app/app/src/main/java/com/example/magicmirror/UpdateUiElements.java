package com.example.magicmirror;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class UpdateUiElements extends Activity {
    public int statusMessage = R.string.bluetooth_disabled;
    private ListView listview;
    protected MainActivity context;

    public UpdateUiElements(MainActivity view) {
        context = view;
    }

    public void listResults(ArrayList<BluetoothDevice> mLeDevices) {
        statusMessage = R.string.bluetooth_results;
        updateStatusMessage();
        Log.d("BluetoothScan", "Scan Done");

        List<String> devices = new ArrayList<String>();
        for (BluetoothDevice bt : mLeDevices) {
            Log.d("Device Name", bt.getName());
            devices.add(bt.getName());
        }

        Log.d("BluetoothScan", "List Created");
        listview = (ListView) context.findViewById(R.id.list);
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(context, android.R.layout.simple_list_item_1, devices);
        listview.setAdapter(arrayAdapter);
    }

    public void updateStatusMessage() {
        TextView textView = (TextView) context.findViewById(R.id.message);
        textView.setTextSize(20);
        textView.setText(statusMessage);
    }
}
