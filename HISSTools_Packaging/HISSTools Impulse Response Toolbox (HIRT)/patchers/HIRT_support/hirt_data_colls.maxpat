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
		"rect" : [ 108.0, 139.0, 300.0, 285.0 ],
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
		"boxes" : [ 			{
				"box" : 				{
					"coll_data" : 					{
						"count" : 4,
						"data" : [ 							{
								"key" : 1,
								"value" : [ 1, "1-1" ]
							}
, 							{
								"key" : 2,
								"value" : [ 2, "1-2" ]
							}
, 							{
								"key" : 3,
								"value" : [ 3, "2-1" ]
							}
, 							{
								"key" : 4,
								"value" : [ 4, "2-2" ]
							}
 ]
					}
,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frozen_object_attributes" : 					{
						"embed" : 1
					}
,
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 49.0, 235.7621, 198.0, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 1
					}
,
					"style" : "",
					"text" : "coll #1suffixes_numbered 1"
				}

			}
, 			{
				"box" : 				{
					"coll_data" : 					{
						"count" : 4,
						"data" : [ 							{
								"key" : 1,
								"value" : [ 1, "L-L" ]
							}
, 							{
								"key" : 2,
								"value" : [ 2, "L-R" ]
							}
, 							{
								"key" : 3,
								"value" : [ 3, "R-L" ]
							}
, 							{
								"key" : 4,
								"value" : [ 4, "R-R" ]
							}
 ]
					}
,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frozen_object_attributes" : 					{
						"embed" : 1
					}
,
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 49.0, 192.821579, 181.0, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 1
					}
,
					"style" : "",
					"text" : "coll #1suffixes_named 1"
				}

			}
, 			{
				"box" : 				{
					"coll_data" : 					{
						"count" : 2,
						"data" : [ 							{
								"key" : 1,
								"value" : [ 1, "L" ]
							}
, 							{
								"key" : 2,
								"value" : [ 2, "R" ]
							}
 ]
					}
,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frozen_object_attributes" : 					{
						"embed" : 1
					}
,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 49.0, 149.881042, 178.0, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 1
					}
,
					"style" : "",
					"text" : "coll #1suffixes_stereo 1"
				}

			}
, 			{
				"box" : 				{
					"coll_data" : 					{
						"count" : 4,
						"data" : [ 							{
								"key" : 1,
								"value" : [ 1, "M-S" ]
							}
, 							{
								"key" : 2,
								"value" : [ 2, "M-to-S" ]
							}
, 							{
								"key" : 3,
								"value" : [ 3, "S" ]
							}
, 							{
								"key" : 4,
								"value" : [ 4, "M" ]
							}
 ]
					}
,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frozen_object_attributes" : 					{
						"embed" : 1
					}
,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 49.0, 106.940521, 174.0, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 1
					}
,
					"style" : "",
					"text" : "coll #1suffixes_mono 1"
				}

			}
, 			{
				"box" : 				{
					"coll_data" : 					{
						"count" : 5,
						"data" : [ 							{
								"key" : "loadmono",
								"value" : [ 1, 1, 1, 1, 4, 1, 4 ]
							}
, 							{
								"key" : "loadstereo",
								"value" : [ 2, 1, 1, 1, 4, 2, 4 ]
							}
, 							{
								"key" : "loadquad",
								"value" : [ 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4 ]
							}
, 							{
								"key" : "loadleft",
								"value" : [ 2, 1, 1, 1, 2, 2, 2 ]
							}
, 							{
								"key" : "loadright",
								"value" : [ 2, 3, 1, 3, 4, 2, 4 ]
							}
 ]
					}
,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"frozen_object_attributes" : 					{
						"embed" : 1
					}
,
					"id" : "obj-8",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 49.0, 64.0, 187.0, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 1
					}
,
					"style" : "",
					"text" : "coll #1loadmultichannel 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 49.0, 27.0, 150.0, 20.0 ],
					"style" : "",
					"text" : "@embed data"
				}

			}
 ],
		"lines" : [  ],
		"dependency_cache" : [  ],
		"autosave" : 0
	}

}
