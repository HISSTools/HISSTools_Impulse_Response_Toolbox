
outlets = 1;

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Object Variables ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// Colors

var background_rgb = [0, 0, 0, 0];
var curve_rgb = [0.95, 0.75, 0.34, 1.0];
var curve_inactive_rgb = [0.65, 0.65, 0.67, 1.0];
var handle_rgbs = [[0.3, 0.25, 0.33, 1.0], [0.28, 0.41, 0.14, 1.0], [0.82, 0.21, 0.15, 1.0]];
var handle_inactive_rgb = [0.8, 0.8, 0.8, 1];
var handle_text_rgb = [0.094, 0.118, 0.137, 1.0];
var marker_rgb = [0.38, 0.38, 0.4, 0.8];
var label_rgb = [0.094, 0.118, 0.137, 0.8];	// dynamic for theme

// Grid and Labels

var freq_grid = [125, 1000, 8000];
var freq_labels = [125, "125", 1000, "1K", 8000, "8K"];
var freq_label_point = -19;//-17;

var gain_grid = [-12, 0, 12];
var gain_labels = [-12, " ", "+12"];
var gain_label_point = 30;//34;

// Drawing Dimensions

var curve_thickness = 2;
var marker_thickness = 1;
var handle_thickness = 2;
var handle_diameter = 13;

// Display and Value Ranges

var freq_lo = 20;
var freq_hi = 22050;
var freq_clip_lo = 30;
var freq_clip_hi = 18000;

var gain_range = 24;//22;       // This value should be positive for standard usage
var gain_clip = 18;         	// This MUST be positive

var qors_vals_clip_lo = 0.05;//0.10;
var qors_vals_clip_hi = 18.0;//2.0;

// Filter Data

var num_filters = 3;
var filter_types = [1, 3, 4];	// Lowshelf, Bell, Highshelf
var freq_defaults = [125, 1000, 8000];
var freq_vals = [125, 1000, 8000];
var gain_defaults = [0, 0, 0];
var gain_vals = [0, 0, 0];
var qors_defaults = [0.5, 0.707107, 0.5];
var qors_vals = [0.5, 0.707107, 0.5];

// Interaction Variables

var obj_active = 1;
var qors_scale = 10;

// Mousing State Variables

var select = 0;
var handle = 2;
//var gain_state = 0;
var qors_mode = 0;
var last_x;
var last_y;

// Useful Constants

var sample_rate = 44100;
var handle_dist = handle_diameter * handle_diameter / 2.;
var mod_lo = Math.log(0.5);
var mod_scale = Math.log(2.) - mod_lo;
var log_freq_lo = Math.log(freq_lo);
var log_freq_hi = Math.log(freq_hi);
var log_10 = Math.log(10);
var freq_consts;

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


// Fill the frequency constants

fill_freq_consts();


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Set Frequency Constants /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function fill_freq_consts()
{
    var freq_val;
    var i;
    
    freq_consts = new Array;
    
    for (i = 0; i < ui_width + 1; i++)
        freq_consts[i] = x2freq(i);
}


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Parameter Conversion //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function x2freq(x)
{
    return Math.exp((x / ui_width) * (log_freq_hi - log_freq_lo) + log_freq_lo);
}

function freq2x(freq)
{
    return ui_width * (Math.log(freq) - log_freq_lo) / (log_freq_hi - log_freq_lo);
}

function gain2y(gain)
{
    return ((gain / gain_range) - 1) * -(ui_height / 2.);
}

function y2gain(y)
{
    return (y / -(ui_height / 2.) + 1.) * gain_range;
}

function ydel2qors(prev_qors, ydel)
{ 
    return prev_qors * Math.exp((mod_scale * (ydel + qors_scale) /  (2. * qors_scale)) + mod_lo);
}

// Set all to local

x2freq.local = 1;
freq2x.local = 1;
gain2y.local = 1;
y2gain.local = 1;
ydel2qors.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Clip Parameters /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function clip_freq(freq)
{
    freq = freq < freq_clip_lo ? freq_clip_lo : freq;
    freq = freq > freq_clip_hi ? freq_clip_hi : freq;
    
    return freq;
}

function clip_gain(gain)
{        
    gain = gain < -gain_clip ? -gain_clip : gain;
    gain = gain >  gain_clip ?  gain_clip : gain;
    
    return gain;
}

function clip_qors(qors_vals)
{
    qors_vals = qors_vals < qors_vals_clip_lo ? qors_vals_clip_lo : qors_vals;
    qors_vals = qors_vals > qors_vals_clip_hi ? qors_vals_clip_hi : qors_vals;
    
    return qors_vals;
}

// Set all to local

clip_freq.local = 1;
clip_gain.local = 1;
clip_qors.local = 1;


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Set Parameter Values //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function setlabeltext(r, g, b, a)
{
	label_rgb = [r, g, b, (a * 0.8)];

	mgraphics.redraw();
}

function setfreq(filter, freq)
{
    if (filter > 0 & filter <= num_filters)
    {
        freq_vals[filter - 1] = clip_freq(freq);
        handle = filter - 1;
    }
        
    mgraphics.redraw();
}

function setgain(filter, gain)
{
    if (filter > 0 & filter <= num_filters)
    {
        gain_vals[filter - 1] = clip_gain(gain);
        handle = filter - 1;
    }
        
    mgraphics.redraw();
}

function setq(filter, q)
{
    if (filter > 0 & filter <= num_filters)
    {
        qors_vals[filter - 1] = clip_qors(q);
        handle = filter - 1;
    }
        
    mgraphics.redraw();
}

function settype(filter, type)
{
    type = type < 0 ? 0 : type;
    type = type > 5 ? 5 : type;
    
    if (filter > 0 & filter <= num_filters)
    {
        filter_types[filter - 1] = type;
        handle = filter - 1;
    }
        
    mgraphics.redraw();
}

function setsr(sr)
{
    sample_rate = sr;
        
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
/////////////////////////////////// Set Skin Values ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function setbackgroundrgb(r, g, b, a)
{
	background_rgb = [r, g, b, a];

//	mgraphics.redraw();
}

function setcurvergb(r, g, b, a)
{
	curve_rgb = [r, g, b, a];

//	mgraphics.redraw();
}

function setcurveinactivergb(r, g, b, a)
{
	curve_inactive_rgb = [r, g, b, a];

//	mgraphics.redraw();
}

function sethandlergbs(handle, r, g, b, a)
{
	var n;

    if (handle > 0 & handle <= num_filters)
    {
		n = handle - 1;
        handle_rgbs[n] = [r, g, b, a];
	}

//	mgraphics.redraw();
}

function sethandleinactivergb(r, g, b, a)
{
	handle_inactive_rgb = [r, g, b, a];

//	mgraphics.redraw();
}

function sethandletextrgb(r, g, b, a)
{
	handle_text_rgb = [r, g, b, a];

//	mgraphics.redraw();
}

function redrawit()
{
	mgraphics.redraw();
}
redrawit.local  = 1;


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Filter and Curve Calculations //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// biquad filter sections	(original, with changes)

function calc_biquad_coefficients(a_coeff, b_coeff)
{
    var A, alpha, alphashelf;
    var a0, a1, a2, b0, b1, b2;
    var i;
    
    // For each filter calculate the biquad coeffiecients

    for (i = 0; i < num_filters; i++)
    {
        var w0 = 2.0 * Math.PI * freq_vals[i] / sample_rate;
        var cosw = Math.cos(w0);
        var sinw = Math.sin(w0);
        var G = gain_vals[i];
        var qs = qors_vals[i];
        
        switch (filter_types[i])
        {
	
			case 0:	// HIGHPASS COOKBOOK

				A		= Math.exp(0.11512925464970 * G);	// dBtoA ...
           		alpha	= sinw / (2.0 * qs);

				a0 =	  1.0 + alpha;
				a1 =	(-2.0 * cosw) / a0;
				a2 =	( 1.0 - alpha) / a0;	
				b0 =	( 1.0 + cosw) / 2.0;
				b0 =	( b0 * A);
				b1 =	 -1.0 - cosw;
				b1 =	( b1 * A) / a0;
				b2 =	 b0 / a0;
				b0 =	 b2;
                break;

            case 1: // LO SHELF COOKBOOK

				A		= Math.pow(10., G / 40.);
            
                if (filter_types[i] == 1)
                {
                    alpha = sinw / (2.0 * qs);
                    alphashelf = 2.0 * Math.sqrt(A) * alpha;
                }
                else
                {
                    alpha = (A * A + 1.0) / qs - (A - 1.0) * (A - 1.0);
                    alphashelf = sinw * (alpha < 0 ? 0 : Math.sqrt(alpha)); 
                }
                
                a0 =             (A + 1.0) + (A - 1.0) * cosw + alphashelf;
                a1 =    (-2.0 * ((A - 1.0) + (A + 1.0) * cosw)) / a0;
                a2 =            ((A + 1.0) + (A - 1.0) * cosw - alphashelf) / a0;
                b0 =       (A * ((A + 1.0) - (A - 1.0) * cosw + alphashelf)) / a0;
                b1 = (2.0 * A * ((A - 1.0) - (A + 1.0) * cosw)) / a0;
                b2 =       (A * ((A + 1.0) - (A - 1.0) * cosw - alphashelf)) / a0;
                
                Array
                break;

			case 2:	// NOTCH COOKBOOK

				A		= Math.exp(0.11512925464970 * G);
                alpha	= sinw / (2.0 * qs);

				a0 =	 1.0 + alpha;
				b0 =	 1.0 * A;
				b1 =	-2.0 * cosw;
				b2 =	 b0 / a0;
				b0 =	 b0 / a0;
				a1 =	 b1 / a0;
				b1 =	(b1 * A) / a0;
				a2 =	(1.0 - alpha) / a0;
                break;

            case 3: // PEAKING COOKBOOK
            
				A		= Math.pow(10., G / 40.);
                alpha	= sinw / (2.0 * qs);
                    
                a0 =   1.0 + alpha / A;
                a1 = (-2.0 * cosw) / a0;
                a2 =  (1.0 - alpha / A) / a0;    
                b0 =  (1.0 + alpha * A) / a0;
                b1 = (-2.0 * cosw) / a0;
                b2 =  (1.0 - alpha * A) / a0;
                break;
            
            case 4: // HI SHELF COOKBOOK
            
				A		= Math.pow(10., G / 40.);

                if (filter_types[i] == 4)
                {
                    alpha = sinw / (2.0 * qs);
                    alphashelf = 2.0 * Math.sqrt(A) * alpha;
                }
                else
                {
                    alpha = (A * A + 1.0) / qs - (A - 1.0) * (A - 1.0);
                    alphashelf = sinw * (alpha < 0 ? 0 : Math.sqrt(alpha)); 
                }
                    
                a0 =              (A + 1.0) - (A - 1.0) * cosw + alphashelf;
                a1 =      (2.0 * ((A - 1.0) - (A + 1.0) * cosw)) / a0;
                a2 =             ((A + 1.0) - (A - 1.0) * cosw - alphashelf) / a0;
                b0 =        (A * ((A + 1.0) + (A - 1.0) * cosw + alphashelf)) / a0;
                b1 = (-2.0 * A * ((A - 1.0) + (A + 1.0) * cosw)) / a0;
                b2 =        (A * ((A + 1.0) + (A - 1.0) * cosw - alphashelf)) / a0;
                break;
                
			case 5:	// LOWPASS COOKBOOK

				A		= Math.exp(0.11512925464970 * G);
                alpha	= sinw / (2.0 * qs);

				a0 =	  1.0 + alpha;
				a1 =	(-2.0 * cosw) / a0;
				a2 =	( 1.0 - alpha) / a0;
				b1 =	  1.0 - cosw;
				b0 =	  b1 / 2.0;
				b0 =	( b0 * A) / a0;
				b1 =	( b1 * A) / a0;
				b2 =	  b0 / a0;
                break;
        }
        
        a_coeff.push(1.);
        a_coeff.push(a1);
        a_coeff.push(a2);
        b_coeff.push(b0);
        b_coeff.push(b1);
        b_coeff.push(b2);
    }
}

// (original)

function calc_biquad_db(freq, a_coeff, b_coeff)
{
    var freq_val = 2 * Math.PI * freq / sample_rate;
    var pow = 1;
    var i;
    
    var A = 0.;
    var B = 0.;
    var C = 0.;
    var D = 0.;

    var cos1 = Math.cos(freq_val);    
    var sin1 = Math.sin(freq_val);
    var cos2 = Math.cos(freq_val * 2);
    var sin2 = Math.sin(freq_val * 2);
       
    // Accumulate the power response of the filters at a given input frequency
    
    for (i = 0; i < num_filters; i++)
    { 
        A = b_coeff[i * 3] + b_coeff[i * 3 + 1] * cos1 + b_coeff[i * 3 + 2] * cos2;
        B = b_coeff[i * 3 + 1] * sin1 + b_coeff[i * 3 + 2] * sin2;
        C = a_coeff[i * 3] + a_coeff[i * 3 + 1] * cos1 + a_coeff[i * 3 + 2] * cos2
        D = a_coeff[i * 3 + 1] * sin1 + a_coeff[i * 3 + 2] * sin2;
        
        var denominator = 1. / (C*C + D*D);
        var real = (A*C + B*D) * denominator; 
        var imag = (B*C - A*D) * denominator; 
        
        pow *= (real * real + imag * imag);
    }
       
    // Return the response in db
               
    return 10. * Math.log(pow) / log_10; 
}


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Paint Routine //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function paint()
{        
    var a_coeff = new Array;
    var b_coeff = new Array;
	var measure_f;
	var measure_g;
    var i;

    with (mgraphics) 
    {
		internalresize(mgraphics.size[0], mgraphics.size[1]);
		
        // Draw background color
    
        set_source_rgba(background_rgb);
        rectangle(0, 0, ui_width, ui_height);
        fill();
        
        // Set font
        
        select_font_face("Lato");
        set_font_size(10);
        set_source_rgba(label_rgb);
        
        // Render frequency labels            

        var freq_label_line = gain2y(freq_label_point);
                
        for (i = 0; i < freq_labels.length / 2; i++)
        {
            measure_f = text_measure((i + 1).toString());    
//			move_to(freq2x(freq_labels[i * 2]) + 2, freq_label_line + 2 + measure_f[1] * 0.65); 
			move_to(freq2x(freq_labels[i * 2]) - measure_f[0], freq_label_line + 5 + measure_f[1] * 0.65); 
            show_text(freq_labels[i * 2 + 1]);
        }
        
        // Render gain labels
        
        var gain_label_line = freq2x(gain_label_point);
             
        for (i = 0; i < gain_grid.length; i++)
        {
            measure_g = text_measure(gain_labels[i].toString());                
//			move_to(gain_label_line - measure_g[0] - 2, gain2y(gain_grid[i]) + (measure_g[1]/2) * 0.4); 
			move_to(gain_label_line - measure_g[0] - 2, gain2y(gain_grid[i]) + (measure_g[1]/2) * 0.6); 
            show_text(gain_labels[i].toString());
        } 

        // Set marker line width and color

        set_line_width(marker_thickness);
        set_source_rgba(marker_rgb);
    
        // Draw frequency grid
        
        for (i = 0; i < freq_grid.length; i++)
        {
            var x = Math.round(freq2x(freq_grid[i])) + 0.5;
            move_to(x, 0);
            line_to(x, ui_height - measure_f[1]);
            stroke();
        }
        
        // Draw gain grid
        
        for (i = 0; i < gain_grid.length; i++)
        {
            var y = Math.round(gain2y(gain_grid[i])) + 0.5;
            move_to(measure_g[0] + 2, y);
            line_to(ui_width, y);
            stroke();
        }

        // Update filter coefficients

        calc_biquad_coefficients(a_coeff, b_coeff);
//		calc_svf_coefficients(a_coeff, b_coeff);

        // Set curve width and color for the response curve
        
        set_line_width(curve_thickness);
        
        if (obj_active)
            set_source_rgba(curve_rgb);
        else
            set_source_rgba(curve_inactive_rgb);
        
        // Draw frequency response
 
        for (i = 0; i < ui_width + 1; i++) 
        {
            var db_plot = calc_biquad_db(freq_consts[i], a_coeff, b_coeff);
//			var db_plot = calc_svf_db(freq_consts[i], a_coeff, b_coeff);
            var plot_y = gain2y(db_plot);
            
            if (i == 0)
                move_to (0, plot_y);
            else
                line_to(i, plot_y);
        }
        stroke();
    
        // Set font and line width for handles
        
        select_font_face("Lato");
        set_font_size(11);
        set_line_width(handle_thickness);
        
        for (i = 0; i < num_filters; i++)
        {
            // Calculate handle position

            var x = freq2x(freq_vals[i]);
            var y = gain2y(gain_vals[i]);
            var measure = text_measure((i + 1).toString());

            // Set handle color

            if (obj_active)
                set_source_rgba(handle_rgbs[i]);
            else
                set_source_rgba(handle_inactive_rgb);
                
            if (i == handle)
            {
                // Draw selected handle

                var handle_adjust = handle_diameter + handle_thickness;
                ellipse(x - handle_adjust/2, y - handle_adjust/2, handle_adjust, handle_adjust);
                fill();

                // Render handle text
            
                set_source_rgba(handle_text_rgb);
                move_to(x - measure[0]/2, y + (measure[1]/2) * 0.65); 
                show_text((i + 1).toString());
            }
            else
            {
                // Draw non-selected handle

                ellipse(x - handle_diameter/2, y - handle_diameter/2, handle_diameter, handle_diameter);
                stroke();
            
                // Render handle text

                move_to(x - measure[0]/2, y + (measure[1]/2) * 0.65); 
                show_text((i + 1).toString());
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Mousing Interaction ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function onclick(x, y, but, cmd, shift, capslock, option, ctrl)
{
    var min_dist = 100000000;
    var i;
 
    // Assume no selection - check for qors_vals edit mode
       
    select = 0;
    qors_mode = option;
    
    // Cache mouse position for tracking delta movements
    
    last_x = x;    
    last_y = y;
    
    // Loop over handles looking for the closet valid match to select
    
    for (i = 0; i < num_filters; i++)
    {
        var compare_x = freq2x(freq_vals[i]);
        var compare_y = gain2y(gain_vals[i]);
        
        compare_x -= x;
        compare_y -= y;
        
        var compare_dist = compare_x * compare_x + compare_y * compare_y;
        
        if ((compare_dist < handle_dist) && (compare_dist < min_dist))
        {
            select = 1;
            handle = i;
        }
    }
    
    // If a handle is selected, output the display message and hide the cursor

    if (select)
    {
        outlet(0, "display", handle + 1);
        max.message("hidecursor");        
    }
}

function ondrag(x, y, but, cmd, shift, capslock, option, ctrl)
{
    // If a handle is selected and we have moved, update the values according to mousing

	//var out_gain;

    if (select && (last_x != x || last_y != y)) // !! bug was here
    {
        if (qors_mode)
        {
            var ydel = y - last_y;
            qors_vals[handle] = clip_qors(ydel2qors(qors_vals[handle], ydel));
			//gain_state = gain_vals[handle] != 0.0;			// 1 if active, 0 if inactive
            //outlet(0, "gainstate", handle + 1, gain_state);
            outlet(0, "q", handle + 1, qors_vals[handle]);
        }
        else
        {    
            freq_vals[handle] = clip_freq(x2freq(x - last_x + freq2x(freq_vals[handle])));
			gain_vals[handle] = clip_gain(y2gain(y - last_y + gain2y(gain_vals[handle])));
			//out_gain = gain_vals[handle];
			//gain_state = out_gain != 0.0;					// 1 if active, 0 if inactive
			//outlet(0, "gainstate", handle + 1, gain_state);
            outlet(0, "freq", handle + 1, freq_vals[handle]);
            outlet(0, "gain", handle + 1, gain_vals[handle]);//out_gain);
        }
    }
    
    // Deselect if the button has gone up
    
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
    // Set last position to current and call click routine to select nearest point

    last_x = x;
    last_y = y;
    
    onclick (x, y, but, cmd, shift, capslock, option, ctrl);
    
    // If a handle was selected reset the y value

    if (select)
    {
		if (cmd)
		{
			if (qors_mode)
			{
				qors_vals[handle] = qors_defaults[handle];
				outlet(0, "q", handle + 1, qors_vals[handle]);
			}
			freq_vals[handle] = freq_defaults[handle];
			outlet(0, "freq", handle + 1, freq_vals[handle]);
		}
        gain_vals[handle] = gain_defaults[handle];
        outlet(0, "gain", handle + 1, gain_vals[handle]);

		//gain_state = 0;										// 0 = inactive
		//outlet(0, "gainstate", handle + 1, gain_state);

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
    
    	fill_freq_consts();
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

