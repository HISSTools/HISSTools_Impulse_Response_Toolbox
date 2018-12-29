// modified version of factory script for HIRT

var task = new Task(init, this);
task.schedule(100);

function init()
{
	var a = this.patcher.getnamed("basic_tab");
    if(a==null)
    {
		this.patcher.message("subpatcher_template", "<none>");
		this.patcher.message("gridonopen", 1);
		this.patcher.message("gridsize", 15., 15.);
		this.patcher.message("fontsize", 13);
		this.patcher.message("fontname", "Arial");
		this.patcher.message("toptoolbarpinned", 2);
		this.patcher.message("script", "newobject", "newobj", "@text","p basic", "@varname", "basic_tab", "@patching_rect", 10, 85, 50, 23);
		this.patcher.wind.size= [809, 766];
		var basic=this.patcher.getnamed("basic_tab");
		this.patcher.message("script", "newobject", "newobj", "@text","jsui @filename helpname.js @jsarguments " + jsarguments[1] + " @patching_rect 11. 11. 328. 58.");
		basic.subpatcher().message("wclose");
		basic.message("showontab", 1);
		basic.message("gridonopen", 1);
		basic.message("gridsize", 15., 15.);
		basic.message("fontsize", 13);
		basic.message("fontname", "Arial");
		basic.message("locked_bgcolor", 0.88, 0.88, 0.86, 1.);
		basic.message("editing_bgcolor", 0.88, 0.88, 0.86, 1.);
		basic.message("toptoolbarpinned", 2);
		basic.subpatcher().message("script", "newobject", "newobj", "@text","jsui @filename helpdetails.js @jsarguments " + jsarguments[1] + " @patching_rect 11. 11. 620. 125.");
		basic.subpatcher().message("script", "newobject", "newobj", "@text","jsui @filename helpargs.js @jsarguments " + jsarguments[1] + " @patching_rect 185. 285. 295. 70.");
//		basic.subpatcher().message("script", "newobject", "newobj", "@text","bpatcher @name hirt.credits.maxpat @patching_rect 11. 620. 390. 65.");
		basic.subpatcher().message("script", "newobject", "newobj", "@text","bpatcher @name hirt.credits.mini.maxpat @patching_rect 310 11. 321. 54.", "@varname", "hirt_credits");
		basic.subpatcher().message("script", "bringtofront", "hirt_credits");
	}
	var b = this.patcher.getnamed("q_tab");
    if (b == null)
       {
               this.patcher.message("script", "newobject", "newobj", "@text","p ?", "@varname", "q_tab", "@patching_rect", 183, 85, 45, 23);
               var q=this.patcher.getnamed("q_tab");
               q.subpatcher().message("wclose");
               q.message("showontab", 1);
       }
}

function resize(x, y)
{
	if(x==null)
	{
		this.patcher.wind.size=[809, 766];
	}
	else
	{
		this.patcher.wind.size= [x, y];
	}
}
