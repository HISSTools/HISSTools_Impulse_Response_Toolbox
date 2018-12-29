
// Num of Outlets

outlets = 1;

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Object Variables ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// Colors

var background_rgb = [0, 0, 0, 0];
var handle_rgb = [0.95, 0.75, 0.34, 1.0];
var handle_inactive_rgb = [0.65, 0.65, 0.67, 1.0];
var label_rgb = [0.094, 0.118, 0.137, 0.8];
var marker_rgb = [0.38, 0.38, 0.4, 0.8];

// Labels

var pan_labels = [-50, "L", 0, "C", 50, "R"];
var depth_labels = [-100, "-100", 0, "0", 100, "+100"];

// Drawing Dimensions

var marker_thickness = 1;
var handle_diameter_min = 8;
var handle_diameter_max = 16;
var centre_line_length = 12;
var x_border = 30;
var y_border_top = 8;
var y_border_btm = 20;

// Display and Value Ranges

var pan_lo = -50;
var pan_hi =  50;

var depth_lo = -100;
var depth_hi =  100;

// Position Data

var pan_val = 0;
var depth_val = 0;

// Interaction Variables

var obj_active = 1;

// Mousing State Variables

var select = 0;
var last_x;
var last_y;

// Object Size

var ui_width = box.rect[2] - box.rect[0];
var ui_height = box.rect[3] - box.rect[1];


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Intilisation //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


// MGraphics Intilisation

mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Parameter Conversion //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function x2pan(x)
{
    return ((x - x_border) / (ui_width - 2 * x_border)) * (pan_hi - pan_lo) + pan_lo;
}

function pan2x(pan)
{
    return x_border + ((ui_width - 2 * x_border) * ((pan - pan_lo) / (pan_hi - pan_lo)));
}

function y2depth(y)
{
    return -((y - ui_height + y_border_btm) / (ui_height - y_border_top - y_border_btm)) * (depth_hi - depth_lo) + depth_lo;
}

function depth2y(depth)
{
    return ui_height - y_border_btm - ((ui_height - y_border_top - y_border_btm) * ((depth - depth_lo) / (depth_hi - depth_lo)));
}

function depth2diameter(depth)
{
    return handle_diameter_min + (handle_diameter_max - handle_diameter_min) * (1 - ((depth - depth_lo) / (depth_hi - depth_lo)));
}


// Set all to local

x2pan.local = 1;
pan2x.local = 1;
y2depth.local = 1;
depth2y.local = 1;
depth2diameter.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Clip Parameters /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



function clip_pan(pan)
{
    pan = pan < pan_lo ? pan_lo : pan;
    pan = pan > pan_hi ? pan_hi : pan;
    
    return pan;
}

function clip_depth(depth)
{
    depth = depth < depth_lo ? depth_lo : depth;
    depth = depth > depth_hi ? depth_hi : depth;
    
    return depth;
} 


// Set all to local

clip_pan.local = 1;
clip_depth.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Set Parameter Values //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function setlabeltext(r, g, b, a)
{
	label_rgb = [r, g, b, (a * 0.8)];

	mgraphics.redraw();
}


function setpan(pan)
{
    pan_val = clip_pan(pan);
            
    mgraphics.redraw();
}


function setdepth(depth)
{
    depth_val= clip_depth(depth);
        
    mgraphics.redraw();
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Set Active State of Object //////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function active(a)
{
    obj_active = a;
    mgraphics.redraw();
}


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Paint Routine //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function paint()
{    
    var i;
    
    with (mgraphics) 
	{                    
		internalresize(mgraphics.size[0], mgraphics.size[1]);

		// Draw background color
    
		set_source_rgba(background_rgb);
        rectangle(0, 0, ui_width, ui_height);
        fill();
            
        // Set line width and color
        
        set_line_width(marker_thickness);
        set_source_rgba(marker_rgb);
        
       	// Calculate background diagram points
        
		var x1 = Math.round(pan2x(-50)) + 0.5;    
       	var x2 = Math.round(pan2x(0)) + 0.5;
		var x3 = Math.round(pan2x(50)) + 0.5;
		var y1 = Math.round(depth2y(100)) + 0.5;
		var y2 = Math.round(depth2y(0)) + 0.5;
 		var y3 = Math.round(depth2y(-100)) + 0.5;
	
		// Draw - center vertical line
		
        move_to(x2, y1);
        line_to(x2, y3);
        stroke();
        
    	// Draw - top line

        move_to(x1, y1);
        line_to(x3, y1);
        stroke();
			
		// Draw - center crossline
		
        move_to(x2 - centre_line_length, y2);
        line_to(x2 + centre_line_length, y2);
        stroke();
	
		// Draw - diagonal lines
		
		move_to(x2, y1);
        line_to(x1, y2);
        stroke();	
	
		move_to(x2, y1);
        line_to(x3, y2);
        stroke();	

        // Set font
            
       	select_font_face("Lato");
        set_font_size(10);
        set_source_rgba(label_rgb);
            
        // Render pan labels            
                   
        for (i = 0; i < pan_labels.length / 2; i++)
        {
			var measure = text_measure((i + 1).toString());
            move_to(pan2x(pan_labels[i * 2]) - measure[0] / 2, y3 + 10 + measure[1] * 0.65); 
            show_text(pan_labels[i * 2 + 1]);
        }
           
      	// Render depth labels
                             
       	for (i = 0; i < depth_labels.length / 2; i++)
       	{
            var measure = text_measure(depth_labels[2 * i + 1].toString());
            var depthiszero = depth_labels[2 * i] == 0; // (i == 1);    // hack for right zero offset
            // left
           	move_to(x1 - measure[0] - 4, depth2y(depth_labels[2 * i]) + (measure[1]/2) * 0.73); 
        	show_text(depth_labels[2 * i + 1].toString());
            // right
            move_to((x3 + measure[0] * 0.23) + (depthiszero * 10), depth2y(depth_labels[2 * i]) + (measure[1]/2) * 0.73); 
            show_text(depth_labels[2 * i + 1].toString());
		} 
        
        // Calculate handle points
                        
        var x = pan2x(pan_val);
        var y = depth2y(depth_val);
        var diameter = depth2diameter(depth_val);
        
        // Set handle color
        
        if (obj_active)
            set_source_rgba(handle_rgb);
        else
            set_source_rgba(handle_inactive_rgb);
        
        // Draw handle
        
        ellipse(x - diameter/2, y - diameter/2, diameter, diameter);
        fill();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Mousing Interaction ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function onclick(x, y, but, cmd, shift, capslock, option, ctrl)
{
    // Assume no selection
    
    select = 0;

    // Cache mouse position for tracking delta movements

    last_x = x;    
    last_y = y;
    
    // Calculate current dimension values
    
    var compare_x = pan2x(pan_val);
    var compare_y = depth2y(depth_val);
	var handle_diameter = depth2diameter(depth_val);
    var handle_dist = handle_diameter * handle_diameter / 2;    

    // Calculate the distance from the mouse to the centre of the hanle
    
   	compare_x -= x;
    compare_y -= y;
        
    var compare_dist = compare_x * compare_x + compare_y * compare_y;
        
    // Check for handle selection (if the distance is suitably small)
    
    if ((compare_dist < handle_dist))
    	select = 1;   
    
    // Hide the cursor if a handle is selected

    if (select)
        max.message("hidecursor");
}


function ondrag(x, y, but, cmd, shift, capslock, option, ctrl)
{        
    // If a handle is selected, update the values according to mousing

    if (select)
    {
       pan_val = clip_pan(x2pan(x - last_x + pan2x(pan_val)));
       depth_val = clip_depth(y2depth(y - last_y + depth2y(depth_val)));
       outlet(0, "pan", pan_val);
       outlet(0, "depth", depth_val);
    }
    
    // Deselect the handle if the button has gone up

    if (!but)
    {
        select = 0;
        max.message("showcursor");
    }
    
    // Store current position and redraw
    
    last_x = x;
    last_y = y;
    mgraphics.redraw();
}


function ondblclick(x, y, but, cmd, shift, capslock, option, ctrl)
{
    // Set last position to current and call click routine to select the handle if in range

    last_x = x;
    last_y = y;
    
    onclick (x, y, but, cmd, shift, capslock, option, ctrl);
        
    // If a handle was selected reset the y value

    if (select)
    {
        pan_val = 0.;
		depth_val = 0;
        outlet(0, "pan", pan_val);
       	outlet(0, "depth", depth_val);
    }
    
    // Deselect the handle
    
    select = 0;
    max.message("showcursor");
}


// Set all to local

ondrag.local = 1;
onclick.local = 1; 
ondblclick.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Object Resize //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function internalresize(w,h)
{
    if (ui_width != w || ui_height != w)
	{
		ui_width = w;
    	ui_height = h;    
	}
}
internalresize.local = 1; 


function onresize(w,h)
{
    ui_width = w;
    ui_height = h;
    
    internalresize(w, h);

 	mgraphics.redraw();
}
onresize.local = 1; 