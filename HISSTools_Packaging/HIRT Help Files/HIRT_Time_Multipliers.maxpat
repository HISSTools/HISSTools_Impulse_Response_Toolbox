{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 6,
			"minor" : 0,
			"revision" : 7
		}
,
		"rect" : [ 104.0, 104.0, 762.0, 116.0 ],
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
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-7",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 323.5, 7.0, 117.0, 23.0 ],
					"text" : "Time Multipliers"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-44",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 12.0, 38.0, 740.0, 60.0 ],
					"text" : "Time multipliers are specified as ints or floats (of 1 or more) and effectively pad the inputs with zeros before deconvolution or other operations  - multipling the input length by the specified factor. This can be used to increase the size of the FFT used for processing, thus reducing time-aliasing, and increasing frequency resolution for deconvolution filters, or phase manipulations. It should be noted, however, that the size of the output will depend on the size of the FFT used, which is always rounded to the next higher power of two in samples."
				}

			}
 ],
		"lines" : [  ],
		"dependency_cache" : [  ]
	}

}
