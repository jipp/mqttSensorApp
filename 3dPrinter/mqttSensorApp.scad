t = 1.2;
t_z = 2;
tolerance = 0.99;

hull_x = 34.2;
hull_y = 25.6;
hull_z = 12.0;

$fn=36;

module hull()
{    
    difference()
    {
        minkowski()
        {
            cube([hull_x, hull_y, hull_z-1]);
            cylinder(r=t, h=1);
        }
    
        translate([0, 0, t_z]) cube([hull_x, hull_y, hull_z-t_z]);
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

