t = 1.2;
tolerance = 0.98;

hull_x = 34.2*1.02;
hull_y = 25.6*1.02;
hull_z = 10.0 + t;

$fn=72;

cableHole = true;
sensorHole = true;

module usb() {
    cube([t, 12, 6]);
}

module reset() {
    cube([7+t, 2+t, 4+t]);
}

module cable() {
    cube([4, t, 2]);
}

module snapHole() {
    cylinder(h=t, d=2);
}

module hull() {
    difference() {
        minkowski() {
            cube([hull_x, hull_y, hull_z-1]);
            cylinder(r=t, h=1);
        }
    
        translate([0, 0, t]) cube([hull_x, hull_y, hull_z-t]);
        
        translate([-t, hull_y/2-6, t]) usb();
        translate([-t, hull_y-2, 0]) reset();
        
        if (cableHole) {
            translate([(hull_x-4)/2, -t, (hull_z-2)/2]) cable();
        }
        
        translate([6, 0, hull_z-2-t]) rotate([90, 0, 0]) snapHole();
        translate([hull_x-6, hull_y, hull_z-2-t]) rotate([270, 0, 0]) snapHole();

        translate([6, hull_y, hull_z-2-t]) rotate([270, 0, 0]) snapHole();
        translate([hull_x-6, 0, hull_z-2-t]) rotate([90, 0, 0]) snapHole();
    }
}

module hole() {
    cylinder(h = t, r = 3);
}

module holder() {
    cylinder(h = 3+t, r = 1.5);
    cylinder(h = 3+t, r = 1.5);    
}

module border() {
    difference() {
        cube([hull_x*tolerance, hull_y*tolerance, t]);
        translate([t, t, 0]) cube([hull_x*tolerance-2*t, hull_y*tolerance-2*t, t]);
    }
}

module latch() {
    w = 4;
    d = 2;
    
    cube([w, t, 4]);
    translate([w/2, 0, 2]) rotate([90, 0, 0]) scale ([1, 1, 0.5]) sphere(d=d);
}

module cover() {
    difference() {
        minkowski() {
            cube([hull_x, hull_y, t-1]);
            cylinder(r=t, h=1);
        }
        
        if (sensorHole) {
            translate([12, hull_y/2, 0]) hole();
        }
    }
    
    translate([(1-tolerance)/2*hull_x,(1-tolerance)/2*hull_y, t]) border();
    
    if (sensorHole) {
        translate([8, hull_y/2-4.35, 0]) holder();
        translate([8, hull_y/2+4.35, 0]) holder();
    }
    
    translate([8, hull_y-(1-tolerance)/2*hull_y, 2*t]) rotate([0, 0, 180]) latch();
    translate([hull_x-8, (1-tolerance)/2*hull_y, 2*t]) latch();

    translate([4, (1-tolerance)/2*hull_y, 2*t]) latch();
    translate([hull_x-4, hull_y-(1-tolerance)/2*hull_y, 2*t]) rotate([0, 0, 180]) latch();
}

translate([0, 0, 0]) hull();
translate([0, -hull_y-10, 0]) color("red", 1.0) cover();
//translate([0, hull_y, hull_z+t]) rotate([180, 0, 0]) color("red", 1.0) cover();
