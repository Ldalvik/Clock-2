package medford.root.clock.color;

import medford.root.clock.bluetooth.BluetoothThread;

public class Gradient {
    private int start_red, start_green, start_blue, end_red, end_green, end_blue;
    private BluetoothThread bluetooth;

    public Gradient(BluetoothThread bluetooth){
        this.bluetooth = bluetooth;
    }
    public Gradient setStartRed(int start_red){
        this.start_red = start_red;
        return this;
    }

    public Gradient setStartGreen(int start_green){
        this.start_green = start_green;
        return this;
    }

    public Gradient setStartBlue(int start_blue){
        this.start_blue = start_blue;
        return this;
    }

    public Gradient setEndRed(int start_red){
        this.start_red = start_red;
        return this;
    }

    public Gradient setEndGreen(int start_green){
        this.start_green = start_green;
        return this;
    }

    public Gradient setEndBlue(int start_blue){
        this.start_blue = start_blue;
        return this;
    }

    public void sendStart(){
        bluetooth.write("s:" + start_red + "," + start_green + "," + start_blue + "/");
    }

    public void sendEnd(){
        bluetooth.write("e:" + end_red + "," + end_green + "," + end_blue + "/");
    }
}
