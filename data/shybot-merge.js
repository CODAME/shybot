'use strict';
const fs = require('fs');
const _ = require('lodash');
const archive = require('./shybot-archive.json');
const location = require('./shybot-location.json');
const heading = require('./shybot-heading.json');
const volts = require('./shybot-volts.json');
const mode = require('./shybot-mode.json');
const latest = require('./shybot-latest.json');

const combined = [];

var arcCur;
var headCur;
var modeCur;
var voltCur;
var head;
var sensorReadings;
for(var i=0; i<location.length; i++) {
  location[i];

  console.log(location[i].created_epoch);
  arcCur = _.find(archive, function(o) {
    const arcDate = o.created_epoch;
    const locDate = location[i].created_epoch;
    return Math.abs(arcDate - locDate) < 10;
  });
  headCur = _.find(heading, function(o) {
    const headDate = o.created_epoch;
    const locDate = location[i].created_epoch;
    return Math.abs(headDate - locDate) < 10;
  });
  modeCur = _.find(mode, function(o) {
    const modeDate = o.created_epoch;
    const locDate = location[i].created_epoch;
    return Math.abs(modeDate - locDate) < 10;
  });
  voltCur = _.find(volts, function(o) {
    const voltDate = o.created_epoch;
    const locDate = location[i].created_epoch;
    return Math.abs(voltDate - locDate) < 10;
  });

  if(arcCur && !arcCur.merged) {
    console.log('found match', arcCur.created_epoch);
    sensorReadings = arcCur.value.split(',').slice(0,-1).join(',');
    
    arcCur.value = [location[i].value,
                    headCur && headCur.value,
                    location[i].lat,
                    location[i].lon,
                    location[i].ele,
                    0,
                    0,
                    0,
                    sensorReadings,
                    voltCur && voltCur.value,
                    modeCur && modeCur.value
                    ].join(',');
    arcCur.merged = true;
    combined.push(arcCur);
  }
}
var mergeEnd = _.last(combined).created_epoch;
for(var i=archive.length-1; i>=0; i--) {
  if(archive[i].created_epoch > mergeEnd) {
    combined.push(archive[i]);
  }
}
for(var i=0; i<latest.length; i++) {
  if(latest[i].created_epoch > mergeEnd) {
    combined.push(latest[i]);
  }
}
combined.sort(function(a,b) {
  return a.created_epoch - b.created_epoch;
});
fs.writeFileSync('shybot-combined.json', JSON.stringify(combined, null, ' '));
