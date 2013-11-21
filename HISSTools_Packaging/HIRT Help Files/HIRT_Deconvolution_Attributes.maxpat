{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 6,
			"minor" : 1,
			"revision" : 4,
			"architecture" : "x86"
		}
,
		"rect" : [ 773.0, 181.0, 520.0, 845.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"statusbarvisible" : 0,
		"toolbarvisible" : 0,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 0,
		"enablevscroll" : 0,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-22",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 347.0, 490.0, 74.0 ],
					"text" : "Additionally you may specify the name of a buffer as the filter specification in which case the power spectrum of the buffer  is used to determine the specification for 'Regularisation' and 'Clipping' modes, and the buffer is applied directly as a convolution filter in 'Filter' mode (ignoring the deconvphase setting in this case). This allows a high degree of control of the process for special applications or custom solutions."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-19",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 123.5, 123.0, 384.5, 47.0 ],
					"text" : "- Regularise the division by adding a constant to the division\n- Clip the denominator in the division\n- Specify the filter explicitly"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Italic",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-20",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 123.0, 96.0, 47.0 ],
					"text" : "Regularisation\nClipping\nFilter"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-17",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 124.0, 654.0, 108.0, 47.0 ],
					"text" : "- Minimum Phase\n- Linear Phase\n- Maximum Phase"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Italic",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-18",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 654.0, 96.0, 47.0 ],
					"text" : "min / minimum \nlin / linear\nmax / maximum"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-14",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 452.0, 489.0, 74.0 ],
					"text" : "When deconvmode is 'Clipping' the denominator of the divsion can be clipped to both minimum values (controlling peaks in the output) and also maximum values (controlling dips in the output). The deconvrange attibute sets the maximum values in relation to the filter specification. Similarly to the deconvolution filter specification, you may either specify a single value in dB or use a list of freq/dB pairs to vary the range with frequency."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-15",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 425.0, 95.0, 23.0 ],
					"text" : "deconvrange"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-16",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 186.0, 425.0, 155.0, 20.0 ],
					"text" : "Deconvolution Filter Range"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-11",
					"linecount" : 6,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 561.0, 492.0, 87.0 ],
					"text" : "The method by which deconvolution is controlled acts as a FIR filter on result of the 'true deconvolution'. This filter will produce time smearing of the result. This can result in pre or post ring in the resultant IR. By controlling the phase of the implicit filter we can control this ring. The default is minimum phase which post-rings only. The phase may either be specified as a float value between 0 and 1 (where 0 is minimum phase, 0.5 is linear phase and 1 is maximum phase), or using any of the following symbols:"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-12",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 534.0, 95.0, 23.0 ],
					"text" : "deconvphase"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-13",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 186.5, 534.0, 154.0, 20.0 ],
					"text" : "Deconvolution Filter Phase"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-8",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 73.0, 489.0, 47.0 ],
					"text" : "Deconvolution must be controlled so as to prevent filter blow-up or excessive time-aliasing. The control is equivalent to filtering the result of a staightforward complex division in the frequency domain (hence the term filter). The three modes are:"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-9",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 46.0, 95.0, 23.0 ],
					"text" : "deconvmode"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-10",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 188.5, 46.0, 150.0, 20.0 ],
					"text" : "Deconvolution Filter Mode"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-1",
					"linecount" : 10,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 202.0, 491.0, 141.0 ],
					"text" : "Regardless of the deconvolution mode it is necessary to set a specification for how to control the deconvolution over the frequency range. The filter specification sets a frequency curve to apply to the relevant mode. You may either specify a static value in dB, or a list of freq/dB values to vary the specification with frequency. For 'Regularisation' this sets the constant value to add to the divisor. For 'Clipping' it sets the minimum values allowed for the division. For 'Filter' it sets the shape of the filter. Typically, for standard usage, very low or high frequencies are liable to blowup through overcorrection or numeric error. Thus higher values for 'Regularisation' and 'clipping areappropraite at low/high frequencies. The 'Filter' mode works in the inverse, and lower values (less gain) is generally more appropriate for extremes of frequency."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-3",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 175.0, 109.0, 23.0 ],
					"text" : "deconvfilter"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-6",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 169.5, 175.0, 188.0, 20.0 ],
					"text" : "Deconvolution Filter Specification"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-7",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 176.5, 10.0, 174.0, 23.0 ],
					"text" : "Deconvolution Attributes"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-5",
					"linecount" : 7,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 735.0, 488.0, 100.0 ],
					"text" : "Deconvolution can result in time-aliasing. In order to make sure that the resultant IR is correctly contained within the period of the FFT, it may be necessary to introduce a modelling delay (shifting the output of the deconvolution). This delay can either be set in milliseconds or using the symbol 'center' which sets the delay to half of the FFT size. This attribute is not available in the irmeasure~/irextract~ objects, as the timing is tightly controlled by the synchronous recording/playback process, making a modelling delay unnecessary."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-4",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 708.0, 95.0, 23.0 ],
					"text" : "deconvdelay"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-2",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 176.0, 708.0, 175.0, 20.0 ],
					"text" : "Deconvolution Modelling Delay"
				}

			}
 ],
		"lines" : [  ],
		"dependency_cache" : [  ]
	}

}
