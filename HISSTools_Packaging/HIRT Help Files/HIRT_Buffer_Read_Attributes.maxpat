{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 5,
			"minor" : 1,
			"revision" : 9
		}
,
		"rect" : [ 206.0, 127.0, 437.0, 137.0 ],
		"bglocked" : 0,
		"defrect" : [ 206.0, 127.0, 437.0, 137.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 0,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 0,
		"enablevscroll" : 0,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Buffer Read Attributes",
					"fontname" : "Arial Black",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 138.0, 10.0, 159.0, 23.0 ],
					"id" : "obj-7",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Sets the channel number for buffer reading (1-4). If the specified channel does not exist the value is wrapped within the existing number of channels (so 3 will read from channel 1 of a two-channel buffer, 4 from channel 2). ",
					"linecount" : 3,
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 13.0, 77.0, 409.0, 48.0 ],
					"id" : "obj-5",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "readchan",
					"fontname" : "Arial Black",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 13.0, 50.0, 72.0, 23.0 ],
					"id" : "obj-4",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Buffer Read Channel",
					"fontname" : "Arial",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 156.5, 50.0, 122.0, 20.0 ],
					"id" : "obj-2",
					"fontsize" : 12.0
				}

			}
 ],
		"lines" : [  ]
	}

}
