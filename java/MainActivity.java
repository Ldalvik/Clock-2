package medford.root.clock;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.SeekBar;
import medford.root.clock.bluetooth.BluetoothThread;
import medford.root.clock.color.Gradient;
import medford.root.clock.color.Solid;

public class MainActivity extends AppCompatActivity {
    SeekBar red, green, blue, startRed, startGreen, startBlue, endRed, endGreen, endBlue;
    EditText mode;
    public BluetoothThread bluetooth;
    BluetoothSocket socket;
    Utils utils;
    Solid s;
    Gradient g;
    String BT_MAC_ADDRESS =  "00:06:66:D0:E6:64";
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mode =             findViewById(R.id.mode);
        red =               findViewById(R.id.red);
        green =           findViewById(R.id.green);
        blue =             findViewById(R.id.blue);
        startRed =     findViewById(R.id.startRed);
        startGreen = findViewById(R.id.startGreen);
        startBlue =   findViewById(R.id.startBlue);
        endRed =         findViewById(R.id.endRed);
        endGreen =     findViewById(R.id.endGreen);
        endBlue =       findViewById(R.id.endBlue);
        utils =            new Utils(this);

        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        socket = BluetoothThread.connect(bluetoothAdapter.getRemoteDevice(BT_MAC_ADDRESS));
        for(int i = 0; i < 5; i++){
            if(!BluetoothThread.isConnected){
                BluetoothThread.disconnect();
                socket = BluetoothThread.connect(bluetoothAdapter.getRemoteDevice(BT_MAC_ADDRESS));
            } else {
                break;
            }
        }

        bluetooth = new BluetoothThread(socket, utils);
        bluetooth.start();
        s = new Solid(bluetooth);
        g = new Gradient(bluetooth);

        red.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                s.sendRed(progress);
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        green.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                s.sendGreen(progress);
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        blue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                s.sendBlue(progress);
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        startRed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g.setStartRed(progress).sendStart();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        startGreen.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g.setStartGreen(progress).sendStart();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        startBlue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g.setStartBlue(progress).sendStart();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        endRed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g.setEndRed(progress).sendEnd();
        }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        endGreen.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g.setEndGreen(progress).sendEnd();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        endBlue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                g.setEndBlue(progress).sendEnd();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    public void setMode(View v){
        String modeType = utils.getValue(mode);
        bluetooth.write("m:" + modeType + "/");
    }
}
