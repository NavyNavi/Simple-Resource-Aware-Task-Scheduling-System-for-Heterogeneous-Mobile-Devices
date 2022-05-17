package com.example.myapplication;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pInfo;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
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
import androidx.appcompat.widget.Toolbar;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

//import static com.fyp.d2d_android.MyFirebaseMessagingService.msgFlag;
@SuppressLint("MissingPermission")
public class WiFiDirect extends AppCompatActivity implements MessageTarget, WifiP2pManager.ConnectionInfoListener, Handler.Callback {

    Button btnOnOff, btnDiscover;
    ListView listView;
    TextView connectionStatus;
    //private Toolbar toolbar;

    WifiManager wifiManager;
    WifiP2pManager mManager;
    WifiP2pManager.Channel mChannel;

    BroadcastReceiver mReceiver;
    IntentFilter mIntentFilter;

    List<WifiP2pDevice> peers=new ArrayList<WifiP2pDevice>();
    String[] deviceNameArray;
    WifiP2pDevice[] deviceArray;
    Map<Integer, InetAddress> worker2Ip;
    private Handler handler = new Handler(Looper.getMainLooper(), this);

    String paringSSID;
    private byte[] SerializedTask;
    public static boolean transactionDone=false;

    //for notification handling
    private Context mContext;
    private int NOTIFICATION_ID = 1;
    private Notification mNotification;
    private NotificationManager mNotificationManager;

    public static final int MESSAGE_READ = 0x400 + 1;
    public static final int MY_HANDLE = 0x400 + 2;

    private void exqListener() {
        btnOnOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(wifiManager.isWifiEnabled())
                {
                    wifiManager.setWifiEnabled(false);
                    btnOnOff.setText("ON");
                }else {
                    wifiManager.setWifiEnabled(true);
                    btnOnOff.setText("OFF");
                }
            }
        });

        //not exist
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
        if(!wifiManager.isWifiEnabled()){
            btnOnOff.callOnClick();
        }
        btnDiscover.callOnClick();
    }

    private void checkLocationPermission() {
        Log.d("WifiDirect","checking location permission.");
        LocationManager manager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        if (!manager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
            Toast.makeText(this, "please enable location services for this app.", Toast.LENGTH_LONG).show();
            startActivity(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS));
        }
    }

    WifiP2pManager.PeerListListener peerListListener=new WifiP2pManager.PeerListListener() {
        @Override
        public void onPeersAvailable(WifiP2pDeviceList peerList) {
            if(!peerList.getDeviceList().equals(peers))
            {
                //TODO: check any more efficient method Collection=>list
                peers.clear();
                peers.addAll(peerList.getDeviceList());

                deviceNameArray=new String[peerList.getDeviceList().size()];
                deviceArray=new WifiP2pDevice[peerList.getDeviceList().size()];
                int index=0;

                for(WifiP2pDevice device : peerList.getDeviceList())
                {
                    Log.d("WifiDirect", String.format("Found device %d",index));
                    deviceNameArray[index]=device.deviceName;
                    deviceArray[index]=device;
                    index++;
                }

                ArrayAdapter<String> adapter=new ArrayAdapter<String>(getApplicationContext(),R.layout.list_item, R.id.list_item, deviceNameArray);
                listView.setAdapter(adapter);
            }

            if(peers.size()==0)
            {
                Toast.makeText(getApplicationContext(),"No Device Found",Toast.LENGTH_SHORT).show();
                return;
            }else {
                if (!transactionDone){ //&& CloudFileScreen.hasRequested){
                    try {
                        WifiP2pDevice device=fetchSecondDevice(deviceArray,paringSSID);
                        transactionDone=true;
                        connectTo(device);
                    }catch (IOException e){
                        Toast.makeText(getApplicationContext(),"2nd Device Not Found",Toast.LENGTH_SHORT).show();
                    }
                }
            }
        }
    };

    @Override
    public boolean handleMessage(Message msg){
        Log.d("handler", "Firing handler callback.");
        switch (msg.what) {
            case MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                String readMessage = new String(readBuf, 0, msg.arg1);
                Log.d("WifiDirect", readMessage);
                break;
            case MY_HANDLE:
                Object obj = msg.obj;
                MainActivity.setChatManager((TaskManager) obj);
        }
        return true;
    }

    public Handler getHandler() {
        return handler;
    }

    @Override
    public void onConnectionInfoAvailable(WifiP2pInfo wifiP2pInfo) {
        Log.d("WifiDirect","connection info available.");
        final InetAddress groupOwnerAddress=wifiP2pInfo.groupOwnerAddress;
        Thread handler = null;

        if(wifiP2pInfo.groupFormed && wifiP2pInfo.isGroupOwner)
        {
            Log.d("WifiDirect","connected as server");
            connectionStatus.setText("Host");
            try {
                handler = new ServerSocketHandler(((MessageTarget) this).getHandler());
                handler.start();
            } catch (IOException e) {
                Log.d("WifiDirect", "Failed to create a server thread");
                return;
            }
        }else if(wifiP2pInfo.groupFormed)
        {
            Log.d("WifiDirect","connected as client");
            //ping the server to register as worker
            handler = new ClientSocketHandler(((MessageTarget) this).getHandler(), wifiP2pInfo.groupOwnerAddress);
            handler.start();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        //mReceiver = new WiFiDirectBroadcastReceiver(mManager, mChannel, this);
        registerReceiver(mReceiver,mIntentFilter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mReceiver);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("WifiDirect", "Creating activity.");
        checkLocationPermission();
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_wifi);
        initialWork();
        exqListener();
    }

    private void initialWork() {
        //adding toolbar
        //toolbar = findViewById(R.id.toolbar);
        //setSupportActionBar(toolbar);
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
}
