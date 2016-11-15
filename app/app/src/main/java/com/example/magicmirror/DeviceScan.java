package com.example.magicmirror;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;

import java.util.ArrayList;


public class DeviceScan extends AsyncTask<Boolean, Void, Void> {

    @Override
    protected Void doInBackground(Boolean... params) {
        scanLeDevice(params[0]);
        return null;
    }

    @Override
    protected void onPostExecute (Void result){
        update.listResults(mLeDevices);
    }

    public BluetoothAdapter mBluetoothAdapter;
    private final BluetoothLeScanner scanner;
    public boolean mScanning;
    private Handler mHandler;
    public ArrayList<BluetoothDevice> mLeDevices;
    public int scanNumber;
    public final static Object sync = new Object();
    protected MainActivity context;
    public UpdateUiElements update;

    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 40000;

    public DeviceScan (BluetoothAdapter bluetooth, MainActivity view) {
        mBluetoothAdapter = bluetooth;
        mLeDevices = new ArrayList<BluetoothDevice>();
        scanNumber = 0;
        mHandler = new Handler();
        scanner = bluetooth.getBluetoothLeScanner();
        context = view;
        update = new UpdateUiElements(view);
    }

    public void scanLeDevice(final boolean enable) {
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    synchronized(sync) {
                        stopScan();
                        sync.notifyAll();
                    }
                }
            }, SCAN_PERIOD);
            mScanning = true;
            Log.d("BluetoothScan", "StartScan");
            update.statusMessage = R.string.bluetooth_scanning;
            update.updateStatusMessage();
            scanner.startScan(mLeScanCallback);
            synchronized(sync) {
                while(mScanning) {
                    try {
                        sync.wait();
                    } catch (InterruptedException e) {
                        Log.d("BluetoothScan", "Scan Interrupted");
                    }
                }
            }
        } else {
            stopScan();
        }
    }

    public void stopScan() {
        mScanning = false;
        Log.d("BluetoothScan", "StopScan");
        scanner.stopScan(mLeScanCallback);
    }

    private ScanCallback mLeScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            Log.d("Result", "ResultFound");
            if(!mLeDevices.contains(result.getDevice())) {
                mLeDevices.add(result.getDevice());
                Log.d("Added", result.getDevice().getName());
            }
        }
    };

}