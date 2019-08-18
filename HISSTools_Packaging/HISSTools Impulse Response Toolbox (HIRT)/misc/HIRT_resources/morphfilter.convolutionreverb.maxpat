{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 0,
			"revision" : 8,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 49.0, 156.0, 837.0, 600.0 ],
		"bglocked" : 1,
		"openinpresentation" : 1,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 2,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 0,
		"lefttoolbarpinned" : 2,
		"toptoolbarpinned" : 2,
		"righttoolbarpinned" : 2,
		"bottomtoolbarpinned" : 2,
		"toolbars_unpinned_last_save" : 15,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 0,
		"enablevscroll" : 0,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "sk_bottomtoolbar_arial12",
		"title" : "morphfilter help file :: convolution reverb",
		"boxes" : [ 			{
				"box" : 				{
					"arrows" : 1,
					"id" : "obj-1",
					"justification" : 4,
					"linecolor" : [ 0.996078431372549, 0.0, 0.0, 1.0 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 651.0, 46.0, 82.0, 69.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 651.0, 46.0, 82.0, 69.0 ]
				}

			}
, 			{
				"box" : 				{
					"args" : [ "morphfilter help file", "@damp", 1, "@mod", 1 ],
					"bgmode" : 1,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-7",
					"lockeddragscroll" : 0,
					"maxclass" : "bpatcher",
					"name" : "hirt.convolutionreverb~.maxpat",
					"numinlets" : 2,
					"numoutlets" : 3,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "" ],
					"patching_rect" : [ 19.0, 75.0, 798.0, 508.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 19.0, 75.0, 798.0, 508.0 ],
					"varname" : "hirt.convolutionreverb~",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 502.0, 478.0, 67.0, 22.0 ],
					"save" : [ "#N", "thispatcher", ";", "#Q", "end", ";" ],
					"text" : "thispatcher"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "wclose" ],
					"patching_rect" : [ 502.0, 454.0, 52.0, 22.0 ],
					"text" : "t wclose"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "bang", "bang" ],
					"patching_rect" : [ 502.0, 430.0, 65.0, 22.0 ],
					"text" : "onebang 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 502.0, 406.0, 144.0, 22.0 ],
					"text" : "r globalclose_mp_helpfile"
				}

			}
, 			{
				"box" : 				{
					"background" : 1,
					"fontname" : "Lato",
					"fontsize" : 13.0,
					"id" : "obj-4",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 660.0, 16.0, 157.0, 38.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 660.0, 16.0, 157.0, 38.0 ],
					"text" : "select \"Damp\" tab for morphfilter UI example"
				}

			}
, 			{
				"box" : 				{
					"background" : 1,
					"fontname" : "Lato",
					"fontsize" : 13.0,
					"hidden" : 1,
					"id" : "obj-3",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 32.0, 367.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 19.0, 32.0, 367.0, 22.0 ],
					"text" : "[hirt.convolutionreverb~] for [morphfilter] helpfile"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-15", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"source" : [ "obj-5", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-7::obj-24::obj-1::obj-29::obj-70" : [ "hirt.val[16]", "hirt.val", 0 ],
			"obj-7::obj-10" : [ "Damp Active", "Active", 0 ],
			"obj-7::obj-31::obj-53::obj-70" : [ "hirt.val[23]", "hirt.val", 0 ],
			"obj-7::obj-5" : [ "Realtime Tabs", "Realtime", 0 ],
			"obj-7::obj-31::obj-22::obj-70" : [ "hirt.val[26]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-47::obj-70" : [ "hirt.val[2]", "hirt.val", 0 ],
			"obj-7::obj-3::obj-62::obj-70" : [ "hirt.val[29]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-36::obj-70" : [ "hirt.val[8]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-49" : [ "Length Mode", "Length Mode", 0 ],
			"obj-7::obj-3::obj-63::obj-70" : [ "hirt.val[30]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-45::obj-70" : [ "hirt.val[4]", "hirt.val", 0 ],
			"obj-7::obj-8" : [ "Shape Active", "Active", 0 ],
			"obj-7::obj-36::obj-114::obj-65" : [ "Drop B", "live.drop", 0 ],
			"obj-7::obj-24::obj-2::obj-29::obj-70" : [ "hirt.val[18]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-100::obj-70" : [ "hirt.val[13]", "hirt.val", 0 ],
			"obj-7::obj-31::obj-56::obj-70" : [ "hirt.val[20]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-79::obj-70" : [ "hirt.val[14]", "hirt.val", 0 ],
			"obj-7::obj-36::obj-131::obj-10" : [ "Category Menu", "Category Menu", 0 ],
			"obj-7::obj-3::obj-59::obj-70" : [ "hirt.val[33]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-92::obj-70" : [ "hirt.val[15]", "hirt.val", 0 ],
			"obj-7::obj-24::obj-2::obj-49" : [ "Type", "Type", 0 ],
			"obj-7::obj-31::obj-54::obj-70" : [ "hirt.val[22]", "hirt.val", 0 ],
			"obj-7::obj-11" : [ "Modulation Active", "Active", 0 ],
			"obj-7::obj-1::obj-1::obj-53" : [ "Direct Mode", "Direct Mode", 0 ],
			"obj-7::obj-31::obj-50::obj-70" : [ "hirt.val[25]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-48::obj-70" : [ "hirt.val[1]", "hirt.val", 0 ],
			"obj-7::obj-31::obj-19::obj-70" : [ "hirt.val[27]", "hirt.val", 0 ],
			"obj-7::obj-31::obj-21" : [ "EQ Routing", "Active", 0 ],
			"obj-7::obj-1::obj-2::obj-99::obj-70" : [ "hirt.val[7]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-101::obj-70" : [ "hirt.val[10]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-81::obj-70" : [ "hirt.val[11]", "hirt.val", 0 ],
			"obj-7::obj-3::obj-65::obj-70" : [ "hirt.val[32]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-96::obj-70" : [ "hirt.val[12]", "hirt.val", 0 ],
			"obj-7::obj-24::obj-1::obj-28::obj-70" : [ "hirt.val[17]", "hirt.val", 0 ],
			"obj-7::obj-31::obj-51::obj-70" : [ "hirt.val[24]", "hirt.val", 0 ],
			"obj-7::obj-31::obj-17::obj-70" : [ "hirt.val[28]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-2::obj-88::obj-70" : [ "hirt.val[9]", "hirt.val", 0 ],
			"obj-7::obj-24::obj-2::obj-8::obj-70" : [ "hirt.val[19]", "hirt.val", 0 ],
			"obj-7::obj-28" : [ "PATCH/PRESS", "PATCH/PRESS", 0 ],
			"obj-7::obj-24::obj-2::obj-25" : [ "Widen", "Widen", 0 ],
			"obj-7::obj-1::obj-1::obj-46::obj-70" : [ "hirt.val[3]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-9" : [ "Reverse", "Reverse", 0 ],
			"obj-7::obj-36::obj-53" : [ "Mode IR", "Mode", 0 ],
			"obj-7::obj-3::obj-64::obj-70" : [ "hirt.val[31]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-44::obj-70" : [ "hirt.val[5]", "hirt.val", 0 ],
			"obj-7::obj-36::obj-131::obj-11" : [ "IR Menu", "IR Menu", 0 ],
			"obj-7::obj-108::obj-90" : [ "number", "number", 0 ],
			"obj-7::obj-79" : [ "Offline Tabs", "Offline", 0 ],
			"obj-7::obj-1::obj-1::obj-12::obj-70" : [ "hirt.val[6]", "hirt.val", 0 ],
			"obj-7::obj-14" : [ "Position Active", "Active", 0 ],
			"obj-7::obj-1::obj-1::obj-97::obj-70" : [ "hirt.val", "hirt.val", 0 ],
			"obj-7::obj-36::obj-114::obj-35" : [ "Drop A", "live.drop", 0 ],
			"obj-7::obj-36::obj-49" : [ "Select IR", "Select", 0 ],
			"obj-7::obj-31::obj-55::obj-70" : [ "hirt.val[21]", "hirt.val", 0 ],
			"parameterbanks" : 			{

			}
,
			"parameter_overrides" : 			{
				"obj-7::obj-24::obj-1::obj-29::obj-70" : 				{
					"parameter_longname" : "hirt.val[16]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 6,
					"parameter_initial" : 0.0,
					"parameter_range" : [ -50.0, 50.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-31::obj-53::obj-70" : 				{
					"parameter_longname" : "hirt.val[23]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 1000.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-7::obj-31::obj-22::obj-70" : 				{
					"parameter_longname" : "hirt.val[26]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 125.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-1::obj-47::obj-70" : 				{
					"parameter_longname" : "hirt.val[2]",
					"parameter_invisible" : 1,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-3::obj-62::obj-70" : 				{
					"parameter_longname" : "hirt.val[29]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 0.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-2::obj-36::obj-70" : 				{
					"parameter_longname" : "hirt.val[8]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.189901,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 20.0, 200.0 ]
				}
,
				"obj-7::obj-3::obj-63::obj-70" : 				{
					"parameter_longname" : "hirt.val[30]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-1::obj-45::obj-70" : 				{
					"parameter_longname" : "hirt.val[4]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 2.0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 2,
					"parameter_initial" : 0.0,
					"parameter_range" : [ 0.0, 1000.0 ]
				}
,
				"obj-7::obj-24::obj-2::obj-29::obj-70" : 				{
					"parameter_longname" : "hirt.val[18]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 1.0,
					"parameter_range" : [ 0.05, 2.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-2::obj-100::obj-70" : 				{
					"parameter_longname" : "hirt.val[13]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 4.0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 1000.0,
					"parameter_range" : [ 30.0, 18000.0 ]
				}
,
				"obj-7::obj-31::obj-56::obj-70" : 				{
					"parameter_longname" : "hirt.val[20]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 8000.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-2::obj-79::obj-70" : 				{
					"parameter_longname" : "hirt.val[14]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.189901,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 20.0, 200.0 ]
				}
,
				"obj-7::obj-3::obj-59::obj-70" : 				{
					"parameter_longname" : "hirt.val[33]",
					"parameter_exponent" : 2.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 2,
					"parameter_initial" : 0.0,
					"parameter_range" : [ 0.0, 200.0 ]
				}
,
				"obj-7::obj-1::obj-2::obj-92::obj-70" : 				{
					"parameter_longname" : "hirt.val[15]",
					"parameter_invisible" : 1,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 1,
					"parameter_initial" : 0.707107,
					"parameter_range" : [ 0.05, 2.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-31::obj-54::obj-70" : 				{
					"parameter_longname" : "hirt.val[22]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.5,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-31::obj-50::obj-70" : 				{
					"parameter_longname" : "hirt.val[25]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.707107,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-1::obj-48::obj-70" : 				{
					"parameter_longname" : "hirt.val[1]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.58,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 50.0, 200.0 ]
				}
,
				"obj-7::obj-31::obj-19::obj-70" : 				{
					"parameter_longname" : "hirt.val[27]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 4,
					"parameter_units" : " ",
					"parameter_initial" : 0.0,
					"parameter_range" : [ -18.0, 18.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-2::obj-99::obj-70" : 				{
					"parameter_longname" : "hirt.val[7]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 4.0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 125.0,
					"parameter_range" : [ 30.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-2::obj-101::obj-70" : 				{
					"parameter_longname" : "hirt.val[10]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 4.0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 8000.0,
					"parameter_range" : [ 30.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-2::obj-81::obj-70" : 				{
					"parameter_longname" : "hirt.val[11]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.189901,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 20.0, 200.0 ]
				}
,
				"obj-7::obj-3::obj-65::obj-70" : 				{
					"parameter_longname" : "hirt.val[32]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 50.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-2::obj-96::obj-70" : 				{
					"parameter_longname" : "hirt.val[12]",
					"parameter_invisible" : 1,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 1,
					"parameter_initial" : 1.0,
					"parameter_range" : [ 0.05, 2.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-24::obj-1::obj-28::obj-70" : 				{
					"parameter_longname" : "hirt.val[17]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 0.0,
					"parameter_range" : [ -100.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-31::obj-51::obj-70" : 				{
					"parameter_longname" : "hirt.val[24]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 4,
					"parameter_units" : " ",
					"parameter_initial" : 0.0,
					"parameter_range" : [ -18.0, 18.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-31::obj-17::obj-70" : 				{
					"parameter_longname" : "hirt.val[28]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.5,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-2::obj-88::obj-70" : 				{
					"parameter_longname" : "hirt.val[9]",
					"parameter_invisible" : 1,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 1,
					"parameter_initial" : 1.0,
					"parameter_range" : [ 0.05, 2.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-24::obj-2::obj-8::obj-70" : 				{
					"parameter_longname" : "hirt.val[19]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 50.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-1::obj-46::obj-70" : 				{
					"parameter_longname" : "hirt.val[3]",
					"parameter_invisible" : 1,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-3::obj-64::obj-70" : 				{
					"parameter_longname" : "hirt.val[31]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 4,
					"parameter_units" : " ",
					"parameter_initial" : 0.0,
					"parameter_range" : [ -20.0, 20.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-1::obj-44::obj-70" : 				{
					"parameter_longname" : "hirt.val[5]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 2.0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 2,
					"parameter_initial" : 0.0,
					"parameter_range" : [ 0.0, 1000.0 ]
				}
,
				"obj-7::obj-1::obj-1::obj-12::obj-70" : 				{
					"parameter_longname" : "hirt.val[6]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 2.5,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 2,
					"parameter_initial" : 15000.0,
					"parameter_range" : [ 100.0, 30000.0 ]
				}
,
				"obj-7::obj-1::obj-1::obj-97::obj-70" : 				{
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.01,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 1.0, 200.0 ]
				}
,
				"obj-7::obj-31::obj-55::obj-70" : 				{
					"parameter_longname" : "hirt.val[21]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 4,
					"parameter_units" : " ",
					"parameter_initial" : 0.0,
					"parameter_range" : [ -18.0, 18.0 ],
					"parameter_exponent" : 1.0
				}

			}

		}
,
		"dependency_cache" : [ 			{
				"name" : "hirt.convolutionreverb~.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb",
				"patcherrelativepath" : "../../patchers/HIRT_reverb",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tabs_offline.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tab_shape.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.dial.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.dial.linear.only.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tab_damp.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_interface_damp.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/jsui",
				"patcherrelativepath" : "../../jsui",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tabs_realtime.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tab_position.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_interface_pos.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/jsui",
				"patcherrelativepath" : "../../jsui",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tab_modulation.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_interface_mod.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/jsui",
				"patcherrelativepath" : "../../jsui",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_interface_eq.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/jsui",
				"patcherrelativepath" : "../../jsui",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_ir.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_loading.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_loading_scheme.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_set.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_check.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_base_name.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_folder.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_data_colls.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_ir_display.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_filter_type.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_picker.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_cr_info_view.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_output.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convrvrb_clientlist_alias.txt",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "HIRT_HISSTools_Logo.png",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/misc/HIRT_image",
				"patcherrelativepath" : "../HIRT_image",
				"type" : "PNG",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.convrvrb.realtime~.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.convolvestereo~.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convrvrb_rt_part1.gendsp",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "gDSP",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convrvrb_rt_part3.gendsp",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "gDSP",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convrvrb_rt_part5.gendsp",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "gDSP",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_gain_params.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.svfcoeff.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_zoom_factor.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/javascript",
				"patcherrelativepath" : "../../javascript",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.damping.cascade.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.buffer.filter.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_partition_late.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_partition_fix_length.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_nan_fix.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_partition_early.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_partition_copy_buffers.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_gain_and_display.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.size.resample.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_size_resample_feed.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "spectrumdraw~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "iraverage~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "irdisplay~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "irtrimnorm~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "multiconvolve~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "morphfilter~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "irstats~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "irmix~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "ircropfade~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "bufreverse~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "bufresample~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0,
		"toolbaradditions" : [ "audiomute", "audiosolo" ]
	}

}
