Pebble.addEventListener("ready", function(){
    Pebble.sendAppMessage({ READY: 1 });
    console.log("JS: Ready...");
});

Pebble.addEventListener("showConfiguration", function(){
    var url = "http://localhost:8080";
    console.log(url);

    Pebble.openURL(url);
});

Pebble.addEventListener("webviewClosed", function(e){
    var options = JSON.parse(decodeURIComponent(e.response));
    console.log(options);

    if(options.beautyColor){
        Pebble.senAppMessage({
            BEAUTY: parseInt(options.beautyColor, 16)
        });
    }
});
