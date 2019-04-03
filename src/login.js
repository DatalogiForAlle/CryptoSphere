var userID = "";

async function handleLoginState(response) {
        if (response === "true" || response === true) {
            startConnectingAnimation();
            try {
                await connectToThing();
            }
            catch (e) {
                popupMsg("Could not reach your CryptoSphere");
                disconnectThing();
                throw("Could not connect to CryptoSphere");
            }
            showLoggedIn();
            popupMsg("Connected to CryptoSphere");
            d3.select("#userId").style("display", "none");
            d3.select("#password").style("display", "none");
            return;
        } else {
            showLoggedOut();
            disconnectThing();
            popupMsg("Not logged in");
            d3.select("#userId").style("display", "");
            d3.select("#password").style("display", "");
            return;
        }
}

function showLoggedIn() {
    stopConnectingAnimation();
    d3.select("#loginButton")
        .html("logout")
        .on("click", function() {
            logout();
        });
}

function showLoggedOut() {
    stopConnectingAnimation();
    d3.select("#loginButton")
        .html("Login")
        .on("click", function() {
            login();
        });
}

async function login() {
    userID = d3.select("#userId").node().value;
    password = d3.select("#password").node().value;
    const response = await serverside_function("validateLogin", [userID, password]);
    return handleLoginState(response);
}

function logout() {
    d3.select("#userId").node().value = "";
    d3.select("#password").node().value = "";
    userID = "";
    password = "";
    handleLoginState(false);
    disconnectThing();
}
