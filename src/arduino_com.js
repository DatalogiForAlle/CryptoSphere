var connecting = false;
var ws;

function send(msg) {
    ws.send(msg);
}

async function connectToThing() {
    
    const response = await serverside_function("getLocalIPByUserID", userID);
    if (response) {
        return await attemptConnection(response);
    }
    else {
        return await Promise.reject("No device for user \"" + userID + "\" has been registered");
    }
}

function disconnectThing() {

    if (ws) {
        ws.close();
    }
}



function attemptConnection(ip) {

    return new Promise((resolve, reject) => {
        console.log("Attempting connectionssss -> ws://" + ip + port + "/");
        var port = "";
        console.log("Attempting connectionssss -> ws://" + ip + port + "/");

        ws = new WebSocket("ws://" + ip + port + "/" + password);

        ws.onerror = function(evt) {
            console.log("Bad request");
            popupMsg("Could not reach your CryptoSphere.");
            reject("Could not reach your CryptoSphere.");
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
            showLoggedOut();
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
                    event = new Event('connected');
                    showLoggedIn();

                    document.dispatchEvent(event);
                    d3.select("body")
                        .classed("connected", true);
                    resolve();
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
    });
}

function connectingAnimation() {
    if (connecting) {
        let loadingDots = d3.select("#loadingDots");
        if (loadingDots.text().length < 25) {
            loadingDots.text("." + loadingDots.text() + ".");
        } else {
            loadingDots.text(". Connecting .");
        }
        setTimeout(connectingAnimation, 200);
    }
}

function startConnectingAnimation() {
    connecting = true;
    d3.select("#login > *").style("display", "none");
    d3.select("#loadingDots").style("display", "visible");
    connectingAnimation();
}

function stopConnectingAnimation() {
    connecting = false;
    d3.select("#login > *").style("display", "block");
    d3.select("#loadingDots").style("display", "none");
}