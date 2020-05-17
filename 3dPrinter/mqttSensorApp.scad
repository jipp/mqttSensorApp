t = 1.2;
tolerance = 0.98;

hull_x = 34.2*1.01;
hull_y = 25.6*1.01;
hull_z = 10.0;

$fn=36;

module usb()
{
    translate([-t, hull_y/2 + t-8, 0]) cube([3, 12 ,6+t]);
}

module reset()
{
    translate([-t, hull_y-2, 0]) cube([10, 2+t, 5+t]);
}

module cable()
{
    translate([(hull_x-4)/2, -t, (hull_z-2)/2]) cube([4, t, 2]);

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
        usb();
        reset();
        cable();
    }
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

module cover()
{
    difference()
    {
        minkowski()
        {
            cube([hull_x, hull_y, t-1]);
            cylinder(r=t, h=1);
        }
    }
    
    translate([(1-tolerance)/2*hull_x,(1-tolerance)/2*hull_y, t]) border();
}

translate([0, 0, 0]) hull();
translate([0, -hull_y-10, 0]) color("red", 1.0) cover();
//translate([0, hull_y, hull_z+1]) rotate([180, 0, 0]) color("red", 1.0) cover();

