package eu.valky.dsoquad;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class WaveformView extends SurfaceView implements SurfaceHolder.Callback{
	
	private static int[] ch1_data = new int[4096];
	private static int[] ch2_data = new int[4096];
	
	private WaveformPlotThread plot_thread;
	
	private Paint ch1_color = new Paint();
	private Paint ch2_color = new Paint();
	private Paint grid_paint = new Paint();
	private Paint cross_paint = new Paint();
	private Paint outline_paint = new Paint();

	public WaveformView(Context context, AttributeSet attrs){
		super(context, attrs);
		
		getHolder().addCallback(this);
		
		// initial values
		for(int x=0; x<getWidth(); x++){
			ch1_data[x] = 100;
			ch2_data[x] = 110;
		}
		
		plot_thread = new WaveformPlotThread(getHolder(), this);
		
		ch1_color.setColor(Color.YELLOW);
		ch2_color.setColor(Color.CYAN);
		grid_paint.setColor(Color.rgb(100, 100, 100));
		cross_paint.setColor(Color.rgb(70, 100, 70));
		outline_paint.setColor(Color.GREEN);
	}
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height){
		
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder){
		plot_thread.setRunning(true);
		plot_thread.start();
	}
	
	@Override
	public void surfaceDestroyed(SurfaceHolder holder){
		boolean retry = true;
		plot_thread.setRunning(false);
		while (retry){
			try{
				plot_thread.join();
				retry = false;
			}catch(InterruptedException e){
				
			}
		}
	}
	
	@Override
	public void onDraw(Canvas canvas){
		PlotPoints(canvas);
	}

	public void set_single(int i, int v1, int v2) {
		ch1_data[i] = v1;
		ch2_data[i] = v2;
	}	
	public void set_single1(int i, int v) {
		ch1_data[i] = v;
	}	
	public void set_single2(int i, int v) {
		ch2_data[i] = v;
	}	
	
	public void PlotPoints(Canvas canvas){
		// clear screen
		canvas.drawColor(Color.rgb(20, 20, 20));
		// draw vertical grids
	    for(int vertical = 1; vertical<10; vertical++){
	    	canvas.drawLine(
	    			vertical*(getWidth()/10)+1, 1,
	    			vertical*(getWidth()/10)+1, getHeight()+1,
	    			grid_paint);
	    }
	    // draw horizontal grids
	    for(int horizontal = 1; horizontal<8; horizontal++){
	    	canvas.drawLine(
	    			1, horizontal*(getHeight()/8)+1,
	    			getWidth()+1, horizontal*(getHeight()/8)+1,
	    			grid_paint);
	    }
 		
 		int h = getHeight();
 		int w = getWidth();
 		int n = 300;
 		
 		// plot data
		for(int x=0; x<300-1; x++){			
			canvas.drawLine(x*w/300, h-ch2_data[x]*h/256, (x+1)*w/300, h-ch2_data[x+1]*h/256, ch2_color);
			canvas.drawLine(x*w/300, h-ch1_data[x]*h/256, (x+1)*w/300, h-ch1_data[x+1]*h/256, ch1_color);
		}
	}
}
