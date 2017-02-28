'use strict';
const fs = require('fs');
const entries = require('./shybot-combined.json');
const filtered = [];

const ctrPoint = [33.85981750,-116.50096130];

if (Number.prototype.toRadians === undefined) {
    Number.prototype.toRadians = function() { return this * Math.PI / 180; };
}

/** Extend Number object with method to convert radians to numeric (signed)
 * degrees */
if (Number.prototype.toDegrees === undefined) {
    Number.prototype.toDegrees = function() { return this * 180 / Math.PI; };
}


function haversine(pt1, pt2) {
  var R = 6371e3; // metres
  var φ1 = pt1[0].toRadians();
  var φ2 = pt2[0].toRadians();
  var Δφ = (pt2[0]-pt1[0]).toRadians();
  var Δλ = (pt2[1]-pt1[1]).toRadians();

  var a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
            Math.cos(φ1) * Math.cos(φ2) *
            Math.sin(Δλ/2) * Math.sin(Δλ/2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

  return R * c;
}

let lat;
let lon;
let pt;
let values;
let lastTime = 0;
for(let i=0; i<entries.length; i++) {
  values = entries[i].value.split(',');
  pt = [parseFloat(values[2]), parseFloat(values[3])];
  if(haversine(ctrPoint, pt) < 1000
    && entries[i].created_epoch - lastTime > 30
    ) {
    filtered.push(entries[i]);
    if(values[16] == "SCANNING") {
      lastTime = entries[i].created_epoch;
    }
  }
}

fs.writeFileSync('shybot-combined-filtered.json', JSON.stringify(filtered, null, ' '));

