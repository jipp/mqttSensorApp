width = 27.5;
depth = 35;
height = 35;
t = 1.2;
h = 3;
d = 1.25;

module huelle() {
difference()
{
    translate([0, -t, -t]) cube([width+t, depth+2*t, height+2*t]);
    cube([width, depth, height]);
    translate([0, -t, 17]) cube([19, t, 8]);
}

translate([0, 0, h]) cube([width, 1, 1]);
translate([0, 0, h+1+d]) cube([width, 1, 1]);

translate([0, depth-1, h]) cube([width, 1, 1]);
translate([0, depth-1, h+1+d]) cube([width, 1, 1]);
}

huelle();
mirror() huelle();