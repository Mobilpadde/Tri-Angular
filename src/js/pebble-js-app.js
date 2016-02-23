Pebble.addEventListener("ready", function(){
    Pebble.sendAppMessage({
		JS_READY: 1
	});
});

Pebble.addEventListener("showConfiguration", function(){
	var url = "http://104.168.165.72/pebble/config/index.html";
    Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e){
    var options = JSON.parse(decodeURIComponent(e.response));

	if(options.beautyColor){
        Pebble.sendAppMessage({
            JS_BEAUTY: parseInt(options.beautyColor, 16)
        });
    }
});
