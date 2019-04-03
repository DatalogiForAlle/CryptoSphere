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

function serverside_function(fun, parameters) {
    return new Promise( (resolve, reject) => {
        var request = new XMLHttpRequest();

        request.onreadystatechange = function() {
            if (this.readyState === 4 && this.status === 200) {
                    clearTimeout(timeout);
                    resolve(this.responseText);
            }
        };

        request.open("GET", "client_handles.php?function=" + fun + "&parameters=" + JSON.stringify(parameters), true);
        request.send();

        let timeout = setTimeout(10000, () => {reject("Server timeout: client_handles.php?function=" + fun + "&parameters=" + JSON.stringify(parameters))})
    })
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