package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.example.myapplication.databinding.ActivityMainBinding;

@SuppressLint("MissingPermission")
public class MainActivity extends AppCompatActivity {

    // Used to load the 'myapplication' library on application startup.
    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;
    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "Creating activity.");
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        TextView logs = binding.logContentText;
        logs.setText(WiFiDirect.logs);
    }

    public void onClick(View view) {
        Log.d(TAG, "Click detected.");
        TextView logs = binding.logContentText;
        Log.d(TAG, "To wifi button clicked.");
        Intent intent = new Intent(MainActivity.this, WiFiDirect.class);
        startActivity(intent);
    }
}