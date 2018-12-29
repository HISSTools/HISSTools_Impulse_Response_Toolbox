{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 7,
			"minor" : 3,
			"revision" : 5,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"rect" : [ 63.0, 103.0, 720.0, 360.0 ],
		"bglocked" : 0,
		"openinpresentation" : 1,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 2,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 100,
		"enablehscroll" : 0,
		"enablevscroll" : 0,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "PeteSubDefault10",
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 217.0, 120.0, 69.0, 22.0 ],
					"save" : [ "#N", "thispatcher", ";", "#Q", "end", ";" ],
					"style" : "",
					"text" : "thispatcher"
				}

			}
, 			{
				"box" : 				{
					"comment" : "",
					"id" : "obj-5",
					"index" : 0,
					"maxclass" : "outlet",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 34.0, 296.0, 30.0, 30.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"comment" : "",
					"id" : "obj-4",
					"index" : 0,
					"maxclass" : "inlet",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 34.0, 34.0, 30.0, 30.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 34.0, 81.0, 202.0, 22.0 ],
					"style" : "",
					"text" : "routepass theme shape damp"
				}

			}
, 			{
				"box" : 				{
					"args" : [ "#1" ],
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-2",
					"lockeddragscroll" : 0,
					"maxclass" : "bpatcher",
					"name" : "hirt_tab_damp.maxpat",
					"numinlets" : 1,
					"numoutlets" : 1,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "" ],
					"patching_rect" : [ 156.0, 159.0, 105.0, 105.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 360.0, 0.0, 360.0, 201.0 ],
					"varname" : "Damp",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"args" : [ "#1" ],
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-1",
					"lockeddragscroll" : 0,
					"maxclass" : "bpatcher",
					"name" : "hirt_tab_shape.maxpat",
					"numinlets" : 1,
					"numoutlets" : 1,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "" ],
					"patching_rect" : [ 34.0, 159.0, 105.0, 105.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 0.0, 0.0, 360.0, 201.0 ],
					"varname" : "Shape",
					"viewvisibility" : 1
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"midpoints" : [ 43.5, 279.5, 43.5, 279.5 ],
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"midpoints" : [ 165.5, 279.5, 43.5, 279.5 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 104.5, 130.5, 43.5, 130.5 ],
					"source" : [ "obj-3", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 43.5, 130.5, 43.5, 130.5 ],
					"order" : 1,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"midpoints" : [ 165.5, 130.5, 165.5, 130.5 ],
					"source" : [ "obj-3", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"midpoints" : [ 43.5, 116.5, 165.5, 116.5 ],
					"order" : 0,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"midpoints" : [ 226.5, 111.0, 226.5, 111.0 ],
					"source" : [ "obj-3", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"midpoints" : [ 43.5, 72.0, 43.5, 72.0 ],
					"source" : [ "obj-4", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-1::obj-46::obj-70" : [ "Early.num.1425", "Early.num", 0 ],
			"obj-2::obj-99::obj-70" : [ "Freq.1.num.1420", "Freq.1.num", 0 ],
			"obj-2::obj-81::obj-70" : [ "Decay.3.num.1416", "Decay.3.num", 0 ],
			"obj-1::obj-48::obj-86" : [ "Size.1427", "Size", 0 ],
			"obj-2::obj-100::obj-70" : [ "Freq.2.num.1414", "Freq.2.num", 0 ],
			"obj-1::obj-44::obj-86" : [ "Fade In.1423", "Fade In", 0 ],
			"obj-2::obj-88::obj-86" : [ "Slope 1.1418", "Slope 1", 0 ],
			"obj-2::obj-92::obj-86" : [ "Q 2.1412", "Q 2", 0 ],
			"obj-1::obj-97::obj-70" : [ "Decay.num.1428", "Decay.num", 0 ],
			"obj-1::obj-45::obj-70" : [ "Fade.Out.num.1424", "Fade.Out.num", 0 ],
			"obj-2::obj-36::obj-70" : [ "Decay.1.num.1419", "Decay.1.num", 0 ],
			"obj-1::obj-47::obj-86" : [ "Length.1426", "Length", 0 ],
			"obj-2::obj-79::obj-70" : [ "Decay.2.num.1413", "Decay.2.num", 0 ],
			"obj-1::obj-12::obj-86" : [ "Length.1422", "Length", 0 ],
			"obj-2::obj-101::obj-86" : [ "Freq 3.1417", "Freq 3", 0 ],
			"obj-2::obj-96::obj-86" : [ "Slope 3.1415", "Slope 3", 0 ],
			"obj-1::obj-48::obj-70" : [ "Size.num.1427", "Size.num", 0 ],
			"obj-1::obj-9" : [ "Shape Reverse", "Reverse", 0 ],
			"obj-1::obj-44::obj-70" : [ "Fade.In.num.1423", "Fade.In.num", 0 ],
			"obj-2::obj-88::obj-70" : [ "Slope.1.num.1418", "Slope.1.num", 0 ],
			"obj-1::obj-46::obj-86" : [ "Early.1425", "Early", 0 ],
			"obj-2::obj-92::obj-70" : [ "Q.2.num.1412", "Q.2.num", 0 ],
			"obj-2::obj-99::obj-86" : [ "Freq 1.1420", "Freq 1", 0 ],
			"obj-2::obj-81::obj-86" : [ "Decay 3.1416", "Decay 3", 0 ],
			"obj-2::obj-100::obj-86" : [ "Freq 2.1414", "Freq 2", 0 ],
			"obj-1::obj-47::obj-70" : [ "Length.num.1426", "Length.num", 0 ],
			"obj-1::obj-12::obj-70" : [ "Length.num.1422", "Length.num", 0 ],
			"obj-2::obj-101::obj-70" : [ "Freq.3.num.1417", "Freq.3.num", 0 ],
			"obj-1::obj-97::obj-86" : [ "Decay.1428", "Decay", 0 ],
			"obj-2::obj-96::obj-70" : [ "Slope.3.num.1415", "Slope.3.num", 0 ],
			"obj-1::obj-45::obj-86" : [ "Fade Out.1424", "Fade Out", 0 ],
			"obj-2::obj-36::obj-86" : [ "Decay 1.1419", "Decay 1", 0 ],
			"obj-1::obj-49" : [ "Length Mode", "Length Mode", 0 ],
			"obj-1::obj-53" : [ "Shape Direct Mode", "Direct Mode", 0 ],
			"obj-2::obj-79::obj-86" : [ "Decay 2.1413", "Decay 2", 0 ]
		}
,
		"dependency_cache" : [ 			{
				"name" : "hirt_tab_shape.maxpat",
				"bootpath" : "~/Documents/Max 7/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.dial.maxpat",
				"bootpath" : "~/Documents/Max 7/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_abstractions",
				"patcherrelativepath" : "../../HIRT_abstractions",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_tab_damp.maxpat",
				"bootpath" : "~/Documents/Max 7/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_interface_damp.js",
				"bootpath" : "~/Documents/Max 7/Packages/HISSTools Impulse Response Toolbox (HIRT)/jsui",
				"patcherrelativepath" : "../../../jsui",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "spectrumdraw~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0,
		"styles" : [ 			{
				"name" : "max6box",
				"default" : 				{
					"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ],
					"accentcolor" : [ 0.8, 0.839216, 0.709804, 1.0 ],
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.5 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "max6inlet",
				"default" : 				{
					"color" : [ 0.423529, 0.372549, 0.27451, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "max6message",
				"default" : 				{
					"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ],
					"bgfillcolor" : 					{
						"type" : "gradient",
						"color1" : [ 0.866667, 0.866667, 0.866667, 1.0 ],
						"color2" : [ 0.788235, 0.788235, 0.788235, 1.0 ],
						"color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
						"angle" : 270.0,
						"proportion" : 0.39,
						"autogradient" : 0
					}

				}
,
				"parentstyle" : "max6box",
				"multi" : 0
			}
, 			{
				"name" : "max6outlet",
				"default" : 				{
					"color" : [ 0.0, 0.454902, 0.498039, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
 ],
		"toolbaradditions" : [ "audiosolo", "audiomute", "transport" ],
		"toolbarexclusions" : [ "browsevizzie", "browsebeap" ],
		"bgfillcolor_type" : "gradient",
		"bgfillcolor_color1" : [ 0.376471, 0.384314, 0.4, 1.0 ],
		"bgfillcolor_color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
		"bgfillcolor_color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
		"bgfillcolor_angle" : 270.0,
		"bgfillcolor_proportion" : 0.39
	}

}
