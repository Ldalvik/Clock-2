<?xml version="1.0" encoding="utf-8"?>
    <ScrollView
        xmlns:android="http://schemas.android.com/apk/res/android"
        android:layout_height="wrap_content"
        android:layout_width="match_parent">

        <LinearLayout
            android:layout_width="match_parent"
            android:layout_height="match_parent"
            android:gravity="center"
            android:orientation="vertical"
            android:id="@+id/ParentLayout">

            <TextView
                android:layout_height="wrap_content"
                android:textAppearance="?android:attr/textAppearanceMedium"
                android:layout_width="wrap_content"
                android:text="Change clock design"
                android:layout_marginTop="10dp"/>
            <LinearLayout
                android:layout_width="match_parent"
                android:layout_height="wrap_content"
                android:orientation="horizontal">

                <EditText
                    android:id="@+id/mode"
                    android:layout_width="wrap_content"
                    android:layout_height="wrap_content"
                    android:layout_weight="6"
                    android:hint="Color mode..." />

            <Button
                android:layout_height="wrap_content"
                android:layout_width="wrap_content"
                android:text="Set Mode"
                android:onClick="setMode"
                android:layout_weight="1"/>
            </LinearLayout>

            <TextView
                android:layout_height="wrap_content"
                android:layout_width="wrap_content"
                android:text="Solid Color"
                android:layout_marginTop="15dp"/>

            <Button
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:text="reset"
                android:layout_margin="10dp"
                android:onClick="reset"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/red"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/green"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/blue"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <View
                android:background="?android:attr/dividerVertical"
                android:layout_height="1dp"
                android:layout_width="match_parent"/>

            <TextView
                android:layout_height="wrap_content"
                android:layout_width="wrap_content"
                android:text="Gradient Start Color"
                android:layout_marginTop="15dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/startRed"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/startGreen"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/startBlue"
                android:max="255"
                android:layout_marginBottom="10dp"/>
            <TextView
                android:layout_height="wrap_content"
                android:layout_width="wrap_content"
                android:text="Gradient End Color"
                android:layout_marginTop="15dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/endRed"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/endGreen"
                android:max="255"
                android:layout_marginBottom="10dp"/>

            <SeekBar
                android:layout_height="wrap_content"
                android:layout_width="match_parent"
                android:id="@+id/endBlue"
                android:max="255"
                android:layout_marginBottom="10dp"/>

        </LinearLayout>

    </ScrollView>
