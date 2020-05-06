length = 34.2 * 1.02;
width = 25.6 * 1.02;

height = 12.0;

thickness = 1.5;
fudge = 0.8;

cylinder_fn = 18;

// case
translate([10, 0, 0])
difference()
{
    translate([0, 0, 0])
        cube([length + 2*thickness, width + 2*thickness, height]);
    translate([thickness, thickness, thickness])
        cube([length, width, height]);
    
    // usb connector
    translate([0, width/2 + thickness - 6, 0])
        cube([3, 12 ,8]);
    
    // reset button
    translate([0, thickness + width - 2, 0])
        cube([10, 2 + thickness, 4 + thickness]);
    
    // cable hole
    translate([(length - 4 * 1.02) / 2 + thickness, 0, (height - 1 * 1.02) / 2])
        cube([4.4 * 1.02, thickness, 1.1 * 1.02]);
}


// top
translate([-10 - length - 2*thickness, 0, 0])
difference()
{
    union()
    {
        translate([0, 0, 0])
            cube([length + 2*thickness, width + 2*thickness, thickness]);
        translate([thickness, thickness, 0])
            cube([length, width, thickness + fudge]);
    }    
}