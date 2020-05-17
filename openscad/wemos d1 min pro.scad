tollerance = 1.02;

length = 34.2 * tollerance;
width = 25.6 * tollerance;

height = 12.0;

thickness = 1.5;
fudge = 1;

cylinder_fn = 18;

cable = false;
hole = false;

// case
translate([10, 0, 0])
difference()
{
    //
    translate([0, 0, 0])
        cube([length+2*thickness, width+2*thickness, height]);
    translate([thickness, thickness, thickness])
        cube([length, width, height]);
    
    // usb connector
    translate([0, width/2 + thickness-7, 0])
        cube([3, 12 ,8]);
    
    // reset button
    translate([0, thickness+width-2, 0])
        cube([10, 2+thickness, 6+thickness]);
    
    // cable hole
    if (cable) {
        translate([(length-4)/2+thickness, 0, (height-1)/2])
            cube([4, thickness, 2]);
    }
}


// top
translate([-10-length - 2*thickness, 0, 0])
difference()
{
    union()
    {
        tollerance = 0.02;

        translate([0, 0, 0])
            cube([length + 2*thickness, width + 2*thickness, thickness]);
        translate([thickness*(2+tollerance)/2, thickness*(2+tollerance)/2, thickness])
            cube([length-thickness*tollerance, width-thickness*tollerance, fudge]);
    
        // hole
        if (hole) {
            translate([5, width / 2 + thickness - 4.35, 0])
                cylinder(h = 3 + thickness + fudge, r = 1.5, $fn = cylinder_fn);
            translate([5, width / 2 + thickness + 4.35, 0])
                cylinder(h = 3 + thickness + fudge, r = 1.5, $fn = cylinder_fn);
        }
    }
    
    if (hole) {
        translate([10, width / 2 + thickness, 0])
            cylinder(h = thickness + fudge, r2 = 2.5, r1 = 5, $fn = cylinder_fn);
    }
}