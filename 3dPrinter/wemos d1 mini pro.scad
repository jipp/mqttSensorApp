t = 1.2;
tolerance = 0.98;

hull_x = 34.2*1.01;
hull_y = 25.6*1.01;
hull_z = 10.0;

$fn=36;

cableHole = true;
sensorHole = true;

module usb()
{
    cube([3+t, 12, 6+t]);
}

module reset()
{
    cube([10+t, 2+t, 5+t]);
}

module cable()
{
    cube([4, t, 2]);

}

module snapHole()
{
    cylinder(h=t, d=2);
}

module hull()
{    
    difference()
    {
        minkowski()
        {
            cube([hull_x, hull_y, hull_z-1]);
            cylinder(r=t, h=1);
        }
    
        translate([0, 0, t]) cube([hull_x, hull_y, hull_z-t]);
        
        translate([-t, hull_y/2 -8, 0]) usb();
        translate([-t, hull_y-2, 0]) reset();
        
        if (cableHole)
        {
            translate([(hull_x-4)/2, -t, (hull_z-2)/2]) cable();
        }
        
        translate([6, 0, 6]) rotate([90, 0, 0]) snapHole();
        translate([hull_x - 6, hull_y, 6]) rotate([270, 0, 0]) snapHole();
    }
}

module hole()
{
    translate([14, hull_y / 2, 0]) cylinder(h = t, r2 = 2.5, r1 = 5);
}

module holder()
{
    translate([8, hull_y / 2 - 4.35, 0]) cylinder(h = 3 + t, r = 1.5);
    translate([8, hull_y / 2 + 4.35, 0]) cylinder(h = 3 + t, r = 1.5);    
}

module border()
{
    height = 1.5;
    
    difference()
    {
        cube([hull_x*tolerance, hull_y*tolerance, height]);
        translate([t, t, 0]) cube([hull_x*tolerance-2*t, hull_y*tolerance-2*t, height]);
    }
}

module latch()
{
    w = 4;
    d = 2;
    
    cube([w, t, 7]);
    translate([w/2, 0, 5]) rotate([90, 0, 0]) scale ([1, 1, 0.5]) sphere(d=d);
}

module cover()
{
    difference()
    {
        minkowski()
        {
            cube([hull_x, hull_y, t-1]);
            cylinder(r=t, h=1);
        }
        
        if (sensorHole)
        {
            hole();
        }
    }
    
    translate([(1-tolerance)/2*hull_x,(1-tolerance)/2*hull_y, t]) border();
    
    if (sensorHole)
    {
        holder();
    }
    
    translate([(1-tolerance)/2*hull_x+8, hull_y - (1-tolerance)/2*hull_y, 0]) rotate([0, 0, 180]) latch();
    translate([hull_x-(1-tolerance)/2*hull_x - 8, (1-tolerance)/2*hull_y, 0]) latch();
}

//translate([0, 0, 0]) hull();
//translate([0, -hull_y-10, 0]) color("red", 1.0) cover();
//translate([0, hull_y, hull_z+1]) rotate([180, 0, 0]) color("red", 1.0) cover();
translate([0, 0, 0]) rotate([180, 0, 0]) color("red", 1.0) cover();