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
		"rect" : [ 34.0, 45.0, 831.0, 721.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 13.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 2,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 2,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 2,
		"tallnewobj" : 0,
		"boxanimatetime" : 100,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "<none>",
		"showrootpatcherontab" : 0,
		"showontab" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-8",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 287.716277999999988, 165.0, 68.0, 23.0 ],
					"text" : "closebang"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 8,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 0.0, 26.0, 831.0, 695.0 ],
						"bglocked" : 1,
						"openinpresentation" : 0,
						"default_fontsize" : 13.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "<none>",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-26",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 119.5, 594.0, 29.5, 23.0 ],
									"text" : "set"
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-20",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 336.182555999999977, 422.5, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "2",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-36",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 243.817443999999995, 419.0, 90.365097000000006, 25.0 ],
									"text" : "play audio",
									"textjustification" : 1
								}

							}
, 							{
								"box" : 								{
									"clipheight" : 37.0,
									"data" : 									{
										"clips" : [ 											{
												"absolutepath" : "sacre.aiff",
												"filename" : "sacre.aiff",
												"filekind" : "audiofile",
												"loop" : 1,
												"content_state" : 												{
													"formant" : [ 1.0 ],
													"originallength" : [ 0.0, "ticks" ],
													"quality" : [ "basic" ],
													"originaltempo" : [ 120.0 ],
													"basictuning" : [ 440 ],
													"mode" : [ "basic" ],
													"slurtime" : [ 0.0 ],
													"pitchcorrection" : [ 0 ],
													"followglobaltempo" : [ 0 ],
													"pitchshift" : [ 1.0 ],
													"play" : [ 0 ],
													"originallengthms" : [ 0.0 ],
													"timestretch" : [ 0 ],
													"formantcorrection" : [ 0 ],
													"speed" : [ 1.0 ]
												}

											}
 ]
									}
,
									"id" : "obj-30",
									"maxclass" : "playlist~",
									"numinlets" : 1,
									"numoutlets" : 5,
									"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
									"patching_rect" : [ 11.0, 412.5, 231.0, 38.0 ]
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubblepoint" : 0.25,
									"bubbleside" : 0,
									"id" : "obj-7",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 511.0, 594.0, 218.0, 69.0 ],
									"text" : "...after this, cmnd/ctrl-e to go into edit mode, then cmnd/ctrl-click the \"View\" button"
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubblepoint" : 0.2,
									"bubbleside" : 0,
									"id" : "obj-18",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 319.0, 525.0, 346.0, 69.0 ],
									"text" : "to explore the abstraction either right-click an instantiated abstraction and choose \"Object > Open Original...\", or open the original from the Max browser..."
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-16",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 380.634887999999989, 297.0, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "5",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-14",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 66.634902999999994, 210.0, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "4",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-13",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 62.634902954101562, 356.0, 138.0, 21.0 ],
									"text" : "(make some changes)"
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-3",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 66.634902954101562, 334.0, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "3",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"hidden" : 1,
									"id" : "obj-11",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 633.0, 11.0, 81.0, 23.0 ],
									"text" : "savemode 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-8",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 88.634902999999994, 333.0, 37.0, 23.0 ],
									"text" : "front"
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-6",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 455.0, 490.0, 250.0, 25.0 ],
									"text" : "(messages can be sent to either inlet)",
									"textjustification" : 1
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubblepoint" : 0.1,
									"bubbleside" : 2,
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-54",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 434.0, 337.0, 149.0, 40.0 ],
									"text" : "Turn bank-autosave  on"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-55",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 434.0, 377.0, 81.0, 23.0 ],
									"text" : "savemode 2"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"hidden" : 1,
									"id" : "obj-51",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 434.0, 417.0, 103.0, 21.0 ],
									"text" : "etc"
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubblepoint" : 0.1,
									"bubbleside" : 2,
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-4",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 449.0, 271.0, 125.0, 40.0 ],
									"text" : "Read a saved bank"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-34",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 464.0, 244.0, 38.0, 23.0 ],
									"text" : "write"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-5",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 449.0, 311.0, 37.0, 23.0 ],
									"text" : "read"
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubblepoint" : 0.1,
									"bubbleside" : 2,
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-61",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 464.0, 204.0, 202.0, 40.0 ],
									"text" : "Save presets bank in json format"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 3,
									"outlettype" : [ "signal", "signal", "" ],
									"patching_rect" : [ 151.0, 502.0, 302.0, 23.0 ],
									"text" : "hirt.convolver~ myPresets @outputmode 2 @flat 1",
									"varname" : "hirt.convolver~"
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-42",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 687.0, 315.0, 98.0, 23.0 ],
									"text" : "loadmess set 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-41",
									"maxclass" : "toggle",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 687.0, 346.0, 24.0, 24.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-39",
									"ignoreclick" : 1,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 687.0, 377.0, 46.0, 23.0 ],
									"text" : "flat $1"
								}

							}
, 							{
								"box" : 								{
									"dontreplace" : 1,
									"frozen_box_attributes" : [ "patching_rect" ],
									"id" : "obj-31",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 151.0, 594.0, 302.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-29",
									"maxclass" : "button",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 151.0, 210.0, 24.0, 24.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-27",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 705.0, 421.0, 97.0, 23.0 ],
									"text" : "storagewindow"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-23",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 604.0, 421.0, 84.0, 23.0 ],
									"text" : "clientwindow"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-21",
									"maxclass" : "number",
									"maximum" : 99,
									"minimum" : 1,
									"mousefilter" : 1,
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 304.0, 333.0, 50.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-19",
									"maxclass" : "tab",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "int", "", "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 274.0, 251.0, 79.0, 65.0 ],
									"tabs" : [ "1", "2", "3", "4", "5", "6", "7", "8", "9" ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-17",
									"maxclass" : "number",
									"maximum" : 99,
									"minimum" : 1,
									"mousefilter" : 1,
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 191.5, 210.0, 50.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-15",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 151.0, 253.0, 59.5, 23.0 ],
									"text" : "i 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 151.0, 293.0, 91.0, 23.0 ],
									"text" : "prepend store"
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-12",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 279.0, 554.5, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "1",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-38",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 198.0, 552.0, 79.0, 25.0 ],
									"text" : "start dsp"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-24",
									"local" : 1,
									"maxclass" : "ezdac~",
									"numinlets" : 2,
									"numoutlets" : 0,
									"patching_rect" : [ 151.0, 542.0, 45.0, 45.0 ]
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubbleside" : 3,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-33",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 88.634902999999994, 209.0, 60.365096999999999, 25.0 ],
									"text" : "store"
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-32",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 355.0, 271.0, 60.365096999999999, 25.0 ],
									"text" : "recall"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-22",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 515.0, 360.0, 116.0, 36.0 ],
									"text" : "(will prompt save dialogue on close)"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-28",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 434.0, 553.0, 141.0, 23.0 ],
									"text" : "route \"IR::Current File\""
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"border" : 0,
									"filename" : "helpdetails.js",
									"id" : "obj-2",
									"ignoreclick" : 1,
									"jsarguments" : [ "hirt.convolver~" ],
									"maxclass" : "jsui",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 11.0, 11.0, 620.0, 190.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 1 ],
									"midpoints" : [ 302.0, 533.0, 186.5, 533.0 ],
									"source" : [ "obj-1", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-28", 0 ],
									"source" : [ "obj-1", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 160.5, 450.5, 160.5, 450.5 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"hidden" : 1,
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 0 ],
									"source" : [ "obj-15", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 1 ],
									"source" : [ "obj-17", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-21", 0 ],
									"source" : [ "obj-19", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 313.5, 492.5, 160.5, 492.5 ],
									"source" : [ "obj-21", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 613.5, 472.5, 443.5, 472.5 ],
									"source" : [ "obj-23", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-31", 0 ],
									"source" : [ "obj-26", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 714.5, 472.5, 443.5, 472.5 ],
									"source" : [ "obj-27", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-31", 1 ],
									"source" : [ "obj-28", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"source" : [ "obj-29", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 73.5, 472.75, 443.5, 472.75 ],
									"source" : [ "obj-30", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 20.5, 482.75, 160.5, 482.75 ],
									"source" : [ "obj-30", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 473.5, 472.0, 443.5, 472.0 ],
									"source" : [ "obj-34", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 696.5, 472.0, 443.5, 472.0 ],
									"source" : [ "obj-39", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 0 ],
									"source" : [ "obj-41", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-41", 0 ],
									"hidden" : 1,
									"source" : [ "obj-42", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 458.5, 473.5, 443.5, 473.5 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 1 ],
									"midpoints" : [ 443.5, 472.5, 443.5, 472.5 ],
									"source" : [ "obj-55", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 98.134902999999994, 492.5, 160.5, 492.5 ],
									"source" : [ "obj-8", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 56.0, 205.0, 64.0, 23.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontsize" : 13.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p presets",
					"varname" : "basic_tab[3]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 8,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 0.0, 26.0, 831.0, 695.0 ],
						"bglocked" : 1,
						"openinpresentation" : 0,
						"default_fontsize" : 13.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "<none>",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"fontface" : 3,
									"id" : "obj-9",
									"linecount" : 4,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 91.0, 588.0, 205.0, 65.0 ],
									"text" : "The instance of hirt.convolver~ in the \"basic\" tab has an argument, naming it \"myConvolver\""
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-6",
									"linecount" : 2,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 298.0, 546.5, 119.0, 38.0 ],
									"text" : ";\rmyConvolver open"
								}

							}
, 							{
								"box" : 								{
									"format" : 6,
									"id" : "obj-20",
									"maxclass" : "flonum",
									"maximum" : 18.0,
									"minimum" : -18.0,
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 565.0, 554.0, 50.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-16",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 565.0, 634.0, 96.0, 23.0 ],
									"text" : "s myConvolver"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-15",
									"ignoreclick" : 1,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 565.0, 594.0, 75.0, 23.0 ],
									"text" : "eqgain2 $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-13",
									"linecount" : 12,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 91.0, 356.0, 138.0, 181.0 ],
									"text" : "If your convolver is named with an argument to the abstraction, this will be your own namespace with which to address it. This message will change the Width parameter of the basic-tab convolver named \"myConvolver\""
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-12",
									"linecount" : 9,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 298.0, 356.0, 119.0, 137.0 ],
									"text" : "This message will open a browser window to search for a new IR file to load into this named instance only (not available in the global namespace)"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-10",
									"linecount" : 11,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 565.0, 356.0, 148.0, 166.0 ],
									"text" : "There is also a global namespace, \"hirt.convolver\". This message will change the Dry/Wet parameter for every currently active instance of the hirt.convolver~ abstraction (every instance in every tab of this helppatcher)"
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 91.0, 222.0, 116.0, 23.0 ],
									"text" : "loadmess set 100."
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-3",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 565.0, 222.0, 109.0, 23.0 ],
									"text" : "loadmess set 50."
								}

							}
, 							{
								"box" : 								{
									"format" : 6,
									"id" : "obj-18",
									"maxclass" : "flonum",
									"maximum" : 100.0,
									"minimum" : 0.0,
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 565.0, 262.0, 50.0, 23.0 ],
									"varname" : "number[1]"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-19",
									"ignoreclick" : 1,
									"linecount" : 2,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 565.0, 302.0, 148.0, 38.0 ],
									"text" : ";\rhirt.convolver drywet $1"
								}

							}
, 							{
								"box" : 								{
									"format" : 6,
									"id" : "obj-5",
									"maxclass" : "flonum",
									"maximum" : 100.0,
									"minimum" : 0.0,
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 91.0, 262.0, 50.0, 23.0 ],
									"varname" : "number"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-8",
									"linecount" : 2,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 298.0, 302.0, 119.0, 38.0 ],
									"text" : ";\rmyConvolver iradd"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-11",
									"ignoreclick" : 1,
									"linecount" : 2,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 91.0, 302.0, 138.0, 38.0 ],
									"text" : ";\rmyConvolver width $1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-22",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 663.0, 627.5, 111.0, 36.0 ],
									"text" : "or your can just create a send :-)"
								}

							}
, 							{
								"box" : 								{
									"border" : 0,
									"filename" : "helpargs.js",
									"id" : "obj-4",
									"ignoreclick" : 1,
									"jsarguments" : [ "hirt.convolver~" ],
									"maxclass" : "jsui",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 278.0, 610.0, 225.979522705078125, 39.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-17",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 298.0, 523.5, 119.0, 21.0 ],
									"text" : "open UI window"
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"border" : 0,
									"filename" : "helpdetails.js",
									"id" : "obj-2",
									"ignoreclick" : 1,
									"jsarguments" : [ "hirt.convolver~" ],
									"maxclass" : "jsui",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 11.0, 11.0, 620.0, 190.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"hidden" : 1,
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-16", 0 ],
									"source" : [ "obj-15", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-19", 0 ],
									"source" : [ "obj-18", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"source" : [ "obj-20", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-18", 0 ],
									"hidden" : 1,
									"source" : [ "obj-3", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-11", 0 ],
									"source" : [ "obj-5", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 41.0, 165.0, 62.0, 23.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontsize" : 13.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p remote",
					"varname" : "basic_tab[2]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 8,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 0.0, 26.0, 831.0, 695.0 ],
						"bglocked" : 1,
						"openinpresentation" : 0,
						"default_fontsize" : 13.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "<none>",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-7",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 101.0, 353.0, 273.139404296875, 21.0 ],
									"text" : "(also available as a handy Snippet)",
									"textcolor" : [ 0.5, 0.5, 0.5, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"arrows" : 1,
									"id" : "obj-5",
									"justification" : 1,
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 11.0, 353.0, 88.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontlink" : 1,
									"hint" : "",
									"id" : "obj-32",
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 296.0, 465.238281250000057, 136.68798828125, 22.5234375 ],
									"text" : "embedded convolver",
									"texton" : "hirt.convolver~",
									"truncate" : 0,
									"usetextovercolor" : 1
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-36",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 101.0, 451.5, 178.0, 50.0 ],
									"text" : "see a patch where the  hirt.convolver~ abstraction is instantiated as a Bpatcher:",
									"textcolor" : [ 0.5, 0.5, 0.5, 1.0 ],
									"textjustification" : 2
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-46",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 433.878905999999915, 464.000000250000085, 62.0, 25.0 ],
									"text" : "open",
									"textjustification" : 1
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-3",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 0,
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 0,
											"revision" : 8,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 625.0, 343.0, 540.0, 271.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 13.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 2,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 2,
										"toptoolbarpinned" : 2,
										"righttoolbarpinned" : 2,
										"bottomtoolbarpinned" : 1,
										"toolbars_unpinned_last_save" : 7,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "<none>",
										"boxes" : [ 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-15",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 88.0, 199.0, 160.0, 23.0 ],
													"text" : "s globalclose_hcv_helpfile"
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-13",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 88.0, 81.0, 23.0, 23.0 ],
													"text" : "t b"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"hidden" : 1,
													"id" : "obj-12",
													"index" : 2,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 88.0, 34.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-2",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 34.0, 121.0, 212.0, 23.0 ],
													"text" : "zl.reg convolver.embed.tab"
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-3",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 34.0, 81.0, 23.0, 23.0 ],
													"text" : "t b"
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-5",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 380.0, 159.0, 84.0, 23.0 ],
													"text" : "prepend load"
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-4",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 380.0, 121.0, 112.0, 23.0 ],
													"text" : "sprintf %s.maxpat"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"hidden" : 1,
													"id" : "obj-1",
													"index" : 1,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 34.0, 34.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-7",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 277.0, 121.0, 86.0, 23.0 ],
													"text" : "prepend help"
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-35",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 277.0, 199.0, 57.0, 23.0 ],
													"text" : "pcontrol"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"hidden" : 1,
													"id" : "obj-14",
													"index" : 3,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 277.0, 34.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"hidden" : 1,
													"id" : "obj-8",
													"linecount" : 4,
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 336.0, 184.0, 150.0, 65.0 ],
													"text" : "!! the hirt.convolutionreverb~ helpfile can take a long time to load !!"
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-3", 0 ],
													"hidden" : 1,
													"midpoints" : [ 43.5, 72.0, 43.5, 72.0 ],
													"source" : [ "obj-1", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-13", 0 ],
													"hidden" : 1,
													"source" : [ "obj-12", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-15", 0 ],
													"hidden" : 1,
													"source" : [ "obj-13", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-5", 0 ],
													"hidden" : 1,
													"midpoints" : [ 43.5, 151.0, 389.5, 151.0 ],
													"source" : [ "obj-2", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-2", 0 ],
													"hidden" : 1,
													"midpoints" : [ 43.5, 112.0, 43.5, 112.0 ],
													"source" : [ "obj-3", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-5", 0 ],
													"hidden" : 1,
													"midpoints" : [ 389.5, 151.0, 389.5, 151.0 ],
													"source" : [ "obj-4", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-35", 0 ],
													"hidden" : 1,
													"midpoints" : [ 389.5, 190.0, 286.5, 190.0 ],
													"source" : [ "obj-5", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-35", 0 ],
													"hidden" : 1,
													"midpoints" : [ 286.5, 171.0, 286.5, 171.0 ],
													"source" : [ "obj-7", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 296.0, 504.761719250000112, 78.139404296875, 23.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"fontsize" : 13.0,
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p load",
									"varname" : "load"
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 391.139404296875, 504.761719250000112, 68.0, 23.0 ],
									"text" : "closebang"
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"bgmode" : 0,
									"border" : 0,
									"clickthrough" : 0,
									"enablehscroll" : 0,
									"enablevscroll" : 0,
									"id" : "obj-6",
									"lockeddragscroll" : 0,
									"maxclass" : "bpatcher",
									"name" : "hirt.credits.mini.maxpat",
									"numinlets" : 0,
									"numoutlets" : 0,
									"offset" : [ 0.0, 0.0 ],
									"patching_rect" : [ 497.0, 185.0, 321.0, 54.0 ],
									"varname" : "hirt_credits",
									"viewvisibility" : 1
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"border" : 0,
									"filename" : "helpdetails.js",
									"id" : "obj-2",
									"ignoreclick" : 1,
									"jsarguments" : [ "hirt.convolver~" ],
									"maxclass" : "jsui",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 11.0, 11.0, 620.0, 187.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 2 ],
									"hidden" : 1,
									"source" : [ "obj-32", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"hidden" : 1,
									"source" : [ "obj-32", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 1 ],
									"hidden" : 1,
									"source" : [ "obj-4", 0 ]
								}

							}
 ],
						"styles" : [ 							{
								"name" : "AudioStatus_Menu",
								"default" : 								{
									"bgfillcolor" : 									{
										"type" : "color",
										"color" : [ 0.294118, 0.313726, 0.337255, 1 ],
										"color1" : [ 0.454902, 0.462745, 0.482353, 0.0 ],
										"color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
										"angle" : 270.0,
										"proportion" : 0.39,
										"autogradient" : 0
									}

								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "DP.M4L",
								"default" : 								{
									"color" : [ 0.960784, 0.827451, 0.156863, 1.0 ],
									"selectioncolor" : [ 0.960784, 0.827451, 0.156863, 1.0 ],
									"bgcolor" : [ 0.4, 0.4, 0.4, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "DP.M4L-New",
								"default" : 								{
									"color" : [ 0.960784, 0.827451, 0.156863, 1.0 ],
									"bgcolor" : [ 1.0, 1.0, 1.0, 0.15 ],
									"textcolor_inverse" : [ 1.0, 1.0, 1.0, 0.15 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "DP.M4L.Test1",
								"default" : 								{
									"color" : [ 0.345098, 0.513725, 0.572549, 1.0 ],
									"bgcolor" : [ 1.0, 1.0, 1.0, 0.15 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "Daan_small10",
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "max6box",
								"default" : 								{
									"accentcolor" : [ 0.8, 0.839216, 0.709804, 1.0 ],
									"bgcolor" : [ 1.0, 1.0, 1.0, 0.5 ],
									"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "max6inlet",
								"default" : 								{
									"color" : [ 0.423529, 0.372549, 0.27451, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "max6message",
								"default" : 								{
									"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ],
									"bgfillcolor" : 									{
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
, 							{
								"name" : "max6outlet",
								"default" : 								{
									"color" : [ 0.0, 0.454902, 0.498039, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "newobjBlue-1",
								"default" : 								{
									"accentcolor" : [ 0.317647, 0.654902, 0.976471, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "newobjGreen-1",
								"default" : 								{
									"accentcolor" : [ 0.0, 0.533333, 0.168627, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "newobjYellow-1",
								"default" : 								{
									"accentcolor" : [ 0.82517, 0.78181, 0.059545, 1.0 ],
									"fontsize" : [ 12.059008 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
, 							{
								"name" : "numberGold-1",
								"default" : 								{
									"accentcolor" : [ 0.764706, 0.592157, 0.101961, 1.0 ]
								}
,
								"parentstyle" : "",
								"multi" : 0
							}
 ]
					}
,
					"patching_rect" : [ 26.0, 125.0, 61.0, 23.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontsize" : 13.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p embed",
					"varname" : "basic_tab[1]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 448.0, 85.0, 207.0, 23.0 ],
					"saved_object_attributes" : 					{
						"filename" : "hirt.helpstarter.js",
						"parameter_enable" : 0
					}
,
					"text" : "js hirt.helpstarter.js hirt.convolver~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 8,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 34.0, 71.0, 831.0, 695.0 ],
						"bglocked" : 1,
						"openinpresentation" : 0,
						"default_fontsize" : 13.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "<none>",
						"showontab" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 274.0, 503.0, 177.0, 23.0 ],
									"text" : "substitute \"Bypass State\" set"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-7",
									"linecount" : 2,
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 3,
									"outlettype" : [ "signal", "signal", "" ],
									"patching_rect" : [ 196.0, 446.0, 97.0, 38.0 ],
									"text" : "hirt.convolver~ myConvolver",
									"varname" : "hirt.convolver~"
								}

							}
, 							{
								"box" : 								{
									"clipheight" : 53.0,
									"data" : 									{
										"clips" : [ 											{
												"absolutepath" : "isthatyou.aiff",
												"filename" : "isthatyou.aiff",
												"filekind" : "audiofile",
												"loop" : 0,
												"content_state" : 												{
													"formant" : [ 1.0 ],
													"originallength" : [ 0.0, "ticks" ],
													"quality" : [ "basic" ],
													"originaltempo" : [ 120.0 ],
													"basictuning" : [ 440 ],
													"mode" : [ "basic" ],
													"slurtime" : [ 0.0 ],
													"pitchcorrection" : [ 0 ],
													"followglobaltempo" : [ 0 ],
													"pitchshift" : [ 1.0 ],
													"play" : [ 0 ],
													"originallengthms" : [ 0.0 ],
													"timestretch" : [ 0 ],
													"formantcorrection" : [ 0 ],
													"speed" : [ 1.0 ]
												}

											}
, 											{
												"absolutepath" : "drumLoop.aif",
												"filename" : "drumLoop.aif",
												"filekind" : "audiofile",
												"loop" : 1,
												"content_state" : 												{
													"formant" : [ 1.0 ],
													"originallength" : [ 0.0, "ticks" ],
													"quality" : [ "basic" ],
													"originaltempo" : [ 120.0 ],
													"basictuning" : [ 440 ],
													"mode" : [ "basic" ],
													"slurtime" : [ 0.0 ],
													"pitchcorrection" : [ 0 ],
													"followglobaltempo" : [ 0 ],
													"pitchshift" : [ 1.0 ],
													"play" : [ 0 ],
													"originallengthms" : [ 0.0 ],
													"timestretch" : [ 0 ],
													"formantcorrection" : [ 0 ],
													"speed" : [ 1.0 ]
												}

											}
 ]
									}
,
									"id" : "obj-11",
									"maxclass" : "playlist~",
									"numinlets" : 1,
									"numoutlets" : 5,
									"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
									"patching_rect" : [ 196.0, 270.0, 331.0, 108.0 ],
									"waveformdisplay" : 0
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-24",
									"local" : 1,
									"maxclass" : "ezdac~",
									"numinlets" : 2,
									"numoutlets" : 0,
									"patching_rect" : [ 196.0, 531.0, 58.0, 58.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-35",
									"maxclass" : "toggle",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"parameter_enable" : 1,
									"patching_rect" : [ 56.0, 311.5, 24.0, 24.0 ],
									"saved_attribute_attributes" : 									{
										"valueof" : 										{
											"parameter_enum" : [ "off", "on" ],
											"parameter_type" : 3,
											"parameter_longname" : "toggle",
											"parameter_initial_enable" : 1,
											"parameter_invisible" : 1,
											"parameter_mmax" : 1.0,
											"parameter_initial" : [ 0 ],
											"parameter_shortname" : "toggle"
										}

									}
,
									"varname" : "toggle"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-1",
									"ignoreclick" : 1,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 56.0, 355.0, 70.0, 23.0 ],
									"text" : "bypass $1"
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-25",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 621.365111999999954, 315.0, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "2",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-26",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 529.0, 311.5, 90.365097000000006, 25.0 ],
									"text" : "play audio"
								}

							}
, 							{
								"box" : 								{
									"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
									"fontname" : "Arial Bold",
									"hint" : "",
									"id" : "obj-12",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 337.0, 549.5, 20.0, 20.0 ],
									"rounded" : 60.0,
									"text" : "1",
									"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"fontname" : "Arial",
									"fontsize" : 13.0,
									"id" : "obj-38",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 256.0, 547.0, 79.0, 25.0 ],
									"text" : "start dsp"
								}

							}
, 							{
								"box" : 								{
									"bubble" : 1,
									"bubbleside" : 3,
									"id" : "obj-9",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 56.0, 452.5, 138.0, 25.0 ],
									"text" : " double-click for UI"
								}

							}
, 							{
								"box" : 								{
									"border" : 0,
									"filename" : "helpargs.js",
									"id" : "obj-4",
									"ignoreclick" : 1,
									"jsarguments" : [ "hirt.convolver~" ],
									"maxclass" : "jsui",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 295.0, 462.0, 225.979522705078125, 39.0 ]
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"bgmode" : 0,
									"border" : 0,
									"clickthrough" : 0,
									"enablehscroll" : 0,
									"enablevscroll" : 0,
									"id" : "obj-6",
									"lockeddragscroll" : 0,
									"maxclass" : "bpatcher",
									"name" : "hirt.credits.mini.maxpat",
									"numinlets" : 0,
									"numoutlets" : 0,
									"offset" : [ 0.0, 0.0 ],
									"patching_rect" : [ 355.0, 11.0, 321.0, 54.0 ],
									"varname" : "hirt_credits",
									"viewvisibility" : 1
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"border" : 0,
									"filename" : "helpdetails.js",
									"id" : "obj-2",
									"ignoreclick" : 1,
									"jsarguments" : [ "hirt.convolver~" ],
									"maxclass" : "jsui",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 11.0, 11.0, 622.365111999999954, 189.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 65.5, 411.5, 205.5, 411.5 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 1 ],
									"source" : [ "obj-11", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-35", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-35", 0 ],
									"hidden" : 1,
									"midpoints" : [ 283.5, 604.0, 33.5, 604.0, 33.5, 300.5, 65.5, 300.5 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 1 ],
									"source" : [ "obj-7", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-24", 0 ],
									"source" : [ "obj-7", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"hidden" : 1,
									"source" : [ "obj-7", 2 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 10.0, 85.0, 50.0, 23.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontsize" : 13.0,
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p basic",
					"varname" : "basic_tab"
				}

			}
, 			{
				"box" : 				{
					"border" : 0,
					"filename" : "helpname.js",
					"id" : "obj-4",
					"ignoreclick" : 1,
					"jsarguments" : [ "hirt.convolver~" ],
					"maxclass" : "jsui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 10.0, 10.0, 322.687957763671875, 57.599853515625 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 8,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 0.0, 26.0, 831.0, 695.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
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
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "<none>",
						"showontab" : 1,
						"boxes" : [  ],
						"lines" : [  ]
					}
,
					"patching_rect" : [ 287.716277999999988, 85.0, 45.0, 23.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p ?",
					"varname" : "q_tab"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 287.716277999999988, 245.0, 160.0, 23.0 ],
					"text" : "s globalclose_hcv_helpfile"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"source" : [ "obj-8", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-2::obj-7::obj-28" : [ "PATCH/PRESS", "PATCH/PRESS", 0 ],
			"obj-2::obj-7::obj-1::obj-97::obj-70" : [ "hirt.val[39]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-1::obj-48::obj-70" : [ "hirt.val[31]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-36::obj-131::obj-11" : [ "IR Menu", "IR Menu", 0 ],
			"obj-7::obj-1::obj-10::obj-53::obj-70" : [ "hirt.val[56]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-3::obj-63::obj-70" : [ "hirt.val[63]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-19::obj-70" : [ "hirt.val[37]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-97::obj-70" : [ "hirt.val[64]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-36::obj-131::obj-11" : [ "IR Menu[5]", "IR Menu", 0 ],
			"obj-7::obj-1::obj-10::obj-17::obj-70" : [ "hirt.val[83]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-36::obj-35" : [ "Drop IR", "live.drop", 3 ],
			"obj-7::obj-1::obj-36::obj-35" : [ "Drop IR[5]", "live.drop", 3 ],
			"obj-7::obj-1::obj-10::obj-51::obj-70" : [ "hirt.val[80]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-3::obj-63::obj-70" : [ "Width.val.1368", "Width.val", 0 ],
			"obj-7::obj-1::obj-3::obj-64::obj-70" : [ "hirt.val[85]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-17::obj-70" : [ "hirt.val[36]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-48::obj-70" : [ "hirt.val[87]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-10::obj-3" : [ "EQ Routing[5]", "EQ", 0 ],
			"obj-2::obj-7::obj-10::obj-3" : [ "EQ Routing", "EQ", 0 ],
			"obj-7::obj-1::obj-10::obj-56::obj-70" : [ "hirt.val[57]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-55::obj-70" : [ "hirt.val[34]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-21" : [ "Saturation Type", "Saturation", 0 ],
			"obj-2::obj-7::obj-10::obj-54::obj-70" : [ "hirt.val[33]", "hirt.val", 0 ],
			"obj-2::obj-35" : [ "toggle", "toggle", 0 ],
			"obj-7::obj-1::obj-10::obj-50::obj-70" : [ "hirt.val[55]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-51::obj-70" : [ "hirt.val[20]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-3::obj-65::obj-70" : [ "hirt.val[62]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-108::obj-90" : [ "number[3]", "number", 0 ],
			"obj-2::obj-7::obj-3::obj-65::obj-70" : [ "Dry/Wet.val.1366", "Dry/Wet.val", 0 ],
			"obj-2::obj-7::obj-108::obj-90" : [ "number", "number", 0 ],
			"obj-7::obj-1::obj-28" : [ "PATCH/PRESS[5]", "PATCH/PRESS", 0 ],
			"obj-2::obj-7::obj-10::obj-56::obj-70" : [ "hirt.val[35]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-10::obj-55::obj-70" : [ "hirt.val[82]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-50::obj-70" : [ "hirt.val", "hirt.val", 0 ],
			"obj-7::obj-1::obj-10::obj-22::obj-70" : [ "hirt.val[58]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-1::obj-59::obj-70" : [ "hirt.val[21]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-36::obj-131::obj-10" : [ "Category Menu[5]", "Category Menu", 0 ],
			"obj-7::obj-1::obj-10::obj-21" : [ "Saturation Type[5]", "Saturation", 0 ],
			"obj-2::obj-7::obj-10::obj-53::obj-70" : [ "hirt.val[30]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-36::obj-131::obj-10" : [ "Category Menu", "Category Menu", 0 ],
			"obj-7::obj-1::obj-10::obj-54::obj-70" : [ "hirt.val[81]", "hirt.val", 0 ],
			"obj-7::obj-1::obj-1::obj-59::obj-70" : [ "hirt.val[86]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-10::obj-22::obj-70" : [ "hirt.val[38]", "hirt.val", 0 ],
			"obj-2::obj-7::obj-3::obj-64::obj-70" : [ "Gain.val.1367", "Gain.val", 0 ],
			"obj-7::obj-1::obj-10::obj-19::obj-70" : [ "hirt.val[84]", "hirt.val", 0 ],
			"parameterbanks" : 			{

			}
,
			"parameter_overrides" : 			{
				"obj-2::obj-7::obj-1::obj-97::obj-70" : 				{
					"parameter_longname" : "hirt.val[39]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.01,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 1.0, 200.0 ]
				}
,
				"obj-2::obj-7::obj-1::obj-48::obj-70" : 				{
					"parameter_longname" : "hirt.val[31]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.58,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 50.0, 200.0 ]
				}
,
				"obj-7::obj-1::obj-10::obj-53::obj-70" : 				{
					"parameter_longname" : "hirt.val[56]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 1000.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-3::obj-63::obj-70" : 				{
					"parameter_longname" : "hirt.val[63]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-2::obj-7::obj-10::obj-19::obj-70" : 				{
					"parameter_longname" : "hirt.val[37]",
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
				"obj-7::obj-1::obj-1::obj-97::obj-70" : 				{
					"parameter_longname" : "hirt.val[64]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.01,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 1.0, 200.0 ]
				}
,
				"obj-7::obj-1::obj-36::obj-131::obj-11" : 				{
					"parameter_longname" : "IR Menu[5]"
				}
,
				"obj-7::obj-1::obj-10::obj-17::obj-70" : 				{
					"parameter_longname" : "hirt.val[83]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.5,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-36::obj-35" : 				{
					"parameter_longname" : "Drop IR[5]"
				}
,
				"obj-7::obj-1::obj-10::obj-51::obj-70" : 				{
					"parameter_longname" : "hirt.val[80]",
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
				"obj-2::obj-7::obj-3::obj-63::obj-70" : 				{
					"parameter_longname" : "Width.val.1368",
					"parameter_shortname" : "Width.val",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-7::obj-1::obj-3::obj-64::obj-70" : 				{
					"parameter_longname" : "hirt.val[85]",
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
				"obj-2::obj-7::obj-10::obj-17::obj-70" : 				{
					"parameter_longname" : "hirt.val[36]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.5,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-1::obj-48::obj-70" : 				{
					"parameter_longname" : "hirt.val[87]",
					"parameter_invisible" : 1,
					"parameter_exponent" : 1.58,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 100.0,
					"parameter_range" : [ 50.0, 200.0 ]
				}
,
				"obj-7::obj-1::obj-10::obj-56::obj-70" : 				{
					"parameter_longname" : "hirt.val[57]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 8000.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-2::obj-7::obj-10::obj-55::obj-70" : 				{
					"parameter_longname" : "hirt.val[34]",
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
				"obj-2::obj-7::obj-10::obj-54::obj-70" : 				{
					"parameter_longname" : "hirt.val[33]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.5,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-10::obj-50::obj-70" : 				{
					"parameter_longname" : "hirt.val[55]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.707107,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-2::obj-7::obj-10::obj-51::obj-70" : 				{
					"parameter_longname" : "hirt.val[20]",
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
				"obj-7::obj-1::obj-3::obj-65::obj-70" : 				{
					"parameter_longname" : "hirt.val[62]",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 50.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-2::obj-7::obj-3::obj-65::obj-70" : 				{
					"parameter_longname" : "Dry/Wet.val.1366",
					"parameter_shortname" : "Dry/Wet.val",
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 5,
					"parameter_initial" : 50.0,
					"parameter_range" : [ 0.0, 100.0 ],
					"parameter_exponent" : 1.0
				}
,
				"obj-2::obj-7::obj-10::obj-56::obj-70" : 				{
					"parameter_longname" : "hirt.val[35]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 8000.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-10::obj-55::obj-70" : 				{
					"parameter_longname" : "hirt.val[82]",
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
				"obj-2::obj-7::obj-10::obj-50::obj-70" : 				{
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.707107,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-10::obj-22::obj-70" : 				{
					"parameter_longname" : "hirt.val[58]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 125.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-2::obj-7::obj-1::obj-59::obj-70" : 				{
					"parameter_longname" : "hirt.val[21]",
					"parameter_exponent" : 2.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 2,
					"parameter_initial" : 0.0,
					"parameter_range" : [ 0.0, 200.0 ]
				}
,
				"obj-7::obj-1::obj-36::obj-131::obj-10" : 				{
					"parameter_longname" : "Category Menu[5]"
				}
,
				"obj-2::obj-7::obj-10::obj-53::obj-70" : 				{
					"parameter_longname" : "hirt.val[30]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 1000.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-7::obj-1::obj-10::obj-54::obj-70" : 				{
					"parameter_longname" : "hirt.val[81]",
					"parameter_exponent" : 3.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 10,
					"parameter_initial" : 0.5,
					"parameter_range" : [ 0.05, 18.0 ]
				}
,
				"obj-7::obj-1::obj-1::obj-59::obj-70" : 				{
					"parameter_longname" : "hirt.val[86]",
					"parameter_exponent" : 2.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 2,
					"parameter_initial" : 0.0,
					"parameter_range" : [ 0.0, 200.0 ]
				}
,
				"obj-2::obj-7::obj-10::obj-22::obj-70" : 				{
					"parameter_longname" : "hirt.val[38]",
					"parameter_exponent" : 4.0,
					"parameter_invisible" : 0,
					"parameter_modmode" : 0,
					"parameter_type" : 0,
					"parameter_unitstyle" : 3,
					"parameter_initial" : 125.0,
					"parameter_range" : [ 10.0, 18000.0 ]
				}
,
				"obj-2::obj-7::obj-3::obj-64::obj-70" : 				{
					"parameter_longname" : "Gain.val.1367",
					"parameter_shortname" : "Gain.val",
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
				"obj-7::obj-1::obj-10::obj-19::obj-70" : 				{
					"parameter_longname" : "hirt.val[84]",
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
				"name" : "helpname.js",
				"bootpath" : "C74:/help/resources",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "helpdetails.js",
				"bootpath" : "C74:/help/resources",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.credits.mini.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/misc/HIRT_resources",
				"patcherrelativepath" : "../misc/HIRT_resources",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "HIRT_HISSTools_Logo.png",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/misc/HIRT_image",
				"patcherrelativepath" : "../misc/HIRT_image",
				"type" : "PNG",
				"implicit" : 1
			}
, 			{
				"name" : "helpargs.js",
				"bootpath" : "C74:/help/resources",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "isthatyou.aiff",
				"bootpath" : "C74:/docs/tutorial-patchers/msp-tut",
				"type" : "AIFF",
				"implicit" : 1
			}
, 			{
				"name" : "drumLoop.aif",
				"bootpath" : "C74:/media/msp",
				"type" : "AIFF",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.convolver~.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver",
				"patcherrelativepath" : "../patchers/HIRT_convolver",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_decay_size_pre.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.dial.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.dial.linear.only.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_cv_info_view.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_output_mini.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolver_clientlist_alias.txt",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.convolver.realtime~.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.convolvestereo~.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolver_rt_part1.gendsp",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "gDSP",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolver_rt_part3.gendsp",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "gDSP",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_gain_params.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.svfcoeff.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_ir_single.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_filter_type.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_loading.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_loading_scheme.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_set.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_check.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_base_name.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_folder.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_data_colls.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_picker.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq_sat.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_interface_eq.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/jsui",
				"patcherrelativepath" : "../jsui",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_zoom_factor.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/javascript",
				"patcherrelativepath" : "../javascript",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_nan_fix.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_gain_display_single.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_convolver/HIRT_convolver_support",
				"patcherrelativepath" : "../patchers/HIRT_convolver/HIRT_convolver_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.size.resample.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../patchers/HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_size_resample_feed.maxpat",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_reverb/HIRT_reverb_support",
				"patcherrelativepath" : "../patchers/HIRT_reverb/HIRT_reverb_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt.helpstarter.js",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/javascript",
				"patcherrelativepath" : "../javascript",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_convolution_rt_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_eq_library.genexpr",
				"bootpath" : "~/SDKs/HISSTools_Impulse_Response_Toolbox/HISSTools_Packaging/HISSTools Impulse Response Toolbox (HIRT)/code",
				"patcherrelativepath" : "../code",
				"type" : "GenX",
				"implicit" : 1
			}
, 			{
				"name" : "sacre.aiff",
				"bootpath" : "C74:/docs/tutorial-patchers/msp-tut",
				"type" : "AIFF",
				"implicit" : 1
			}
, 			{
				"name" : "multiconvolve~.mxo",
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
				"name" : "spectrumdraw~.mxo",
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
				"name" : "bufresample~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0,
		"toolbaradditions" : [ "audiosolo", "audiomute", "transport" ],
		"toolbarexclusions" : [ "browsevizzie", "browsebeap" ]
	}

}
