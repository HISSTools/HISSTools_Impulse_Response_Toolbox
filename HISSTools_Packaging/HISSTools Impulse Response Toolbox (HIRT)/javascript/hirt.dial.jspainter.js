
// constants

const FAKE_BGCOLOR = [ 0.0, 0.0, 0.0, 0.0 ];		// bgcolor fixed alpha zero for hirt (real 'bgcolor' used for 'inactive' colour)
//const ANGLESWITCH = 57.29577951289617;			// "sin & cos switcheroo because centered around top" (jkc)
const DEGREESDIV = 0.017501908933648;				// twopi / (360 - 1)

// main

function paint()
{
	// attributes
	var this_degrees = box.getattr("degrees");
	var this_thickness = box.getattr("thickness");
	var fake_drawingstyle = box.getattr("mode");	// hack: we use these for our own styles
	var fake_inactivecolor = box.getattr("bgcolor");// hack: we use this as our 'inactive' colour
	var this_needlecolor = box.getattr("needlecolor");
	var this_outlinecolor = box.getattr("outlinecolor");
	var fake_active = box.getattr("clip");			// hack: we use 'clip' attribute as a fake 'active' control
	var this_size = box.getattr("size");
	var this_min = box.getattr("min");
//	var this_mult = box.getattr("mult");
	var this_floatoutput = box.getattr("floatoutput");

	// dial value
	var this_val = box.getvalueof();
//	var this_val = ((this_type != null) && (this_type != 2)) ? (box.getvalueof() - this_range[0]) : (parseFloat(box.getvalueof()));

	// box
	var box_viewsize = mgraphics.size;
	var box_width = box_viewsize[0];
	var box_height = box_viewsize[1];

	// local
	var max_dial_xc = box_width * 0.5;
	var max_dial_yc = box_height * 0.5;
	var max_dial_radius = box_width * 0.375;
	var draw_start, draw_center, draw_end, draw_realtime;
	var val_range, val_norm, val_center;//, val_angle;
	var fake_centercolor, fake_needlecolor;

	// hack: use 'clip' attribute as a fake 'active' control for colours, redraw() when changed
	if (fake_active > 0) {
		fake_centercolor = this_needlecolor;	// or 'fake_inactivecolor' here ??
		fake_needlecolor = this_needlecolor;
		ForceRedraw();
	} else {
		fake_centercolor = FAKE_BGCOLOR;
		fake_needlecolor = fake_inactivecolor;
		ForceRedraw();
	}

	// static drawing points global
	draw_start = (270 - this_degrees * 0.5) * DEGREESDIV;
	draw_center = 270 * DEGREESDIV;
	draw_end = (270 + this_degrees * 0.5) * DEGREESDIV;

	draw_realtime = draw_end;	// initialise

	// value range global
	val_range = this_floatoutput ? this_size : maximum((this_size - 1), 2);	// seems stupid, but is exactly what max [dial] does natively
	val_range = (val_range <= 1) ? 1 : val_range;
	val_norm = this_val / val_range;

	// background global
	mgraphics.set_source_rgba(FAKE_BGCOLOR);
	mgraphics.rectangle(0, 0, box_width, box_height);
	mgraphics.fill();

	// hack: we use 'fake_drawingstyle' for our own drawing styles, not the native ones represented by the 'mode' attribute
	if (fake_drawingstyle == 2) {				// 2 = bipolar pan special

		// draw special center point outline arc (and line width)
		mgraphics.set_source_rgba(this_outlinecolor);
		mgraphics.set_line_width(box_width * 0.1 * (this_thickness * 0.01));
		mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_start, draw_center * 0.98);
		mgraphics.stroke();
		mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_center * 1.01, draw_end);
		mgraphics.stroke();
		// (optional) fill gap if 'active' == 1
		mgraphics.set_source_rgba(fake_centercolor);
		mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_center * 0.98, draw_center * 1.01);
		mgraphics.stroke();

		val_center = val_range * 0.5;

		draw_realtime -= this_degrees * DEGREESDIV * (1 - val_norm);

		mgraphics.set_source_rgba(fake_needlecolor);

		if (this_val > val_center) {

			mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_realtime, draw_end);
			mgraphics.stroke();

		} else if (this_val < val_center) {

			mgraphics.arc_negative(max_dial_xc, max_dial_yc, max_dial_radius, draw_realtime, draw_start);
			mgraphics.stroke();

		} else {	// (this_val == val_center), draw static place holder

			mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_center * 0.98, draw_center * 1.01);
			mgraphics.stroke();

		}

	} else {

		// full outline arc for all other drawing styles
		mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_start, draw_end);
		mgraphics.set_line_width(box_width * 0.1 * (this_thickness * 0.01));
		mgraphics.set_source_rgba(this_outlinecolor);
		mgraphics.stroke();

		if (fake_drawingstyle == 1) {			// 1 = bipolar around center

			val_center = val_range * 0.5;

			draw_realtime -= this_degrees * DEGREESDIV * (1 - val_norm);

			mgraphics.set_source_rgba(fake_needlecolor);

			if (this_val > val_center) {

				mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_center * 0.99, draw_realtime);
				mgraphics.stroke();

			} else if (this_val < val_center) {

				mgraphics.arc_negative(max_dial_xc, max_dial_yc, max_dial_radius, draw_center, draw_realtime);
				mgraphics.stroke();

			} else {	// (this_val == val_center), draw static place holder

				mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_center * 0.98, draw_center * 1.01);
				mgraphics.stroke();

			}

		} else if (fake_drawingstyle == 3) {	// 3 = reverse linear special	// !! Max8 only !!

			draw_realtime -= this_degrees * DEGREESDIV * val_norm;

			mgraphics.set_source_rgba(fake_needlecolor);
			mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_start, draw_realtime);
			mgraphics.stroke();

		} else {								// 0 (& 4+ in Max8+) = standard linear dial

			draw_realtime -= this_degrees * DEGREESDIV * (1 - val_norm);

			mgraphics.set_source_rgba(fake_needlecolor);
			mgraphics.arc(max_dial_xc, max_dial_yc, max_dial_radius, draw_start, draw_realtime);
			mgraphics.stroke();

		}

	}

}
/*
function DialValToAngle(val, valrange, floatoutput, degrees)
{
	var res, m;

	m = floatoutput ? valrange : (valrange - 1);
	res = (val * (degrees - 1)) / m;

	return res;
}

function DialPointFromAngleAmp(centerx, centery, amp, angle, degrees)
{
	var half = (degrees - 1) * 0.25;
	var loc = {x:0, y:0};
	var pt = {x:0, y:0};

	if (angle < half)
		angle += (360 - half);	// !!
	else
		angle -= half;

	loc.x = amp * Math.sin(angle / ANGLESWITCH);
	loc.y = amp * Math.cos(angle / ANGLESWITCH);

	pt.x = centerx + loc.x;
	pt.y = centery - loc.y;

	return pt;
}
*/
function ForceRedraw()
{
	mgraphics.redraw();
}

