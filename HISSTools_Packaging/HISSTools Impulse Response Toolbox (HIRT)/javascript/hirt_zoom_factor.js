
inlets = 1;
outlets = 2;


function zoomfactor(z)
{
	this.patcher.message("zoomfactor", z);
}

function list(x, y)
{
	this.patcher.wind.scrollto(x, y);
	outlet(1, x, y);
	bang();
}

function bang()
{
	outlet(0, this.patcher.wind.location);
}

