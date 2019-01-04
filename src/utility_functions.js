var CookiesImport = document.createElement('script');
CookiesImport.src = 'Cookies.src.js';
document.head.appendChild(CookiesImport);

function displayMessage(encryptedMessage) {
    var message = encryptedMessage;
    //var message = numericToCharacters(numericMessage);
    d3.select("#message")
        .text(message);
}

// function decrypt(encrypted) {
//     var decrypted = encrypted;
//     return decrypted;
// }

function numericToCharacters(numeric) {
    var numericArray = numeric.split(";");
    return String.fromCharCode.apply(this, numericArray);
}

function serverside_function(fun, parameters, callback = null) {
    var async = true;

    var request = new XMLHttpRequest();

    if (callback == null) {
        async = false
    }

    if (async)
    {
        request.onreadystatechange = function() {
            if (this.readyState === 4 && this.status === 200) {
                if (this.responseText !== "false") {
                    callback(this.responseText);
                }
                else
                {
                    callback(false);
                }
            }
        };
    }

    request.open("GET", "client_handles.php?function=" + fun + "&parameters=" + JSON.stringify(parameters), async);
    request.send();

    if (!async)
    {
        return request.responseText;
    }
}

function popupMsg(message, persistent = false) {
    var popUp = d3.select("#popup")
        .style("display", "block")
        .style("opacity", "1")
        .html(message);
    if (persistent) {
        d3.select("#popup")
            .on("click", function () {
                d3.select("#popup").transition()
                    .duration(750)
                    .on("end", function() {
                        d3.select("#popup").style("display", "none");
                    })
                    .style("opacity", "0");
                d3.select("#popup")
                    .on('click',null);
            })
    } else {
        popUp.transition()
            .delay(5000)
            .duration(750)
            .on("end", function() {
                d3.select("#popup").style("display", "none");
            })
            .style("opacity", "0");
    }

}