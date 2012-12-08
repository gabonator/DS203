package eu.valky.dsoquad;

import android.graphics.Canvas;
import android.view.SurfaceHolder;

public class WaveformPlotThread extends Thread {
	private SurfaceHolder holder;
	private WaveformView plot_area;
	private boolean _run = false;
	
	public WaveformPlotThread(SurfaceHolder surfaceHolder, WaveformView view){
		holder = surfaceHolder;
		plot_area = view;
	}
	public void setRunning(boolean run){
		_run = run;
	}
	
	@Override
	public void run(){
		Canvas c;
		while(_run){
			c = null;
			try{
				c = holder.lockCanvas(null);
				synchronized (holder) {
					plot_area.PlotPoints(c);
				}
			}finally{
				if(c!=null){
					holder.unlockCanvasAndPost(c);
				}
			}
		}
	}
}
