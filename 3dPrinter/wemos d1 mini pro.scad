$fn=72;

t = 1.2;
p = 0.1;

hull_x = 34.2;
hull_y = 25.6;
hull_z = 10.0;

cableHole = false;
sensorHole = true;

cover = false;
hull = true;

module usb() {
    cube([t, 12, 6]);
}

module reset() {
    cube([7+t, 2+t, 4+t]);
}

module snapHole() {
    cylinder(h=t, d=2);
}

module cable() {
    cube([4, t, 2]);
}

module hull() {
    difference() {
        minkowski() {
            cube([hull_x, hull_y, hull_z+t/2]);
            cylinder(r=t, h=t/2);
        }
        translate([0, 0, t]) cube([hull_x, hull_y, hull_z]);
        
        translate([-t, hull_y/2-6, t]) usb();
        translate([-t, hull_y-2, 0]) reset();

        translate([6, 0, hull_z-2]) rotate([90, 0, 0]) snapHole();
        translate([hull_x-6, hull_y, hull_z-2]) rotate([270, 0, 0]) snapHole();
        translate([6, hull_y, hull_z-2]) rotate([270, 0, 0]) snapHole();
        translate([hull_x-6, 0, hull_z-2]) rotate([90, 0, 0]) snapHole();     
     
        if (cableHole) {
            translate([(hull_x-4)/2, -t, (hull_z-2+t)/2]) cable();
        }
    }
}

module border() {
    difference() {
        translate([p/2, p/2, 0]) cube([hull_x-p, hull_y-p, t]);
        translate([p/2+t, p/2+t, 0]) cube([hull_x-p-2*t, hull_y-p-2*t, t]);
    }
}

module latch() {
    w = 4;
    d = 2;
    
    cube([w, t, 4]);
    translate([w/2, 0, 2]) rotate([90, 0, 0]) scale ([1, 1, 0.5]) sphere(d=d);
}

module hole() {
    cylinder(h = t, r = 3);
}

module holder() {
    cylinder(h = 3+t, r = 1.5);
    cylinder(h = 3+t, r = 1.5);    
}

module cover() {
    difference() {
        minkowski() {
            cube([hull_x, hull_y, t/2]);
            cylinder(r=t, h=t/2);
        }
        
        if (sensorHole) {
            translate([12, hull_y/2, 0]) hole();
        }
    }
    
    translate([0,0, t]) border();
    
    translate([8, hull_y-p/2, 2*t]) rotate([0, 0, 180]) latch();
    translate([hull_x-8, p/2, 2*t]) latch();
    translate([4, p/2, 2*t]) latch();
    translate([hull_x-4, hull_y-p/2, 2*t]) rotate([0, 0, 180]) latch();

    if (sensorHole) {
        translate([8, hull_y/2-4.35, 0]) holder();
        translate([8, hull_y/2+4.35, 0]) holder();
    }
}

if (hull) {
    translate([0, 0, 0]) color("green", 1.0) hull();
    //translate([0, hull_y, hull_z+2*t]) rotate([180, 0, 0]) color("red", 1.0) cover();
}

if (cover) {
    translate([0, -hull_y-10, 0]) color("red", 1.0) cover();
}
