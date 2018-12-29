//autowatch = 1;

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

//var rate_labels = [0.05, "0.05 Hz", 1, "1 Hz", 2, "2 Hz"];
//var amount_labels = [0, "0 %", 50, "50 %", 100, "100 %"];
var rate_labels = [1, "1Hz"];
var amount_labels = [50, "50%"];

// Drawing Dimensions

var marker_thickness = 1;
var handle_diameter_min = 10;
var handle_diameter_max = 18;
var x_border = 9;		// handle max radius
var y_border_top = 9;	// handle max radius
var y_border_btm = 5;	// handle min radius...

// Display and Value Ranges

var rate_lo = 0.05;
var rate_hi =  2.0;

var amount_lo = 0;
var amount_hi = 100;

// Position Data

var rate_val = 1;
var amount_val = 50;

// Interaction Variables

var obj_active = 0;

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


function x2rate(x)
{
    return ((x - x_border) / (ui_width - 2 * x_border)) * (rate_hi - rate_lo) + rate_lo;
}

function rate2x(rate)
{
    return x_border + ((ui_width - 2 * x_border) * ((rate - rate_lo) / (rate_hi - rate_lo)));
}

function y2amount(y)
{
    return -((y - ui_height + y_border_btm) / (ui_height - y_border_top - y_border_btm)) * (amount_hi - amount_lo) + amount_lo;
}

function amount2y(amount)
{
    return ui_height - y_border_btm - ((ui_height - y_border_top - y_border_btm) * ((amount - amount_lo) / (amount_hi - amount_lo)));
}

function amount2diameter(amount)
{
//	return handle_diameter_min + (handle_diameter_max - handle_diameter_min) * (1 - ((amount - amount_lo) / (amount_hi - amount_lo)));
	return handle_diameter_min + (handle_diameter_max - handle_diameter_min) * ((amount - amount_lo) / (amount_hi - amount_lo));
}


// Set all to local

x2rate.local = 1;
rate2x.local = 1;
y2amount.local = 1;
amount2y.local = 1;
amount2diameter.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Clip Parameters /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



function clip_rate(rate)
{
    rate = rate < rate_lo ? rate_lo : rate;
    rate = rate > rate_hi ? rate_hi : rate;
    
    return rate;
}

function clip_amount(amount)
{
    amount = amount < amount_lo ? amount_lo : amount;
    amount = amount > amount_hi ? amount_hi : amount;
    
    return amount;
} 


// Set all to local

clip_rate.local = 1;
clip_amount.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Set Parameter Values //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function setlabeltext(r, g, b, a)
{
	label_rgb = [r, g, b, (a * 0.8)];

	mgraphics.redraw();
}


function setrate(rate)
{
    rate_val = clip_rate(rate);
            
    mgraphics.redraw();
}


function setamount(amount)
{
    amount_val= clip_amount(amount);
        
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
        var measure_r;
        var measure_a;

		internalresize(mgraphics.size[0], mgraphics.size[1]);

		// Draw background color
    
		set_source_rgba(background_rgb);
        rectangle(0, 0, ui_width, ui_height);
        fill();
 
        // Calculate background diagram points
        
        var x1 = Math.round(rate2x(0.05));// + 0.5;    
        var x2 = Math.round(rate2x(1.0));// + 0.5;
        var x3 = Math.round(rate2x(2.0));// + 0.5;
        var y1 = Math.round(amount2y(100)) + 0.5;
        var y2 = Math.round(amount2y(50)) + 0.5;
        var y3 = Math.round(amount2y(0)) + 0.5;
    
        // Set font
            
        select_font_face("Lato");
        set_font_size(10);
        set_source_rgba(label_rgb);
            
        // Render rate labels            
                   
        for (i = 0; i < rate_labels.length / 2; i++)
        {
            measure_r = text_measure((i + 1).toString());    
//          move_to(rate2x(rate_labels[i * 2]) - measure_r[0] / 2, y3 + 10 + measure_r[1] * 0.65); 
            move_to((rate2x(rate_labels[i * 2]) - measure_r[0] / 2) - 5, y3 - 8 + measure_r[1] * 0.65); 
            show_text(rate_labels[i * 2 + 1]);
        }
           
        // Render amount labels
                             
        for (i = 0; i < amount_labels.length / 2; i++)
        {
            measure_a = text_measure(amount_labels[2 * i + 1].toString());                
//          move_to(x1 - measure_a[0] - 10, amount2y(amount_labels[2 * i]) + (measure_a[1]/2) * 0.73); 
            move_to(x1 + measure_a[0] - 18, amount2y(amount_labels[2 * i]) + (measure_a[1]/2) * 0.8); 
            show_text(amount_labels[2 * i + 1].toString());
        } 
           
        // Set line width and color
        
        set_line_width(marker_thickness);
        set_source_rgba(marker_rgb);
        
		// Draw - center vertical line
		
        move_to(x2, y1);
        line_to(x2, y3 - measure_r[1]);
        stroke();

		// Draw - center horizontal line
		
        move_to(x1 + measure_a[0] + 4, y2);
        line_to(x3, y2);
        stroke();
        
        // Calculate handle points
                        
        var x = rate2x(rate_val);
        var y = amount2y(amount_val);
        var diameter = amount2diameter(amount_val);
        
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
    
    var compare_x = rate2x(rate_val);
    var compare_y = amount2y(amount_val);
	var handle_diameter = amount2diameter(amount_val);
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
       rate_val = clip_rate(x2rate(x - last_x + rate2x(rate_val)));
       amount_val = clip_amount(y2amount(y - last_y + amount2y(amount_val)));
       outlet(0, "rate", rate_val);
       outlet(0, "amount", amount_val);
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
		if (cmd)
		{
			outlet(0, "buttons", 0);
		}
        rate_val = 1.;
		amount_val = 50.;
        outlet(0, "rate", rate_val);
       	outlet(0, "amount", amount_val);
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

