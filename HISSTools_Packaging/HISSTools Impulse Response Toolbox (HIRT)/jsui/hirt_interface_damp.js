
outlets = 2;

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Object Variables ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// Colors

var background_rgb = [0, 0, 0, 0];
var curve_rgb = [0.82, 0.21, 0.15, 1.0];
var curve_uncalculated_rgb = [0.8, 0.8, 0.8, 1];
var curve_inactive_rgb = [0.65, 0.65, 0.67, 1.0];
var handle_rgbs = [[0.3, 0.25, 0.33, 1.0], [0.28, 0.41, 0.14, 1.0], [0.95, 0.75, 0.34, 1.0]];
var handle_inactive_rgb = [0.8, 0.8, 0.8, 1];
var handle_text_rgb = [0.094, 0.118, 0.137, 1.0];
var marker_rgb = [0.38, 0.38, 0.4, 0.8];
var label_rgb = [1.0, 1.0, 1.0, 0.66];//[0.094, 0.118, 0.137, 0.8];	// dynamic for theme

// Grid and Labels

var freq_grid = [125, 1000, 8000];
var freq_labels = [125, "125", 1000, "1K", 8000, "8K"];
var freq_label_point = -4;//0

var damp_grid = [0, 100, 200];
var damp_labels = [0, 100, 200];
var damp_label_point = 31;//35

// Drawing Dimensions

var curve_thickness = 2;
var marker_thickness = 1;
var handle_thickness = 2;
var handle_diameter = 14;

// Display and Value Ranges

var freq_lo = 20;
var freq_hi = 22050;
var freq_clip_lo = 30;
var freq_clip_hi = 18000;

var damp_lo = -30;
var damp_hi = 230;
var damp_clip_lo = 20;
var damp_clip_hi = 200;

var qors_clip_lo = 0.05;
var qors_clip_hi = 2;

// Filter Data

var num_filters = 3;
var filter_types = [3, 2, 4];
var freq_defaults = [125, 1000, 8000];
var freq_vals = [125, 1000, 8000];
var damp_defaults = [100, 100, 100];
var damp_vals = [100, 100, 100];
var qors_defaults = [1.0, 0.707107, 1.0];
var qors_vals = [1.0, 0.707107, 1.0];

// Interaction Variables

var obj_active = 0;
var qors_scale = 10;

// Mousing State Variables

var qors_mode = 0;
var handle = 0;     
var select = 0;
var drag = 0;
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


function x2freq (x)
{
    return Math.exp((x / ui_width) * (log_freq_hi - log_freq_lo) + log_freq_lo);
}

function freq2x (freq)
{
    return ui_width * (Math.log(freq) - log_freq_lo) / (log_freq_hi - log_freq_lo);
}

function damp2y(damp)
{
    return ui_height - (ui_height * ((damp - damp_lo) / (damp_hi - damp_lo)));
}

function y2damp(y)
{
    return ((ui_height - y) / ui_height) * (damp_hi - damp_lo) + damp_lo;
}

function ydel2qors(prev_qors, ydiff)
{ 
    return prev_qors *  Math.exp((mod_scale * (ydiff + qors_scale) /  (2 * qors_scale)) + mod_lo);
}


// Set all to local

x2freq.local = 1;
freq2x.local = 1;
damp2y.local = 1;
y2damp.local = 1;
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

function clip_damp(damp)
{
    damp = damp < damp_clip_lo ? damp_clip_lo : damp;
    damp = damp > damp_clip_hi ? damp_clip_hi : damp;
    
    return damp;
}

function clip_q(q)
{
    q = q < qors_clip_lo ? qors_clip_lo : q;
    q = q > qors_clip_hi ? qors_clip_hi : q;
    
    return q;
}


// Set all to local

clip_freq.local = 1;
clip_damp.local = 1;
clip_q.local = 1;


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

function setdecay(filter, gain)
{
    if (filter > 0 & filter <= num_filters)
    {
        damp_vals[filter - 1] = clip_damp(gain);
        handle = filter - 1;
    }
        
    mgraphics.redraw();
}

function setq(filter, q)
{
    if (filter > 0 & filter <= num_filters)
    {
        qors_vals[filter - 1] = clip_q(q);
        handle = filter - 1;
    }
        
    mgraphics.redraw();
}

function settype(filter, type)
{
    type = type < 0 ? 0 : type;
    type = type > 4 ? 4 : type;
    
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
////////////////////////////// Filter and Curve Calculations //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function calc_filter_coefficients(a_coeff, b_coeff)
{
    var alpha, alphashelf;
    var a0, a1, a2, b0, b1, b2;
    var i;
    
    // For each filter calculate the biquad coeffiecients

    for (i = 0; i < num_filters; i++)
    {
        var w0 = 2.0 * Math.PI * freq_vals[i] / sample_rate;
        var cosw = Math.cos(w0);
        var sinw = Math.sin(w0);
        var A = Math.sqrt(damp_vals[i] / 100);
        var qs = qors_vals[i];
        
        switch (filter_types[i])
        {
            case 0: // LO SHELF COOKBOOK
            case 3: // LO SHELF MAX
            
                if (filter_types[i] == 0)
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
            
            case 1: // HI SHELF COOKBOOK
            case 4: // HI SHELF MAX
            
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
                    
                a0 =              (A + 1.0) - (A - 1.0) * cosw + alphashelf;
                a1 =      (2.0 * ((A - 1.0) - (A + 1.0) * cosw)) / a0;
                a2 =             ((A + 1.0) - (A - 1.0) * cosw - alphashelf) / a0;
                b0 =        (A * ((A + 1.0) + (A - 1.0) * cosw + alphashelf)) / a0;
                b1 = (-2.0 * A * ((A - 1.0) + (A + 1.0) * cosw)) / a0;
                b2 =        (A * ((A + 1.0) + (A - 1.0) * cosw - alphashelf)) / a0;
                break;
                
            case 2: // PEAKING COOKBOOK
            
                alpha = sinw / (2.0 * qs);
                    
                a0 =   1.0 + alpha / A;
                a1 = (-2.0 * cosw) / a0;
                a2 =  (1.0 - alpha / A) / a0;    
                b0 =  (1.0 + alpha * A) / a0;
                b1 = (-2.0 * cosw) / a0;
                b2 =  (1.0 - alpha * A) / a0;
                break;
        }
        
        a_coeff.push(1);
        a_coeff.push(a1);
        a_coeff.push(a2);
        b_coeff.push(b0);
        b_coeff.push(b1);
        b_coeff.push(b2);
    }
}

function calc_amp(freq, a_coeff, b_coeff)
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
      
    // Return the linear amplitude response
      
    return Math.sqrt(pow);
}


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Paint Routine //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


function paint()
{        
    var a_coeff = new Array;
    var b_coeff = new Array;
	var measure_f;
	var measure_d;
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

        var freq_label_line = damp2y(freq_label_point);
                
        for (i = 0; i < freq_labels.length / 2; i++)
        {
            measure_f = text_measure((i + 1).toString());    
//			move_to(freq2x(freq_labels[i * 2]) + 2, freq_label_line + 2 + measure_f[1] * 0.65); 
			move_to(freq2x(freq_labels[i * 2]) - measure_f[0], freq_label_line + 4 + measure_f[1] * 0.65); 
            show_text(freq_labels[i * 2 + 1]);
        }
        
        // Render damping labels
        
        var damp_label_line = freq2x(damp_label_point);
             
        for (i = 0; i < damp_grid.length; i++)
        {
            measure_g = text_measure(damp_labels[i].toString());                
            move_to(damp_label_line - measure_g[0] - 2, damp2y(damp_grid[i]) + (measure_g[1]/2) * 0.75); 
            show_text(damp_labels[i].toString());
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
        
        // Draw damping grid
        
        for (i = 0; i < damp_grid.length; i++)
        {
            var y = Math.round(damp2y(damp_grid[i])) + 0.5;
            move_to(measure_g[0] + 4, y);
            line_to(ui_width, y);
            stroke();
        }
        
        // Update filter coefficients
        
        calc_filter_coefficients(a_coeff, b_coeff);
        
        // Set curve width and color for the response curve
            
        set_line_width(curve_thickness);
        
        if (select && drag) 
            set_source_rgba(curve_uncalculated_rgb);
        else
        {
            if (obj_active)
                set_source_rgba(curve_rgb);
            else
                set_source_rgba(curve_inactive_rgb);
        }
            
        // Draw frequency response
        
        for (i = 0; i < ui_width + 1; i++) 
        {
            var damp_plot = calc_amp(freq_consts[i], a_coeff, b_coeff);
            var plot_y = damp2y(damp_plot * 100);
            
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
            var y = damp2y(damp_vals[i]);
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
    
     // Assume no selection - check for q edit mode
       
    select = 0;
    qors_mode = option;
    
    // Cache mouse position for tracking delta movements
    
    last_x = x;    
    last_y = y;
    
    // Loop over handles looking for the closet valid match to select

    for (i = 0; i < num_filters; i++)
    {
        var compare_x = freq2x(freq_vals[i]);
        var compare_y = damp2y(damp_vals[i]);
        
        compare_x -= x;
        compare_y -= y;
        
        var compare_dist = compare_x * compare_x + compare_y * compare_y;
        
        if ((compare_dist < handle_dist) && (compare_dist < min_dist))
        {
            select = 1;
            handle = i;
            drag = 0;
        }
    }
    
    // If a handle is selected, output edit state / the display message / hide the cursor
    
    if (select)
    {
        outlet(1, 0);
        outlet(0, "display", handle + 1);
        max.message("hidecursor");
    }
}

function ondrag(x, y, but, cmd, shift, capslock, option, ctrl)
{        
    // If a handle is selected and we have moved, update the values according to mousing
    
    if (select && (last_x != x || last_y != y))
    {
        drag = 1;
        if (qors_mode)
        {
            var ydiff = y - last_y;
            qors_vals[handle] = clip_q(ydel2qors(qors_vals[handle], ydiff));
            outlet(0, "q", handle + 1, qors_vals[handle]);
        }
        else
        {    
            freq_vals[handle] = clip_freq(x2freq(x - last_x + freq2x(freq_vals[handle])));
            damp_vals[handle] = clip_damp(y2damp(y - last_y + damp2y(damp_vals[handle])));
            outlet(0, "freq", handle + 1, freq_vals[handle]);
            outlet(0, "gain", handle + 1, damp_vals[handle]);
        }
    }
    
    // Deselect the handle if the button has gone up

    if (!but)
    {
        select = 0;
        drag = 0;
        outlet(1, 1);
        max.message("showcursor");
    }
    
    // Store current position and redraw

    last_x = x;
    last_y = y;
    mgraphics.redraw();
}

function ondblclick(x, y, but, cmd, shift, capslock, option, ctrl)
{
    // Set last position to current and call click routine to select nearest handle
    
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
        damp_vals[handle] = 100.;
        outlet(0, "gain", handle + 1, damp_vals[handle]);
    }
    
    // Deselect the handle
    
    select = 0;
    drag = 0;
    max.message("showcursor");
    outlet(1, 1);
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