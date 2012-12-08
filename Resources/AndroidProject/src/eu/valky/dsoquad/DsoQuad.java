package eu.valky.dsoquad;

import eu.valky.dsoquad.R;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class DsoQuad extends Activity implements  Button.OnClickListener {
	
	// Message types sent from the BluetoothRfcommClient Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;
    
	// Key names received from the BluetoothRfcommClient Handler
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";
    
    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    
	// bt-uart constants
    private static final int MAX_SAMPLES = 640;
    private static final int  MAX_LEVEL	= 240;
    private static final int  DATA_START = (MAX_LEVEL + 1);
    private static final int  DATA_END = (MAX_LEVEL + 2);
    
    private static final byte  REQ_DATA = 0x00;
    private static final byte  ADJ_HORIZONTAL = 0x01;
    private static final byte  ADJ_VERTICAL = 0x02;
    private static final byte  ADJ_POSITION = 0x03;

    private static final byte  CHANNEL1 = 0x01;
    private static final byte  CHANNEL2 = 0x02;
    
    // Run/Pause status
    private boolean bReady = false;
    // receive data 
    private int[] ch1_data = new int[MAX_SAMPLES/2];
	private int[] ch2_data = new int[MAX_SAMPLES/2];
    private int dataIndex=0, dataIndex1=0, dataIndex2=0;
	private boolean bDataAvailable=false;
	private int rangeMin=0;
	private int rangeMax=1000;
    
	private String strCommand = "";
	private int nCommandCounter = 0;
    // Layout Views
    private TextView mBTStatus;
    private Button mConnectButton;
    
    int dead = 0;
    
    public WaveformView mWaveform = null;

    
    // Name of the connected device
    private String mConnectedDeviceName = null;
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the RFCOMM services
    private BluetoothRfcommClient mRfcommClient = null;
    

    // stay awake
    protected PowerManager.WakeLock mWakeLock;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Set up the window layout
        requestWindowFeature(Window.FEATURE_NO_TITLE);        
        setContentView(R.layout.main);

        // Get local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        
        // If the adapter is null, then Bluetooth is not supported
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
         //   finish();
            return;
        }
        
        // Prevent phone from sleeping
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "My Tag"); 
        this.mWakeLock.acquire();
    }
    
    @Override
    public void onStart(){
    	super.onStart();
    	
    	// If BT is not on, request that it be enabled.
    	if (!mBluetoothAdapter.isEnabled()){
    		Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
    		startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
    	}
    	// Otherwise, setup the Oscillosope session
    	else{
    		if (mRfcommClient == null) setupOscilloscope();
    	}    	
    }
    
    @Override
    public synchronized void onResume(){
    	super.onResume();
    	// Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mRfcommClient != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mRfcommClient.getState() == BluetoothRfcommClient.STATE_NONE) {
              // Start the Bluetooth  RFCOMM services
              mRfcommClient.start();
            }
        }
    }
    
    @Override
    public void  onClick(View v){
    	int buttonID;
    	buttonID = v.getId();
    	switch (buttonID){
    	}
    }
    
    @Override
    public void onDestroy(){
    	super.onDestroy();
    	// Stop the Bluetooth RFCOMM services
        if (mRfcommClient != null) mRfcommClient.stop();
        // release screen being on
        if (mWakeLock.isHeld()) { 
            mWakeLock.release();
        }
    }
        
    /**
     * Sends a message.
     * @param message  A string of text to send.
     */
    private void sendMessage(String message){
    	// Check that we're actually connected before trying anything
    	if (mRfcommClient.getState() != BluetoothRfcommClient.STATE_CONNECTED) {
    		Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
    		return;
    	}
    	// Check that there's actually something to send
    	if (message.length() > 0) {
    		// Get the message bytes and tell the BluetoothRfcommClient to write
    		byte[] send = message.getBytes();
    		mRfcommClient.write(send);
    	}
    }
  

    private void setupOscilloscope(){
        //mPlaySound = new PlaySound();
 	
    	mBTStatus = (TextView) findViewById(R.id.txt_btstatus);
    	
    	mConnectButton = (Button) findViewById(R.id.button_connect);
    	mConnectButton.setOnClickListener(new OnClickListener(){
			public void onClick(View arg0) {
				//mPlaySound.play();
				BTConnect();
			}    		
    	});

    	findViewById(R.id.button1).setOnClickListener( new OnClickListener() {
    		public void onClick(View arg0) {
    	    	sendMessage("OSC.GetViewData();\n");
    	    	strCommand = "adcdata";
    	    	nCommandCounter = 0;
    		}
    	});
    	findViewById(R.id.button2).setOnClickListener( new OnClickListener() {
    		public void onClick(View arg0) {
    	    	sendMessage("Beep(100);\n");
    		}
    	});
    	findViewById(R.id.button3).setOnClickListener( new OnClickListener() {
    		public void onClick(View arg0) {
    	    	sendMessage("Print('Hello!');\n");
    		}
    	});
    	findViewById(R.id.button4).setOnClickListener( new OnClickListener() {
    		public void onClick(View arg0) {
    	    	sendMessage("CH1::Resolution=2;\nALL::Setup();\nUpdate();\n");
    		}
    	});
    	findViewById(R.id.button5).setOnClickListener( new OnClickListener() {
    		public void onClick(View arg0) {
    	    	sendMessage("CH1::Resolution=3;\nALL::Setup();\nUpdate();\n");
    		}
    	});
        
    	// Initialize the BluetoothRfcommClient to perform bluetooth connections
        mRfcommClient = new BluetoothRfcommClient(this, mHandler);
        
        // waveform / plot area
        mWaveform = (WaveformView)findViewById(R.id.WaveformArea);
    }
    
    private void BTConnect(){
    	Intent serverIntent = new Intent(this, DeviceListActivity.class);
    	startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
    }
    
    private int toScreenPos(byte position){
    	//return ( (int)MAX_LEVEL - (int)position*6 );
    	return ( (int)MAX_LEVEL - (int)position*6 - 7);
    }
    
    // The Handler that gets information back from the BluetoothRfcommClient
    private final Handler mHandler = new Handler(){
    	@Override
        public void handleMessage(Message msg){
    		switch (msg.what){
    		case MESSAGE_STATE_CHANGE:
    			switch (msg.arg1){
    			case BluetoothRfcommClient.STATE_CONNECTED:
    				mBTStatus.setText(R.string.title_connected_to);
                    mBTStatus.append("\n" + mConnectedDeviceName);
    				break;
    			case BluetoothRfcommClient.STATE_CONNECTING:
    				mBTStatus.setText(R.string.title_connecting);
    				break;
    			case BluetoothRfcommClient.STATE_NONE:
    				mBTStatus.setText(R.string.title_not_connected);
    				break;
    			}
    			break;
    		case MESSAGE_READ: // todo: implement receive data buffering
    			byte[] readBuf = (byte[]) msg.obj;
    			int data_length = msg.arg1;
    			if ( strCommand == "adcdata" )
    			{
    				int nBaseOfs = 0;
    				if ( nCommandCounter == 0 )
    				{
    					// ANS=(binary, len=%d)
    					for (; nBaseOfs<data_length; nBaseOfs++)
    						if ( readBuf[nBaseOfs] == ')' )
    						{
    							nBaseOfs++;
    							break;
    						}
    				}
    				for (int i=nBaseOfs; i<data_length; i++, nCommandCounter++)
    				{
    					int nValue = readBuf[i] & 0xff;
    					if ( nCommandCounter % 2 == 0 )
    						mWaveform.set_single1( nCommandCounter/2, nValue);
    					else
    						mWaveform.set_single2( nCommandCounter/2, nValue);
    				}
    				if ( nCommandCounter >= 300*2 )
    					strCommand = "";
    			}
    			break;
    		case MESSAGE_DEVICE_NAME:
    			// save the connected device's name
                mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                Toast.makeText(getApplicationContext(), "Connected to "
                        + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
    			break;
    		case MESSAGE_TOAST:
    			Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
                        Toast.LENGTH_SHORT).show();
    			break;
    		}
    	}
    };
    
    public void onActivityResult(int requestCode, int resultCode, Intent data){
    	switch (requestCode) {
    	case REQUEST_CONNECT_DEVICE:
    		// When DeviceListActivity returns with a device to connect
    		if (resultCode == Activity.RESULT_OK){
    			// Get the device MAC address
    			String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
    			// Get the BLuetoothDevice object
                BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
                // Attempt to connect to the device
                mRfcommClient.connect(device);
    		}
    		break;
    	case REQUEST_ENABLE_BT:
    		// When the request to enable Bluetooth returns
            if (resultCode == Activity.RESULT_OK){
            	// Bluetooth is now enabled, so set up the oscilloscope
            	setupOscilloscope();
            }else{
            	// User did not enable Bluetooth or an error occured
                Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                finish();
            }
    		break;
    	}
    }
}
