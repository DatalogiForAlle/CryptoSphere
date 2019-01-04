var connecting = false;
var ws;

function send(msg) {
    ws.send(msg);
}

function connectToThing() {

    var callback = function (response) {
        if (response) {
            attemptConnection(response);
        } else {
            console.log("No device for user \"" + userID + "\" has been registered")
        }
    };

    serverside_function("getLocalIPByUserID", userID , callback);
}

function disconnectThing() {

    if (ws) {
        ws.close();
    }
}



function attemptConnection(ip) {

    // if (ws) {
    //     ws.close();
    // }
    var port = "";
    console.log("Attempting connectionssss -> ws://" + ip + port + "/");

    ws = new WebSocket("ws://" + ip + port + "/" + accessToken);

    ws.onerror = function(evt) {
        console.log("Bad request");
        popupMsg("Could not reach your CryptoSphere.");
    };


    ws.onopen =function() {
        console.log("Connection open");
        d3.select("#connection")
            .classed("connected", true);

    };
    ws.onclose =function() {
        console.log("Connection closed");
        d3.select("#connection")
            .classed("connected", false);
        d3.select("body")
            .classed("connected", false);
        FB.logout(function(response){checkLoginState(response);});
    };

    ws.onmessage = function(evt) {
        var msg = evt.data;
        console.log("Message received: " + msg);
        var msgJson = JSON.parse(msg);
        console.log(msgJson);
        var msgType = msgJson["type"];
        var event;
        switch(msgType) {
            case "connected":
                getUserName();
                showFBLoggedIn();
                event = new Event('connected');
                document.dispatchEvent(event);
                d3.select("body")
                    .classed("connected", true);
                break;
            case "messageSent":
                event = new CustomEvent('messageSent', {detail: msgJson["guid"]});
                document.dispatchEvent(event);
                break;
            case "received":
                event = new Event('received');
                document.dispatchEvent(event);
                break;
            case "solution":
                handleSubmission(msgJson["solution"]);
                break;
            default:
                console.log("Unknown message type: " + msgType);
        }

    };

    window.onbeforeunload = function() {
        ws.close();
    };
}

function connectingAnimation() {
    if (connecting) {
        var button = d3.select("#fbLogin");
        if (button.text().length < 25) {
            button.text("." + button.text() + ".");
        } else {
            button.text(". Connecting .");
        }
        setTimeout(connectingAnimation, 200);
    }
}

function startConnectingAnimation() {
    connecting = true;
    connectingAnimation();
}

function stopConnectingAnimation() {
    connecting = false;
}