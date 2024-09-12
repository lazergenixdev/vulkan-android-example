package com.company.app;

import android.app.Activity;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ListView;

public class App extends Activity {

	private SurfaceView surfaceView;

	public native int create();
	public native int destroy();
	public static native void createGraphics(Surface surface);
	public static native void resizeGraphics(Surface surface, int format, int width, int height);
	public void showDialog(String title, String message) {
		final App activity = this;
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				new AlertDialog.Builder(activity)
					.setTitle(title)
					.setMessage(message)
					.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialogInterface, int i) {
							LOGI("onClick", String.format("Got %d", i));
						}
					})
					.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialogInterface, int i) {
							LOGI("onClick", String.format("Got %d", i));
						}
					})
					.show();
			}
		});
	}

	public static native void destroyGraphics();

	static {
		try {
			System.loadLibrary("App");
		}
		catch (Exception e) {
			LOGE("Initialize", e.getMessage());
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceInfo) {
		super.onCreate(savedInstanceInfo);

		create();

		surfaceView = new SurfaceView(this);

		setContentView(surfaceView);

		surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
			@Override
			public void surfaceCreated(SurfaceHolder holder) {
				createGraphics(holder.getSurface());
			}

			@Override
			public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
				resizeGraphics(holder.getSurface(), format, width, height);
			}

			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
				destroyGraphics();
			}
		});

        LOGI("Hello world!", "I'm Java");
	}

	@Override
	protected void onDestroy() {
		destroy();
		super.onDestroy();
	}

	private static void LOGI(String function_name, String message) {
		Log.i(function_name, "[Java] " + message);
	}
	private static void LOGE(String function_name, String message) {
		Log.i(function_name, "[Java] " + message);
	}
}
