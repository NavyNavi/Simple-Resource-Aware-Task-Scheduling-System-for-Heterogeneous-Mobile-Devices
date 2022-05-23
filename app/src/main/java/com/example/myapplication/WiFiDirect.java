package com.example.myapplication;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;
import android.net.wifi.WifiManager;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pInfo;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import java.io.IOException;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

//import static com.fyp.d2d_android.MyFirebaseMessagingService.msgFlag;
@SuppressLint("MissingPermission")
public class WiFiDirect extends AppCompatActivity implements MessageTarget, WifiP2pManager.ConnectionInfoListener, Handler.Callback {

    Button btnStartTest, btnOnOff, btnDiscover;
    ListView listView;
    TextView connectionStatus;
    //private Toolbar toolbar;

    WifiManager wifiManager;
    WifiP2pManager mManager;
    WifiP2pManager.Channel mChannel;

    BroadcastReceiver mReceiver;
    IntentFilter mIntentFilter;

    List<WifiP2pDevice> peers = new ArrayList<>();
    String[] deviceNameArray;
    WifiP2pDevice[] deviceArray;
    Map<Integer, TaskManager> worker2taskManager = new HashMap<>();
    private final Handler handler = new Handler(Looper.getMainLooper(), this);

    String paringSSID;
    public static boolean transactionDone = false;

    private final String TAG = "WifiDirect";
    private enum testcases {wifi_communication, matrix_mul}
    public static final int MESSAGE_READ = 0x400 + 1;
    public static final int MY_HANDLE = 0x400 + 2;
    public static String logs;

    //native <--> java
    public native String startScheduler(int testcase);
    public native void executeTask(String serializedTask);
    public native void receiveCompletedTask(String serializedTask);
    public native int registerWorker();

    public void sendSerializedTask(String task, int workerId){
        Log.d(TAG, "sending task.");
        Objects.requireNonNull(worker2taskManager.get(workerId)).write(task);
    }

    public void sendResult(String res) {
        Log.d(TAG, "sending result.");
        Objects.requireNonNull(worker2taskManager.get(0)).write(res);
    }

    //network interface
    WifiP2pManager.PeerListListener peerListListener = new WifiP2pManager.PeerListListener() {
        @Override
        public void onPeersAvailable(WifiP2pDeviceList peerList) {
            if (!peerList.getDeviceList().equals(peers)) {
                //TODO: check any more efficient method Collection=>list
                peers.clear();
                peers.addAll(peerList.getDeviceList());

                deviceNameArray = new String[peerList.getDeviceList().size()];
                deviceArray = new WifiP2pDevice[peerList.getDeviceList().size()];
                int index = 0;

                for (WifiP2pDevice device : peerList.getDeviceList()) {
                    deviceNameArray[index] = device.deviceName;
                    deviceArray[index] = device;
                    index++;
                }

                ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplicationContext(), R.layout.list_item, R.id.list_item, deviceNameArray);
                listView.setAdapter(adapter);
            }

            if (peers.size() == 0) {
                Toast.makeText(getApplicationContext(), "No Device Found", Toast.LENGTH_SHORT).show();
            } else {
                if (!transactionDone) { //&& CloudFileScreen.hasRequested){
                    try {
                        WifiP2pDevice device = fetchSecondDevice(deviceArray, paringSSID);
                        transactionDone = true;
                        connectTo(device);
                    } catch (IOException ignored) {
                    }
                }
            }
        }
    };

    @Override
    public boolean handleMessage(Message msg) {
        Log.d("handler", "Firing handler callback.");
        switch (msg.what) {
            case MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                String readMessage = new String(readBuf, 0, msg.arg1);
                Log.d("handler", readMessage);
                if (readMessage.charAt(0) == '0') { //received task
                    executeTask(readMessage.substring(2));
                } else { //received result
                    receiveCompletedTask(readMessage.substring(2));
                }
                break;
            case MY_HANDLE:
                int newId = registerWorker();
                worker2taskManager.put(newId, (TaskManager) msg.obj);

        }
        return true;
    }

    public Handler getHandler() {
        return handler;
    }

    @Override
    public void onConnectionInfoAvailable(WifiP2pInfo wifiP2pInfo) {
        final InetAddress groupOwnerAddress=wifiP2pInfo.groupOwnerAddress;
        Thread handler;

        if(wifiP2pInfo.groupFormed && wifiP2pInfo.isGroupOwner)
        {
            Log.d(TAG,"connected as server");
            connectionStatus.setText("Host");
            try {
                handler = new ServerSocketHandler(((MessageTarget) this).getHandler());
                handler.start();
            } catch (IOException e) {
                Log.d(TAG, "Failed to create a server thread");
            }
        }else if(wifiP2pInfo.groupFormed)
        {
            Log.d(TAG,"connected as client");
            //ping the server to register as worker
            handler = new ClientSocketHandler(((MessageTarget) this).getHandler(), wifiP2pInfo.groupOwnerAddress);
            handler.start();
        }
    }


    public void connectTo(WifiP2pDevice d){
        Log.d("WifiDirect", "connecting.");
        final WifiP2pDevice device=d;
        WifiP2pConfig config=new WifiP2pConfig();
        config.deviceAddress=device.deviceAddress;

        mManager.connect(mChannel, config, new WifiP2pManager.ActionListener() {
            @Override
            public void onSuccess() {
                Toast.makeText(getApplicationContext(),"Connected to "+device.deviceName,Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(int i) {
                Toast.makeText(getApplicationContext(),"Not Connected",Toast.LENGTH_SHORT).show();
            }
        });
    }

    public static WifiP2pDevice fetchSecondDevice(WifiP2pDevice[] arr,String name) throws IOException{
        Log.d("WifiDirect", "finding second device.");

        for (WifiP2pDevice d:arr){
            if(d.deviceName.equalsIgnoreCase(name)){
                return d;
            }
        }
        throw new IOException("Device not found");
    }

    //activity lifecycle
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "Creating activity.");
        checkLocationPermission();
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_wifi);
        initialize();
        exqListener();
    }

    private void checkLocationPermission() {
        Log.d("WifiDirect", "checking location permission.");
        LocationManager manager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        if (!manager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
            Toast.makeText(this, "please enable location services for this app.", Toast.LENGTH_LONG).show();
            startActivity(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS));
        }
    }

    private void exqListener() {
        btnStartTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startScheduler(WiFiDirect.testcases.wifi_communication.ordinal());
            }
        });

        btnOnOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (wifiManager.isWifiEnabled()) {
                    wifiManager.setWifiEnabled(false);
                    btnOnOff.setText("ON");
                } else {
                    wifiManager.setWifiEnabled(true);
                    btnOnOff.setText("OFF");
                }
            }
        });

        btnDiscover.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mManager.discoverPeers(mChannel, new WifiP2pManager.ActionListener() {
                    @Override
                    public void onSuccess() {
                        connectionStatus.setText("Discovery Started");
                    }

                    @Override
                    public void onFailure(int i) {
                        connectionStatus.setText("Discovery Starting Failed");
                    }
                });
            }
        });

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                connectTo(deviceArray[i]);
            }
        });

        //automate WiFi enabling and discovery
        if (!wifiManager.isWifiEnabled()) {
            btnOnOff.callOnClick();
        }
        btnDiscover.callOnClick();
    }

    private void initialize() {
        //adding toolbar
        //toolbar = findViewById(R.id.toolbar);
        //setSupportActionBar(toolbar);
        btnStartTest = findViewById(R.id.start_test_button);
        btnOnOff = findViewById(R.id.wifi_on_off_button);
        btnDiscover = findViewById(R.id.start_button);
        listView = findViewById(R.id.device_list);
        connectionStatus = findViewById(R.id.wifi_status_text);

        wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        mManager = (WifiP2pManager) getSystemService(Context.WIFI_P2P_SERVICE);
        mChannel = mManager.initialize(this, getMainLooper(), null);

        mReceiver = new WiFiDirectBroadcastReceiver(mManager, mChannel, this);

        mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mReceiver);
    }

    @Override
    protected void onResume() {
        super.onResume();
        //mReceiver = new WiFiDirectBroadcastReceiver(mManager, mChannel, this);
        registerReceiver(mReceiver,mIntentFilter);
    }
}
