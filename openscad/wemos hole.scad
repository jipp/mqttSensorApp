length = 34.2 * 1.02;
width = 25.6 * 1.02;

hight = 12.0;

thickness = 1.5;
fudge = 0.8;

cylinder_fn = 18;

// case
translate([10, 0, 0])
difference()
{
    translate([0, 0, 0])
        cube([length + 2*thickness, width + 2*thickness, hight]);
    translate([thickness, thickness, thickness])
        cube([length, width, hight]);
    
    translate([0, width/2 + thickness - 6, 0])
        cube([3, 12 ,8]);
    translate([0, thickness + width - 2, 0])
        cube([10, 2 + thickness, 4 + thickness]);
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
    
        translate([5, width / 2 + thickness - 4.35, 0])
            cylinder(h = 3 + thickness + fudge, r = 1.5, $fn = cylinder_fn);
        translate([5, width / 2 + thickness + 4.35, 0])
            cylinder(h = 3 + thickness + fudge, r = 1.5, $fn = cylinder_fn);
    }
    
    translate([10, width / 2 + thickness, 0])
        cylinder(h = thickness + fudge, r2 = 2.5, r1 = 5, $fn = cylinder_fn);
}