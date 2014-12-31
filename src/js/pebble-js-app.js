var seconds = {};
var r_time = {};
var mConfig = {};
Pebble.addEventListener('ready', function(e) {
  	loadLocalData();
  	returnConfigToPebble();
});
Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL('http://www.dewguzzler.com/decimal_time_configurable.html');
});
Pebble.addEventListener('webviewclosed', function(e) {
  var config = JSON.parse(e.response);
  saveLocalData(config);
  returnConfigToPebble();
 });
function returnConfigToPebble() {
	Pebble.sendAppMessage({
		"seconds": parseInt(mConfig.seconds),
		"r_time": parseInt(mConfig.r_time)
	});
}

function loadLocalData() {

  mConfig.seconds = parseInt(localStorage.getItem("seconds"));
  mConfig.r_time = parseInt(localStorage.getItem("r_time"));
  console.log("load this bitch" + JSON.stringify(mConfig));

  // console.log("loadLocalData() " + JSON.stringify(mConfig));
}
function saveLocalData(config) {

  console.log("saveLocalData() " + JSON.stringify(config));
  localStorage.setItem("r_time", parseInt(config.r_time));
  localStorage.setItem("seconds", parseInt(config.seconds));
  loadLocalData();

}