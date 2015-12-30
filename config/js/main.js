(function(){
    $("#submit").on("click", function(){
        var returnTo = getQueryParam("return_to", "pebblejs://close#");
        document.location = returnTo + encodeURIComponent(JSON.stringify(storeConfig()));
    });

    var loadStorage = function(){
        if(localStorage.beautyColor) $("#beautyColor").val(localStorage.beautyColor);
    }, getQueryParam = function(variable, defaultValue){
        var query = location.search.substring(1),
            vars = query.split("&");

        for(var i in vars){
            var pair = vars[i].split("=");
            if(pair[0] == variable) return decodeURIComponent(pair[1]);
        }

        return defaultValue || false;
    }, storeConfig = function(){
        var options = {
            beautyColor: $("#beautyColor").val()
        }

        localStorage.beautyColor = options.beautyColor;

        return options;
    }

    loadStorage();
}());
