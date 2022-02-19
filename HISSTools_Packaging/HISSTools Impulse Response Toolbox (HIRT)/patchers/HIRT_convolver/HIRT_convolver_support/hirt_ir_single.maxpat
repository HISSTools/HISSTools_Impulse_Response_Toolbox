{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 2,
			"revision" : 2,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 0.0, 45.0, 930.0, 616.0 ],
		"bglocked" : 1,
		"openinpresentation" : 1,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 2,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 0,
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
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-18",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 123.5, 74.25, 75.0, 22.0 ],
					"text" : "route default"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-27",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 123.5, 34.5, 173.0, 22.0 ],
					"text" : "route load"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-33",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 2,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 20.0, 73.0, 585.0, 615.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
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
						"bottomtoolbarpinned" : 2,
						"toolbars_unpinned_last_save" : 15,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-14",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 201.5, 151.0, 77.0, 22.0 ],
									"text" : "route symbol"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-11",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "NULL" ],
									"patching_rect" : [ 49.0, 73.0, 52.0, 22.0 ],
									"text" : "t l NULL"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "???" ],
									"patching_rect" : [ 351.0, 429.5, 35.0, 22.0 ],
									"text" : "t ???"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-8",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 351.0, 312.5, 22.0, 22.0 ],
									"text" : "t b"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-7",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 351.0, 390.5, 70.0, 22.0 ],
									"text" : "route NULL"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 351.0, 351.5, 72.0, 22.0 ],
									"text" : "zl.reg NULL"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 402.0, 507.5, 103.0, 22.0 ],
									"text" : "prepend notfound"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 402.0, 468.5, 83.0, 22.0 ],
									"text" : "prepend addir"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 201.5, 259.0, 86.0, 22.0 ],
									"text" : "route notfound"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-30",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 402.0, 547.0, 130.5, 22.0 ],
									"text" : "s #1global_errors"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-39",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 201.5, 205.0, 77.0, 22.0 ],
									"text" : "absolutepath"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "" ],
									"patching_rect" : [ 49.0, 112.0, 324.0, 22.0 ],
									"text" : "route bang file"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-21",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"patching_rect" : [ 49.0, 205.0, 136.0, 22.0 ],
									"text" : "opendialog WAVE AIFF"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-18",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 49.0, 34.0, 136.0, 22.0 ],
									"text" : "r #1addIRmanually"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-32",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 49.0, 347.5, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-16",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 280.5, 152.0, 143.0, 20.0 ],
									"text" : "<<-- strip @ttribute"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-14", 0 ],
									"midpoints" : [ 211.0, 142.0, 211.0, 142.0 ],
									"source" : [ "obj-1", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-21", 0 ],
									"midpoints" : [ 58.5, 169.0, 58.5, 169.0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-4", 0 ],
									"midpoints" : [ 360.5, 459.5, 411.5, 459.5 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 58.5, 103.0, 58.5, 103.0 ],
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-6", 1 ],
									"midpoints" : [ 91.5, 103.25, 413.5, 103.25 ],
									"source" : [ "obj-11", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 0 ],
									"midpoints" : [ 269.0, 193.5, 211.0, 193.5 ],
									"order" : 1,
									"source" : [ "obj-14", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 0 ],
									"midpoints" : [ 211.0, 188.5, 211.0, 188.5 ],
									"order" : 1,
									"source" : [ "obj-14", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-6", 1 ],
									"midpoints" : [ 269.0, 182.25, 413.5, 182.25 ],
									"order" : 0,
									"source" : [ "obj-14", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-6", 1 ],
									"midpoints" : [ 211.0, 182.25, 413.5, 182.25 ],
									"order" : 0,
									"source" : [ "obj-14", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-11", 0 ],
									"midpoints" : [ 58.5, 64.0, 58.5, 64.0 ],
									"source" : [ "obj-18", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-32", 0 ],
									"midpoints" : [ 58.5, 279.25, 58.5, 279.25 ],
									"source" : [ "obj-21", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"midpoints" : [ 211.0, 242.5, 211.0, 242.5 ],
									"source" : [ "obj-39", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-5", 0 ],
									"midpoints" : [ 411.5, 498.5, 411.5, 498.5 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-30", 0 ],
									"midpoints" : [ 411.5, 537.75, 411.5, 537.75 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 360.5, 381.5, 360.5, 381.5 ],
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-10", 0 ],
									"midpoints" : [ 360.5, 420.5, 360.5, 420.5 ],
									"source" : [ "obj-7", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-4", 0 ],
									"midpoints" : [ 411.5, 440.0, 411.5, 440.0 ],
									"source" : [ "obj-7", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 360.5, 342.5, 360.5, 342.5 ],
									"source" : [ "obj-8", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-32", 0 ],
									"midpoints" : [ 278.0, 313.75, 58.5, 313.75 ],
									"source" : [ "obj-9", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.749, 0.74902, 0.74902, 0.0 ],
									"destination" : [ "obj-8", 0 ],
									"midpoints" : [ 211.0, 296.25, 360.5, 296.25 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ],
						"styles" : [ 							{
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
									"bgfillcolor" : 									{
										"type" : "gradient",
										"color1" : [ 0.866667, 0.866667, 0.866667, 1.0 ],
										"color2" : [ 0.788235, 0.788235, 0.788235, 1.0 ],
										"color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
										"angle" : 270.0,
										"proportion" : 0.39,
										"autogradient" : 0
									}
,
									"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
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
 ]
					}
,
					"patching_rect" : [ 277.5, 224.25, 77.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p IR_manual"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-29",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 85.0, 375.75, 74.0, 22.0 ],
					"text" : "prepend set"
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-26",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 505.5, 375.75, 128.0, 22.0 ],
					"text" : "r #1clearfilepath"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "clear" ],
					"patching_rect" : [ 19.0, 302.25, 43.0, 22.0 ],
					"text" : "t clear"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 19.0, 263.25, 68.0, 22.0 ],
					"text" : "route none"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 84.5, 560.25, 68.0, 22.0 ],
					"text" : "route bang"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 84.5, 520.75, 61.0, 22.0 ],
					"text" : "route text"
				}

			}
, 			{
				"box" : 				{
					"bangmode" : 1,
					"bgcolor" : [ 0.290196, 0.309804, 0.301961, 0.0 ],
					"border" : 1.0,
					"bordercolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"clickmode" : 1,
					"id" : "obj-19",
					"maxclass" : "textedit",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "int", "", "" ],
					"parameter_enable" : 0,
					"parameter_mappable" : 0,
					"patching_rect" : [ 84.5, 429.75, 243.0, 74.0 ],
					"rounded" : 4.0,
					"varname" : "Drop Path"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 0.541176, 0.815686, 0.913725, 1.0 ],
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "" ],
					"patching_rect" : [ 175.5, 375.75, 243.0, 22.0 ],
					"restore" : [ "<empty>" ],
					"saved_object_attributes" : 					{
						"parameter_enable" : 0,
						"parameter_mappable" : 0
					}
,
					"text" : "pattr \"Current File\" @autorestore 0 @dirty 0",
					"varname" : "Current File"
				}

			}
, 			{
				"box" : 				{
					"align" : 1,
					"allowdrag" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.0 ],
					"bgfillcolor_angle" : 270.0,
					"bgfillcolor_autogradient" : 0.0,
					"bgfillcolor_color" : [ 1.0, 1.0, 1.0, 0.0 ],
					"bgfillcolor_color1" : [ 0.376471, 0.384314, 0.4, 1.0 ],
					"bgfillcolor_color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
					"bgfillcolor_proportion" : 0.39,
					"bgfillcolor_type" : "color",
					"color" : [ 0.803922, 0.898039, 0.909804, 0.0 ],
					"elementcolor" : [ 0.803922, 0.898039, 0.909804, 0.0 ],
					"fontname" : "Lato",
					"fontsize" : 10.0,
					"id" : "obj-64",
					"items" : "sec",
					"maxclass" : "umenu",
					"menumode" : 2,
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"parameter_mappable" : 0,
					"patching_rect" : [ 578.5, 192.25, 100.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 144.0, 1.5, 86.0, 20.0 ],
					"textcolor" : [ 1.0, 1.0, 1.0, 0.66 ],
					"textjustification" : 1,
					"varname" : "IR Length Display"
				}

			}
, 			{
				"box" : 				{
					"annotation" : "",
					"bordercolor" : [ 0.27451, 0.32549, 0.4, 0.0 ],
					"decodemode" : 1,
					"focusbordercolor" : [ 0.0, 0.019608, 0.078431, 0.0 ],
					"id" : "obj-35",
					"legend" : "Drop IRs",
					"maxclass" : "live.drop",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"parameter_enable" : 1,
					"parameter_mappable" : 0,
					"patching_rect" : [ 124.0, 152.25, 87.0, 54.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 2.0, 25.5, 356.0, 146.5 ],
					"saved_attribute_attributes" : 					{
						"bordercolor" : 						{
							"expression" : ""
						}
,
						"focusbordercolor" : 						{
							"expression" : ""
						}
,
						"textcolor" : 						{
							"expression" : ""
						}
,
						"valueof" : 						{
							"parameter_initial" : [ "" ],
							"parameter_invisible" : 1,
							"parameter_longname" : "Drop IR",
							"parameter_order" : 3,
							"parameter_shortname" : "live.drop",
							"parameter_type" : 4
						}

					}
,
					"textcolor" : [ 0.0, 0.019608, 0.078431, 0.0 ],
					"varname" : "Drop IR"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-30",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 546.5, 224.25, 116.0, 22.0 ],
					"text" : "r #1device_load"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-62",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "float", "bang" ],
					"patching_rect" : [ 582.5, 263.25, 150.5, 22.0 ],
					"text" : "buffer~ #1IRdisplay 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 725.0, 74.25, 94.0, 22.0 ],
					"saved_object_attributes" : 					{
						"mode" : 1
					}
,
					"text" : "hover @mode 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 775.0, 159.25, 95.0, 22.0 ],
					"text" : "s #1hover_2"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 799.5, 116.75, 101.0, 22.0 ],
					"text" : "s #1hover_13"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 725.0, 198.25, 95.0, 22.0 ],
					"text" : "s #1hover_0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "bang", "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 2,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 648.0, 68.0, 419.0, 419.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
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
						"bottomtoolbarpinned" : 2,
						"toolbars_unpinned_last_save" : 15,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-13",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 248.25, 96.0, 128.0, 22.0 ],
									"text" : "r #1device_state"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-17",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 131.25, 174.000014999999991, 91.0, 22.0 ],
									"text" : "pak 1 1"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-20",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 131.25, 213.000014999999991, 110.0, 22.0 ],
									"text" : "expr $i1 + $i2 == 2"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-31",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "" ],
									"patching_rect" : [ 98.5, 135.000014999999991, 84.5, 22.0 ],
									"text" : "route init on"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-18",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "" ],
									"patching_rect" : [ 49.0, 96.0, 118.0, 22.0 ],
									"text" : "route theme file"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-85",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "bang", "", "" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 2,
											"revision" : 2,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 241.0, 49.0, 704.0, 583.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
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
										"bottomtoolbarpinned" : 2,
										"toolbars_unpinned_last_save" : 15,
										"tallnewobj" : 0,
										"boxanimatetime" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "PeteSubDefault10",
										"assistshowspatchername" : 0,
										"boxes" : [ 											{
												"box" : 												{
													"id" : "obj-63",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 243.0, 420.0, 89.0, 22.0 ],
													"text" : "prepend active"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-9",
													"index" : 2,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 243.0, 504.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-27",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 75.0, 120.0, 71.0, 22.0 ],
													"text" : "gate 1 1"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-26",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "int", "int" ],
													"patching_rect" : [ 49.0, 81.0, 70.5, 22.0 ],
													"text" : "t i i i"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-24",
													"index" : 3,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 349.0, 504.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-23",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "bang", "" ],
													"patching_rect" : [ 199.0, 81.0, 30.0, 22.0 ],
													"text" : "t b l"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-21",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 349.0, 361.0, 133.0, 22.0 ],
													"text" : "zl.reg 0.65 0.65 0.67 1."
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-20",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "int" ],
													"patching_rect" : [ 49.0, 159.0, 29.5, 22.0 ],
													"text" : "i 1"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-13",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 349.0, 420.0, 103.0, 22.0 ],
													"text" : "prepend textcolor"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-12",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 199.0, 361.0, 133.0, 22.0 ],
													"text" : "zl.reg 0.82 0.21 0.15 1."
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-10",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 49.0, 420.0, 107.0, 22.0 ],
													"text" : "prepend htabcolor"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-6",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 560.0, 322.0, 106.0, 22.0 ],
													"text" : "zl.reg 1. 1. 1. 0.66"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-7",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 370.0, 322.0, 127.0, 22.0 ],
													"text" : "zl.reg 0.38 0.38 0.4 1."
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-8",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 49.0, 361.0, 133.0, 22.0 ],
													"text" : "zl.reg 0.65 0.65 0.67 1."
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-73",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 499.0, 361.0, 152.0, 22.0 ],
													"text" : "zl.reg 0.094 0.118 0.137 1."
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-5",
													"index" : 2,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 199.0, 34.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-3",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 331.0, 81.0, 24.0, 22.0 ],
													"text" : "t b"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-4",
													"index" : 3,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 331.0, 34.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-2",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 3,
													"outlettype" : [ "bang", "bang", "" ],
													"patching_rect" : [ 49.0, 198.0, 46.0, 22.0 ],
													"text" : "sel 0 1"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-1",
													"index" : 1,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 49.0, 34.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-84",
													"index" : 1,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 49.0, 504.0, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-19",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 231.0, 39.0, 60.0, 20.0 ],
													"text" : "theme"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-18",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 81.0, 39.0, 60.0, 20.0 ],
													"text" : "active"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-16",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 363.0, 39.0, 49.0, 20.0 ],
													"text" : "init"
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-26", 0 ],
													"midpoints" : [ 58.5, 72.0, 58.5, 72.0 ],
													"source" : [ "obj-1", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"midpoints" : [ 58.5, 466.5, 252.5, 466.5 ],
													"source" : [ "obj-10", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-10", 0 ],
													"midpoints" : [ 208.5, 401.0, 58.5, 401.0 ],
													"source" : [ "obj-12", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-24", 0 ],
													"midpoints" : [ 358.5, 472.5, 358.5, 472.5 ],
													"order" : 0,
													"source" : [ "obj-13", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"midpoints" : [ 358.5, 466.5, 252.5, 466.5 ],
													"order" : 1,
													"source" : [ "obj-13", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-12", 0 ],
													"midpoints" : [ 72.0, 290.0, 208.5, 290.0 ],
													"order" : 1,
													"source" : [ "obj-2", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-21", 0 ],
													"midpoints" : [ 58.5, 322.0, 358.5, 322.0 ],
													"order" : 0,
													"source" : [ "obj-2", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-73", 0 ],
													"midpoints" : [ 72.0, 290.0, 508.5, 290.0 ],
													"order" : 0,
													"source" : [ "obj-2", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-8", 0 ],
													"midpoints" : [ 58.5, 290.0, 58.5, 290.0 ],
													"order" : 1,
													"source" : [ "obj-2", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-2", 0 ],
													"midpoints" : [ 58.5, 189.0, 58.5, 189.0 ],
													"source" : [ "obj-20", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-13", 0 ],
													"source" : [ "obj-21", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-27", 1 ],
													"midpoints" : [ 208.5, 111.0, 136.5, 111.0 ],
													"source" : [ "obj-23", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-73", 1 ],
													"midpoints" : [ 219.5, 111.5, 641.5, 111.5 ],
													"source" : [ "obj-23", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-20", 0 ],
													"midpoints" : [ 58.5, 130.5, 58.5, 130.5 ],
													"source" : [ "obj-26", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-27", 0 ],
													"midpoints" : [ 84.25, 111.0, 84.5, 111.0 ],
													"source" : [ "obj-26", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-63", 0 ],
													"midpoints" : [ 110.0, 174.0, 252.5, 174.0 ],
													"source" : [ "obj-26", 2 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-20", 0 ],
													"midpoints" : [ 84.5, 150.0, 58.5, 150.0 ],
													"source" : [ "obj-27", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-84", 0 ],
													"midpoints" : [ 340.5, 482.0, 58.5, 482.0 ],
													"source" : [ "obj-3", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-3", 0 ],
													"source" : [ "obj-4", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-23", 0 ],
													"midpoints" : [ 208.5, 72.0, 208.5, 72.0 ],
													"source" : [ "obj-5", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"midpoints" : [ 252.5, 472.5, 252.5, 472.5 ],
													"source" : [ "obj-63", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-13", 0 ],
													"midpoints" : [ 508.5, 401.0, 358.5, 401.0 ],
													"source" : [ "obj-73", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-10", 0 ],
													"source" : [ "obj-8", 0 ]
												}

											}
 ],
										"styles" : [ 											{
												"name" : "max6box",
												"default" : 												{
													"accentcolor" : [ 0.8, 0.839216, 0.709804, 1.0 ],
													"bgcolor" : [ 1.0, 1.0, 1.0, 0.5 ],
													"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
												}
,
												"parentstyle" : "",
												"multi" : 0
											}
, 											{
												"name" : "max6inlet",
												"default" : 												{
													"color" : [ 0.423529, 0.372549, 0.27451, 1.0 ]
												}
,
												"parentstyle" : "",
												"multi" : 0
											}
, 											{
												"name" : "max6message",
												"default" : 												{
													"bgfillcolor" : 													{
														"type" : "gradient",
														"color1" : [ 0.866667, 0.866667, 0.866667, 1.0 ],
														"color2" : [ 0.788235, 0.788235, 0.788235, 1.0 ],
														"color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
														"angle" : 270.0,
														"proportion" : 0.39,
														"autogradient" : 0
													}
,
													"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
												}
,
												"parentstyle" : "max6box",
												"multi" : 0
											}
, 											{
												"name" : "max6outlet",
												"default" : 												{
													"color" : [ 0.0, 0.454902, 0.498039, 1.0 ]
												}
,
												"parentstyle" : "",
												"multi" : 0
											}
 ],
										"toolbaradditions" : [ "transport", "audiosolo", "audiomute" ],
										"toolbarexclusions" : [ "audiopowerctrl", "browsebeap", "browsevizzie", "lessonbrowser" ]
									}
,
									"patching_rect" : [ 131.25, 252.0, 97.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p active_colours"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-4",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 49.0, 49.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-7",
									"index" : 2,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 131.25, 336.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-15",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 72.25, 336.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-19",
									"index" : 3,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 209.25, 336.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-17", 1 ],
									"midpoints" : [ 257.75, 145.500007000000011, 212.75, 145.500007000000011 ],
									"order" : 0,
									"source" : [ "obj-13", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 257.75, 318.5, 140.75, 318.5 ],
									"order" : 1,
									"source" : [ "obj-13", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-20", 0 ],
									"midpoints" : [ 140.75, 204.000014999999991, 140.75, 204.000014999999991 ],
									"source" : [ "obj-17", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-31", 0 ],
									"midpoints" : [ 108.0, 126.000007999999994, 108.0, 126.000007999999994 ],
									"source" : [ "obj-18", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-85", 1 ],
									"midpoints" : [ 58.5, 241.5, 179.75, 241.5 ],
									"source" : [ "obj-18", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-85", 0 ],
									"midpoints" : [ 140.75, 243.000008000000008, 140.75, 243.000008000000008 ],
									"source" : [ "obj-20", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-17", 0 ],
									"midpoints" : [ 140.75, 165.000014999999991, 140.75, 165.000014999999991 ],
									"source" : [ "obj-31", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-85", 2 ],
									"midpoints" : [ 108.0, 204.000008000000008, 218.75, 204.000008000000008 ],
									"source" : [ "obj-31", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-18", 0 ],
									"midpoints" : [ 58.5, 87.0, 58.5, 87.0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 179.75, 304.5, 81.75, 304.5 ],
									"source" : [ "obj-85", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-19", 0 ],
									"midpoints" : [ 218.75, 304.5, 218.75, 304.5 ],
									"source" : [ "obj-85", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 140.75, 304.5, 140.75, 304.5 ],
									"source" : [ "obj-85", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 432.5, 152.75, 165.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p state"
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
					"id" : "obj-131",
					"lockeddragscroll" : 0,
					"lockedsize" : 0,
					"maxclass" : "bpatcher",
					"name" : "hirt_file_picker.maxpat",
					"numinlets" : 2,
					"numoutlets" : 1,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "" ],
					"patching_rect" : [ 123.5, 302.75, 365.0, 52.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 0.0, 175.0, 360.0, 50.0 ],
					"varname" : "IR Choice",
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-10",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 2,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 385.0, 45.0, 628.0, 599.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 1,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 2,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 2,
						"bottomtoolbarpinned" : 2,
						"toolbars_unpinned_last_save" : 15,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 287.0, 161.5, 62.0, 22.0 ],
									"text" : "zl change"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "int" ],
									"patching_rect" : [ 49.0, 23.0, 43.0, 22.0 ],
									"text" : "t b i"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-11",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 49.0, 69.5, 100.0, 22.0 ],
									"text" : "zl reg none none"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-12",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 49.0, 116.0, 43.0, 22.0 ],
									"text" : "zl mth"
								}

							}
, 							{
								"box" : 								{
									"comment" : "From File Drops",
									"id" : "obj-35",
									"index" : 2,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 545.5, 19.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"color" : [ 0.6, 0.4, 0.2, 1.0 ],
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-33",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 347.0, 351.0, 102.0, 22.0 ],
									"text" : "s #1MuteWet"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-32",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "bang" ],
									"patching_rect" : [ 287.0, 305.0, 79.0, 22.0 ],
									"text" : "t b b"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-26",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 287.0, 247.0, 56.0, 22.0 ],
									"text" : "deferlow"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-23",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 49.0, 290.0, 62.0, 22.0 ],
									"text" : "zl change"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 406.0, 443.517455999999981, 92.0, 22.0 ],
									"text" : "s #1Reload"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-19",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 403.0, 247.0, 62.0, 22.0 ],
									"text" : "zl change"
								}

							}
, 							{
								"box" : 								{
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-47",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 180.0, 200.5, 90.0, 22.0 ],
									"text" : "r #1Reload"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-21",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 287.0, 351.0, 43.0, 22.0 ],
									"text" : "del 20"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-8",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "" ],
									"patching_rect" : [ 287.0, 200.5, 115.0, 22.0 ],
									"text" : "t b s"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"patching_rect" : [ 287.0, 405.0, 211.0, 22.0 ],
									"text" : "hirt_file_loading #1",
									"varname" : "IR File Loading 1"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-149",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 49.0, 200.5, 70.0, 22.0 ],
									"text" : "zl reg none"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-132",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 49.0, 336.0, 105.0, 22.0 ],
									"text" : "prepend filepicker"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 287.0, 443.517455999999981, 81.0, 22.0 ],
									"text" : "prepend load"
								}

							}
, 							{
								"box" : 								{
									"comment" : "From File Drops",
									"id" : "obj-6",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 287.0, 19.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "To filepicker",
									"id" : "obj-7",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 49.0, 529.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "Control Out",
									"id" : "obj-9",
									"index" : 2,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 287.0, 529.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-13",
									"linecount" : 7,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 410.5, 161.5, 150.0, 114.0 ],
									"text" : "<< zl change used to be below\n|\n|\n|\n|\n|\n"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"destination" : [ "obj-8", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-11", 0 ],
									"hidden" : 1,
									"midpoints" : [ 58.5, 56.75, 58.5, 56.75 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-12", 1 ],
									"hidden" : 1,
									"midpoints" : [ 82.5, 80.0, 82.5, 80.0 ],
									"source" : [ "obj-10", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-12", 0 ],
									"hidden" : 1,
									"midpoints" : [ 58.5, 103.25, 58.5, 103.25 ],
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 58.5, 495.5, 58.5, 495.5 ],
									"source" : [ "obj-132", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-23", 0 ],
									"source" : [ "obj-149", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-4", 0 ],
									"midpoints" : [ 488.5, 434.758728000000019, 415.5, 434.758728000000019 ],
									"source" : [ "obj-2", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"source" : [ "obj-21", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-132", 0 ],
									"midpoints" : [ 58.5, 323.5, 58.5, 323.5 ],
									"source" : [ "obj-23", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-32", 0 ],
									"midpoints" : [ 296.5, 279.0, 296.5, 279.0 ],
									"source" : [ "obj-26", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-21", 0 ],
									"midpoints" : [ 296.5, 338.5, 296.5, 338.5 ],
									"source" : [ "obj-32", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-33", 0 ],
									"midpoints" : [ 356.5, 338.5, 356.5, 338.5 ],
									"source" : [ "obj-32", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 555.0, 494.0, 58.5, 494.0 ],
									"source" : [ "obj-35", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-26", 0 ],
									"midpoints" : [ 189.5, 234.25, 296.5, 234.25 ],
									"source" : [ "obj-47", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"midpoints" : [ 296.5, 496.758728000000019, 296.5, 496.758728000000019 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 296.5, 124.25, 296.5, 124.25 ],
									"order" : 0,
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"destination" : [ "obj-149", 0 ],
									"midpoints" : [ 296.5, 150.25, 58.5, 150.25 ],
									"order" : 1,
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 1 ],
									"source" : [ "obj-8", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-26", 0 ],
									"midpoints" : [ 296.5, 234.25, 296.5, 234.25 ],
									"source" : [ "obj-8", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 123.5, 263.25, 328.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p File_Handling",
					"varname" : "IR Files"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-16",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 2,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 200.0, 96.0, 734.0, 479.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 1,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 2,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 2,
						"bottomtoolbarpinned" : 2,
						"toolbars_unpinned_last_save" : 15,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "int", "int" ],
									"patching_rect" : [ 49.0, 186.0, 58.0, 22.0 ],
									"text" : "change 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 49.0, 147.0, 82.0, 22.0 ],
									"text" : "route int"
								}

							}
, 							{
								"box" : 								{
									"comment" : "IR Mode",
									"id" : "obj-16",
									"index" : 2,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 254.0, 49.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-14",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "bang" ],
									"patching_rect" : [ 254.0, 147.0, 64.0, 22.0 ],
									"text" : "t b b"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-12",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 299.0, 225.0, 29.5, 22.0 ],
									"text" : "i 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "bang", "bang", "" ],
									"patching_rect" : [ 299.0, 264.0, 347.0, 22.0 ],
									"text" : "sel 1 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 299.0, 303.0, 147.0, 22.0 ],
									"text" : "zl.reg 0.82 0.21 0.15 0.66"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-7",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 463.0, 303.0, 147.0, 22.0 ],
									"text" : "zl.reg 0.65 0.65 0.67 0.66"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 299.0, 358.0, 117.0, 22.0 ],
									"text" : "prepend definecolor"
								}

							}
, 							{
								"box" : 								{
									"comment" : "IR Mode",
									"id" : "obj-8",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 48.5, 49.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-3",
									"ignoreclick" : 1,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 254.0, 186.0, 431.0, 22.0 ],
									"text" : "definerange -1. 1., definepoint none, definethickness 1, refer #1IRdisplay"
								}

							}
, 							{
								"box" : 								{
									"comment" : "Control In",
									"id" : "obj-13",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 254.0, 412.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 472.5, 294.0, 472.5, 294.0 ],
									"source" : [ "obj-10", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"midpoints" : [ 308.5, 294.0, 308.5, 294.0 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 0 ],
									"midpoints" : [ 308.5, 255.0, 308.5, 255.0 ],
									"source" : [ "obj-12", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-12", 0 ],
									"midpoints" : [ 308.5, 196.5, 308.5, 196.5 ],
									"source" : [ "obj-14", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"midpoints" : [ 263.5, 177.0, 263.5, 177.0 ],
									"source" : [ "obj-14", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-14", 0 ],
									"midpoints" : [ 263.5, 112.5, 263.5, 112.5 ],
									"source" : [ "obj-16", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-12", 0 ],
									"midpoints" : [ 121.5, 216.5, 308.5, 216.5 ],
									"source" : [ "obj-2", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-4", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-13", 0 ],
									"midpoints" : [ 263.5, 309.5, 263.5, 309.5 ],
									"source" : [ "obj-3", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-12", 0 ],
									"midpoints" : [ 58.5, 216.0, 308.5, 216.0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-13", 0 ],
									"midpoints" : [ 308.5, 395.5, 263.5, 395.5 ],
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 472.5, 341.0, 308.5, 341.0 ],
									"source" : [ "obj-7", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"midpoints" : [ 58.0, 108.5, 58.5, 108.5 ],
									"source" : [ "obj-8", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 308.5, 341.0, 308.5, 341.0 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ],
						"styles" : [ 							{
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
									"bgfillcolor" : 									{
										"type" : "gradient",
										"color1" : [ 0.866667, 0.866667, 0.866667, 1.0 ],
										"color2" : [ 0.788235, 0.788235, 0.788235, 1.0 ],
										"color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
										"angle" : 270.0,
										"proportion" : 0.39,
										"autogradient" : 0
									}
,
									"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
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
 ]
					}
,
					"patching_rect" : [ 505.5, 263.25, 60.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p Display"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-12",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 123.5, 112.75, 74.0, 22.0 ],
					"text" : "prepend set"
				}

			}
, 			{
				"box" : 				{
					"comment" : "Control Out",
					"id" : "obj-2",
					"index" : 0,
					"maxclass" : "outlet",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 432.5, 371.75, 30.0, 30.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-25",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 2,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 79.0, 170.0, 750.0, 465.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 2,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 1,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 2,
						"toptoolbarpinned" : 2,
						"righttoolbarpinned" : 2,
						"bottomtoolbarpinned" : 2,
						"toolbars_unpinned_last_save" : 15,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-2",
									"index" : 2,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 627.0, 285.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-1",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 405.0, 285.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-13",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 405.0, 175.0, 135.0, 22.0 ],
									"text" : "sprintf symout %.2f sec"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-48",
									"ignoreclick" : 1,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 405.0, 230.0, 76.0, 22.0 ],
									"text" : "setitem 0 $1"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-61",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 183.0, 120.0, 59.5, 22.0 ],
									"text" : "t l s"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-58",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 63.5, 308.0, 179.5, 22.0 ],
									"text" : "pack process #1IRdisplay s"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-57",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 63.5, 347.0, 62.0, 22.0 ],
									"text" : "irdisplay~"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-41",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "" ],
									"patching_rect" : [ 183.0, 81.0, 463.0, 22.0 ],
									"text" : "route waveform rtime"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-54",
									"ignoreclick" : 1,
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 63.5, 230.0, 104.0, 22.0 ],
									"text" : "average $1 $1 $2"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-55",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 63.5, 269.0, 68.0, 22.0 ],
									"text" : "iraverage~"
								}

							}
, 							{
								"box" : 								{
									"comment" : "Waveform Instructions",
									"id" : "obj-23",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 183.0, 34.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-5",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 125.0, 334.5, 135.0, 47.0 ],
									"text" : "(unsupported in HIRT, specific fix for hirt.convolver~ only)"
								}

							}
, 							{
								"box" : 								{
									"hidden" : 1,
									"id" : "obj-4",
									"linecount" : 5,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 67.0, 363.0, 266.0, 74.0 ],
									"text" : "|\n|\nArguments are - target buffer 1 / source buffer 1 / [target buffer 2 / source buffer 2] / [multiplier 1] / [multplier 2]"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-48", 0 ],
									"midpoints" : [ 414.5, 206.75, 414.5, 206.75 ],
									"source" : [ "obj-13", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-41", 0 ],
									"source" : [ "obj-23", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-13", 0 ],
									"source" : [ "obj-41", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"source" : [ "obj-41", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-61", 0 ],
									"source" : [ "obj-41", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-48", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-55", 0 ],
									"source" : [ "obj-54", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-58", 0 ],
									"source" : [ "obj-55", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-57", 0 ],
									"source" : [ "obj-58", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-54", 0 ],
									"midpoints" : [ 192.5, 185.5, 73.0, 185.5 ],
									"source" : [ "obj-61", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-58", 2 ],
									"source" : [ "obj-61", 1 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 277.5, 112.75, 174.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p display_and_time"
				}

			}
, 			{
				"box" : 				{
					"comment" : "Control In",
					"id" : "obj-22",
					"index" : 0,
					"maxclass" : "inlet",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 432.5, 30.5, 30.0, 30.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-57",
					"justification" : 1,
					"linecolor" : [ 0.380392, 0.380392, 0.4, 0.8 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 7.0, 100.0, 62.0, 5.0 ],
					"saved_attribute_attributes" : 					{
						"linecolor" : 						{
							"expression" : ""
						}

					}

				}

			}
, 			{
				"box" : 				{
					"id" : "obj-14",
					"justification" : 1,
					"linecolor" : [ 0.380392, 0.380392, 0.4, 0.8 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 7.0, 107.499992000000006, 62.0, 5.0 ],
					"saved_attribute_attributes" : 					{
						"linecolor" : 						{
							"expression" : ""
						}

					}

				}

			}
, 			{
				"box" : 				{
					"angle" : 270.0,
					"background" : 1,
					"border" : 1,
					"bordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"grad1" : [ 0.298039, 0.247059, 0.329412, 0.66 ],
					"grad2" : [ 0.298039, 0.247059, 0.329412, 0.33 ],
					"hidden" : 1,
					"id" : "obj-75",
					"maxclass" : "panel",
					"mode" : 1,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 3.0, 27.5, 70.0, 50.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 0.0, 24.0, 360.0, 152.0 ],
					"proportion" : 0.39,
					"rounded" : 0
				}

			}
, 			{
				"box" : 				{
					"angle" : 270.0,
					"background" : 1,
					"border" : 1,
					"bordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"grad1" : [ 0.298039, 0.247059, 0.329412, 0.66 ],
					"grad2" : [ 0.298039, 0.247059, 0.329412, 0.33 ],
					"hidden" : 1,
					"id" : "obj-74",
					"maxclass" : "panel",
					"mode" : 1,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 3.0, 77.5, 70.0, 50.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 0.0, 175.0, 360.0, 50.0 ],
					"proportion" : 0.39,
					"rounded" : 0
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-6",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 109.0, 162.75, 117.0, 33.0 ],
					"text" : "should we disable @decodemode ?",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-28",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 505.5, 399.75, 128.0, 20.0 ],
					"text" : "!! ignore - dev only !!",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-31",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 200.5, 68.75, 154.0, 33.0 ],
					"text" : "!! this is not [hirt.convolutionreverb~] !!"
				}

			}
, 			{
				"box" : 				{
					"angle" : 270.0,
					"background" : 1,
					"border" : 1,
					"bordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"grad1" : [ 0.298039, 0.247059, 0.329412, 0.33 ],
					"grad2" : [ 0.298039, 0.247059, 0.329412, 0.66 ],
					"hidden" : 1,
					"id" : "obj-73",
					"maxclass" : "panel",
					"mode" : 1,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 3.0, 2.5, 70.0, 25.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 0.0, 0.0, 360.0, 25.0 ],
					"proportion" : 0.611111,
					"rounded" : 0
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-51",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 124.0, 224.25, 87.0, 22.0 ],
					"text" : "hirt_filter_type"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.0 ],
					"fontsize" : 11.0,
					"frozen_box_attributes" : [ "bgcolor", "thinmode", "gridcolor", "thinto", "gridorigincolor", "thinthresh" ],
					"gridcolor" : [ 0.501961, 0.501961, 0.501961, 0.0 ],
					"gridorigincolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"id" : "obj-52",
					"ignoreclick" : 1,
					"margins" : [ 0.0, 0.0, 0.0, 0.0 ],
					"maxclass" : "plot~",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 505.5, 290.25, 202.5, 77.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 1.0, 24.5, 358.0, 148.5 ],
					"subplots" : [ 						{
							"color" : [ 0.819999992847443, 0.209999993443489, 0.150000005960464, 0.660000026226044 ],
							"thickness" : 1.0,
							"point_style" : "none",
							"line_style" : "origin",
							"number_style" : "none",
							"filter" : "none",
							"domain_start" : 0.0,
							"domain_end" : 1.0,
							"domain_style" : "linear",
							"domain_markers" : [  ],
							"domain_labels" : [  ],
							"range_start" : -1.0,
							"range_end" : 1.0,
							"range_style" : "linear",
							"range_markers" : [  ],
							"range_labels" : [  ],
							"origin_x" : 0.0,
							"origin_y" : 0.0
						}
 ],
					"thinmode" : "sample",
					"thinthresh" : 10,
					"thinto" : 0.200000002980232
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"midpoints" : [ 297.0, 413.25, 94.0, 413.25 ],
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-131", 0 ],
					"midpoints" : [ 133.0, 293.5, 133.0, 293.5 ],
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-10", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"midpoints" : [ 143.0, 592.25, 72.0, 592.25, 72.0, 101.75, 133.0, 101.75 ],
					"source" : [ "obj-11", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-35", 0 ],
					"midpoints" : [ 133.0, 143.0, 133.5, 143.0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-27", 0 ],
					"midpoints" : [ 133.0, 364.75, 104.0, 364.75, 104.0, 23.25, 133.0, 23.25 ],
					"source" : [ "obj-131", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"midpoints" : [ 94.0, 551.0, 94.0, 551.0 ],
					"source" : [ "obj-15", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-52", 0 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"midpoints" : [ 189.0, 104.0, 133.0, 104.0 ],
					"source" : [ "obj-18", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"source" : [ "obj-18", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"midpoints" : [ 94.0, 511.75, 94.0, 511.75 ],
					"source" : [ "obj-19", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 1 ],
					"source" : [ "obj-20", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"source" : [ "obj-20", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-64", 0 ],
					"source" : [ "obj-20", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"midpoints" : [ 442.0, 83.625, 287.0, 83.625 ],
					"source" : [ "obj-22", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-20", 0 ],
					"source" : [ "obj-25", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-64", 0 ],
					"midpoints" : [ 287.0, 179.0, 588.0, 179.0 ],
					"source" : [ "obj-25", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"hidden" : 1,
					"midpoints" : [ 515.0, 413.25, 94.0, 413.25 ],
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-18", 0 ],
					"source" : [ "obj-27", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"source" : [ "obj-27", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"midpoints" : [ 94.5, 413.25, 94.0, 413.25 ],
					"source" : [ "obj-29", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"midpoints" : [ 28.5, 413.0, 94.0, 413.0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 1 ],
					"source" : [ "obj-30", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"midpoints" : [ 287.0, 254.25, 133.0, 254.25 ],
					"source" : [ "obj-33", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"midpoints" : [ 133.5, 214.75, 28.5, 214.75 ],
					"order" : 1,
					"source" : [ "obj-35", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-51", 1 ],
					"source" : [ "obj-35", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-51", 0 ],
					"order" : 0,
					"source" : [ "obj-35", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"source" : [ "obj-4", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"midpoints" : [ 759.5, 106.0, 809.0, 106.0 ],
					"source" : [ "obj-4", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"source" : [ "obj-4", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"midpoints" : [ 28.5, 293.25, 28.5, 293.25 ],
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"midpoints" : [ 133.5, 254.25, 133.0, 254.25 ],
					"order" : 0,
					"source" : [ "obj-51", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-29", 0 ],
					"midpoints" : [ 133.5, 254.5, 94.5, 254.5 ],
					"order" : 1,
					"source" : [ "obj-51", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-131::obj-10" : [ "Category Menu", "Category Menu", 0 ],
			"obj-131::obj-11" : [ "IR Menu", "IR Menu", 0 ],
			"obj-35" : [ "Drop IR", "live.drop", 3 ],
			"parameterbanks" : 			{

			}
,
			"inherited_shortname" : 1
		}
,
		"dependency_cache" : [ 			{
				"name" : "hirt_base_name.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_data_colls.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_check.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_loading.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_picker.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_file_set.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_filter_type.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_folder.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "hirt_loading_scheme.maxpat",
				"bootpath" : "~/Documents/Max 8/Packages/HISSTools Impulse Response Toolbox (HIRT)/patchers/HIRT_support",
				"patcherrelativepath" : "../../HIRT_support",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "iraverage~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "irdisplay~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0,
		"styles" : [ 			{
				"name" : "max6box",
				"default" : 				{
					"accentcolor" : [ 0.8, 0.839216, 0.709804, 1.0 ],
					"bgcolor" : [ 1.0, 1.0, 1.0, 0.5 ],
					"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
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
					"bgfillcolor" : 					{
						"type" : "gradient",
						"color1" : [ 0.866667, 0.866667, 0.866667, 1.0 ],
						"color2" : [ 0.788235, 0.788235, 0.788235, 1.0 ],
						"color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
						"angle" : 270.0,
						"proportion" : 0.39,
						"autogradient" : 0
					}
,
					"textcolor_inverse" : [ 0.0, 0.0, 0.0, 1.0 ]
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
		"bgcolor" : [ 1.0, 1.0, 1.0, 0.0 ]
	}

}
