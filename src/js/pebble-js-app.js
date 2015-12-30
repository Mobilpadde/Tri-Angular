Pebble.addEventListener("ready", function(){
    Pebble.sendAppMessage({
		JS_READY: 1
	});
});

Pebble.addEventListener("showConfiguration", function(){
    var url = "http://212.18.227.43:8080/pebble/tri-angular/config/";
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
