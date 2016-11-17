package com.example.magicmirror;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
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
        List<String> devices = new ArrayList<String>();
        for (BluetoothDevice bt : mLeDevices) {
            devices.add(bt.getName());
        }

        listview = (ListView) context.findViewById(R.id.list);
        ArrayAdapter<BluetoothDevice> arrayAdapter = new ArrayAdapter<BluetoothDevice>(context, android.R.layout.simple_list_item_1, mLeDevices);
        listview.setAdapter(arrayAdapter);

    }

    public void updateStatusMessage() {
        TextView textView = (TextView) context.findViewById(R.id.message);
        textView.setTextSize(20);
        textView.setText(statusMessage);
    }
}
