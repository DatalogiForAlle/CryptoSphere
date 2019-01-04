var accessToken = "";
var userID = "";
var userName = "";
var taggable_friends = [];

function checkLoginState() {
    FB.getLoginStatus(function(response) {
        console.log(response);
        if (response.status === "connected") {
            accessToken = response.authResponse.accessToken;
            userID = response.authResponse.userID;
            d3.select("#userId").text("Your facebookID: " + userID);
            startConnectingAnimation();
            connectToThing();
            getTaggableFriends();
        } else {
            disconnectThing();
            showFBLoggedOut();
        }
    });
}

function showFBLoggedIn() {
    stopConnectingAnimation();
    d3.select("#fbLogin")
        .html("Disconnect (" + userName + ")")
        .on("click", function() {
            FB.logout(function(response){checkLoginState(response);});
        });
    d3.select("body")
        .classed("fbConnected", true);
}

function showFBLoggedOut() {
    stopConnectingAnimation();
    d3.select("#fbLogin")
        .html("Connect with Facebook")
        .on("click", function() {
            FB.login(function(response){checkLoginState(response);}, {scope: "user_friends"});
        });
    d3.select("body")
        .classed("fbConnected", false);
}

function getTaggableFriends(next_url) {
    var httpRequest = new XMLHttpRequest();
    httpRequest.onreadystatechange = function() {
        if (httpRequest.readyState === 4) {
            console.log(httpRequest.response);
            var data = JSON.parse(httpRequest.response);

            if (data.pagin) {
                if (data.paging.previous) {
                    taggable_friends = taggable_friends.concat(data.data);
                } else {
                    taggable_friends = data.data;
                }

                if (data.paging.next) {
                    console.log("Getting next page...");
                    getTaggableFriends(data.paging.next);
                }
            } else {
                console.log("Done!");
            }

        }
    };
    if (next_url) {
        httpRequest.open("GET", next_url, true); // true for asynchronous
    } else {
        httpRequest.open("GET", "https://graph.facebook.com/v2.12/me/taggable_friends", true); // true for asynchronous

    }
    httpRequest.setRequestHeader("Authorization", "Bearer " + accessToken);
    httpRequest.send(null);
}

function searchFriends(str) {
    console.log("Searching...");
    if (str === "") {
        hideActiveSearch();
        d3.select("#receiverId").node().value = "";
        return;
    }
    var httpRequest = new XMLHttpRequest();
    httpRequest.onreadystatechange = function() {
        if (httpRequest.readyState === 4) {
            activeSearchFriends(httpRequest.responseText, 5);
        }
    };
    httpRequest.open("GET", "https://graph.facebook.com/v2.12/search?type=user&q=" + encodeURIComponent(str), true); // true for asynchronous
    httpRequest.setRequestHeader("Authorization", "Bearer " + accessToken);
    httpRequest.send(null);

}

function searchFriends_new(str) {
    if (str.length === 0) {
        d3.select("#activeSearchField").style("display", "none");
        d3.select("#activeSearchField").selectAll("span").remove();
        return;
    }
    console.log("Searching...");

    var matches = [];

    console.log(taggable_friends.length);
    for (var n = 0; n < taggable_friends.length; n++) {
        var friend = taggable_friends[n];
        if (friend.name.substr(0, str.length) === str) {
            matches.push(friend);
        }
    }

    activeSearchFriends(matches, 5);

}

function getUserName() {
    var httpRequest = new XMLHttpRequest();
    httpRequest.onreadystatechange = function() {
        if (httpRequest.readyState === 4) {
            console.log(httpRequest.responseText);
            userName = JSON.parse(httpRequest.responseText).name;
            showFBLoggedIn();
        }
    };
    httpRequest.open("GET", "https://graph.facebook.com/v2.12/me", true); // true for asynchronous
    httpRequest.setRequestHeader("Authorization", "Bearer " + accessToken);
    httpRequest.send(null);
}

function activeSearchFriends(jsonStr, limit) {
    var data = JSON.parse(jsonStr).data;
    var length = data.length;
    if (length === 0) {
        hideActiveSearch();
        d3.select("#receiverId").node().value = "";
        return;
    }

    var people = [];
    for (var n = 0; n < limit && n < length; n++) {
        people[n] = data[n];
    }
    var asf = d3.select("#activeSearchField")
        .style("display", "flex")
        .selectAll("span")
        .data(people);

    asf.enter()
        .append("span")
        .merge(asf)
        .text(function(d, i) { return d.name + " (" + d.id + ")"; })
        .on("click", function(d, i) {
            d3.select("#receiver").node().value = d.name;
            d3.select("#receiverId").node().value = d.id;
            hideActiveSearch();
        })
        .each(function(d, i) {
            if (i === 0) {
                d3.select("#receiverId").node().value = d.id;
            }
        });
    asf.exit().remove();
}

function activeSearchFriends_new(data, limit) {
    var length = data.length;
    if (length === 0) {
        hideActiveSearch();
        d3.select("#receiverId").node().value = "";
        return;
    }

    var people = [];
    for (var n = 0; n < limit && n < length; n++) {
        people[n] = data[n];
    }
    var asf = d3.select("#activeSearchField")
        .style("display", "flex")
        .selectAll("span")
        .data(people);

    asf.enter()
        .append("span")
        .merge(asf)
        .text(function(d, i) { return d.name;})
        .on("click", function(d, i) {
            d3.select("#receiver").node().value = d.name;
            d3.select("#receiverId").node().value = d.id;
            hideActiveSearch();
        })
        .each(function(d, i) {
            if (i === 0) {
                d3.select("#receiverId").node().value = d.id;
            }
        });
    asf.exit().remove();
}

function hideActiveSearch() {
    d3.select("#activeSearchField span:first-child")
        .each(function (d, i) {
            d3.select("#receiver").node().value = d.name;
        });
    d3.select("#activeSearchField").style("display", "none");
    d3.select("#activeSearchField").selectAll("span").remove();
}