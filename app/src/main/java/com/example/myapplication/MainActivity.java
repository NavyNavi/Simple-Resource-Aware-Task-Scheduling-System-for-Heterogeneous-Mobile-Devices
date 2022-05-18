package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.example.myapplication.databinding.ActivityMainBinding;
import com.google.android.material.textfield.TextInputEditText;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

@SuppressLint("MissingPermission")
public class MainActivity extends AppCompatActivity {

    // Used to load the 'myapplication' library on application startup.
    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;
    private static TaskManager taskManager;
    private enum testcases {wifi_communication, matrix_mul};

    public static void setChatManager(TaskManager manager) {
        Log.d("MainActiity", "received handler.");
        taskManager = manager;
    }

    public void sendSerializedTask(String task, int workerId){
        Log.d("MainActiity", "sending task.");
        taskManager.write(task);
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("MainActivity", "Creating activity.");
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
    }

    public void onClick(View view) {
        Log.d("MainActivity", "Click detected.");
        TextView sampleTv = binding.sampleText;
        switch (view.getId()) {
            case R.id.nqueen_button:
                sampleTv.setText(startScheduler(testcases.wifi_communication.ordinal()));
                break;
            case R.id.to_wifi_button:
                Log.d("MainActivity", "To wifi button clicked.");
                Intent intent = new Intent(MainActivity.this, WiFiDirect.class);
                startActivity(intent);
        }
    }

    /**
     * A native method that is implemented by the 'myapplication' native library,
     * which is packaged with this application.
     */
    public native String startScheduler(int testcase);
    public native void receiveCompletedTask(String serializedTask);
}