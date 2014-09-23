// Converts the lon/lat coordinates into integers for consumption by the app
function cleanCoordinate(coord) {
  return (coord * 1000000)|0;
}

Pebble.addEventListener('ready', function() {
  console.log("Ready.");
  navigator.geolocation.getCurrentPosition(
    function success(pos) {
      Pebble.sendAppMessage({
        COMMAND_ID: 1,
        POS_LON: cleanCoordinate(pos.coords.longitude),
        POS_LAT: cleanCoordinate(pos.coords.latitude)
      });
      console.log("Sent coordinates (" + pos.coords.longitude + ", " + pos.coords.latitude + ")");
    }, function error(err) {
      console.log('ERROR(' + err.code + '): ' + err.message);
    }, {
      timeout: 10000,
      maximumAge: 600000
    }
  );
});
