d3.select(window).on('load', init);

document.addEventListener('received', function () {
    popupMsg("Message has been loaded onto CryptoSphere!");
}, false);

document.addEventListener('messageSent', function (e) {
    popupMsg("Message has been sent!");
    d3.select("#message").node().value = "";

}, false);

function init() {
    showLoggedOut();

    if (puzzleType) {
        updateEncryptionLevel();
        displayMessage(encryptedMessage);
        d3.select("#message")
            .attr("contenteditable", "false");
        document.addEventListener('connected', function () {
            ws.send("r:" + puzzleJson);
        }, false);
        if (puzzleType === "binary") {
            d3.select("#emulatorButton")
                .style("display", "auto");
        }
        d3.select("#sendButton").text("Decrypt with CryptoSphere");
    } else {
        d3.select("#message")
            .attr("contenteditable", "true");
        d3.select("#sendButton").text("Encrypt with CryptoSphere");
    }
}

function updateEncryptionLevel() {
    d3.select("#encryptionLevel")
        .classed("level" + encryptionLevel, true);
}

async function searchPeople(str) {
    if (str.length === 0) {
        d3.select("#activeSearchField").style("display", "none");
        d3.select("#activeSearchField").selectAll("span").remove();
        return;
    }
    console.log("Searching...");

    const results = await serverside_function("searchPeople", [str]);
    activeSearchPeople(results, 5);
}

function activeSearchPeople(jsonStr, limit) {
    var data = JSON.parse(jsonStr);
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
        .text(function(d, i) { return d['USER_NAME'] + " (" + d["USER_ID"] + ")"; })
        .on("click", function(d, i) {
            console.log(d['USER_NAME']);
            d3.select("#receiver").node().value = d['USER_NAME'];
            d3.select("#receiverId").node().value = d['USER_ID'];
            hideActiveSearch();
        })
        .each(function(d, i) {
            if (i === 0) {
                d3.select("#receiverId").node().value = d["USER_ID"];
            }
        });
    asf.exit().remove();
}

function hideActiveSearch() {
    /*d3.select("#activeSearchField span:first-child")
        .each(function (d, i) {
            d3.select("#receiver").node().value = d['USER_NAME'];
        });*/
    d3.select("#activeSearchField").style("display", "none");
    d3.select("#activeSearchField").selectAll("span").remove();
}

async function handleSubmission(solution) {

    let decryptedMessage = null;

    if (puzzleType === "binary") {
        decryptedMessage = await serverside_function("validateBinarySolution", [session_guid, solution]);
    }
    else if (puzzleType === "color") {
        decryptedMessage = await serverside_function("validateColorSolution", [message_guid, solution]);
    }

    if (decryptedMessage) {
        displayMessage(decryptedMessage);
        console.log(decryptedMessage);
        send("c");
    } else {
        console.log("Wrong solution!");
        send("d");
        //resetPuzzle();
    }

}

async function resetPuzzle() {
    if (puzzleType === "binary") {
        let puzzle_session_json = await serverside_function("startPuzzle", [puzzle_guid]);
        if (puzzle_session_json) {
            console.log(puzzle_session_json);
            var puzzle_session = JSON.parse(puzzle_session_json);
            session_guid = puzzle_session["GUID"];
            var puzzle = JSON.parse(puzzleJson);
            puzzle["start_state"] = puzzle_session["START_STATE"];
            puzzleJson = JSON.stringify(puzzle);
            sendButton();
        }
    }
    else if (puzzleType === "color") {
        sendButton();
    }

}

async function recentMessages() {
    let response = await serverside_function("getRecentMessages", [userID, password]);
    if (response) {
        console.log(response);
        var messages = JSON.parse(response);
        popupMsg("", true);
        var table = d3.select("#popup")
            .append("table");

        var header = table.append("tr");

        header.append("th")
            .text("Recipient");

        header.append("th")
            .text("Encoding");

        header.append("th")
            .text("Key");

        var tableRows = table.selectAll(".dataRow")
            .data(messages)
            .enter()
            .append("tr")
            .classed("dataRow", true);


        tableRows
            .append("td")
            .html(function(d, i) {
                return d["RECIPIENT"];
            });

        tableRows
            .append("td")
            .html(function(d, i) {
                if (d["PUZZLE_TYPE"] === "color") {
                    return "Secret"
                }
                return "Puzzle";
            });

        tableRows
            .append("td")
            .html(function(d, i) {
                if (d["PUZZLE_TYPE"] === "color") {
                    var colors = JSON.parse(d["COLORJSON"]);
                    console.log(colors.length);
                    var htmlString = "<div class='colorPreviewWrap'>";
                    for (var n = 0; n < colors.length; n++) {
                        var color = colors[n];
                        // var colorIntensity = Math.sqrt(Math.pow(color[0], 2) + Math.pow(color[1], 2) + Math.pow(color[2], 2));
                        // var scaler = 1 / colorIntensity * 255;
                        var scaler = 1.0 / Math.max(...color) * 255.0;
                        htmlString += "<div class='colorPreview'" +
                            " style='background-color: rgb(" +
                            Math.round(color[0] * scaler) + ", " +
                            Math.round(color[1] * scaler) + ", " +
                            Math.round(color[2] * scaler) + ");'></div>";
                    }
                    return htmlString + "</div>";
                }
                return d["ENCRYPTION_LEVEL"];
            });
    } else {
        popupMsg("No recent messages sent by you");
    }
}

function sendButton() {

    if (encryptedMessage) {
        send("r:" + puzzleJson);
    } else {
        var msg = d3.select("#message").node().value;
        var receiver = d3.select("#receiverId").node().value;
        var receiver_name = d3.select("#receiver").node().value;
        var puzzleType = d3.select("#puzzleType").node().value;
        send("s:{\"puzzle_type\": \"" + puzzleType.toLowerCase() + "\""
            + ", \"receiver\": \""+ receiver + "\""
            + ", \"receiver_name\": \""+ receiver_name + "\""
            + ", \"message\": \"" + msg + "\"}");
    }
}

function displayHelp() {
    if (encryptedMessage) {
        if (puzzleType === "binary") {
            popupMsg(
                "<h>Reading a <b>puzzle</b>-encrypted message</h><br>" +
                "So you've received a message. But what do you do now?\n" +
                "If you have a CryptoSphere and have configured according to the guide," +
                " then all you need to do is:" +
                "<ol>" +
                "<li>Power on your CryptoSphere; If you don't have one," +
                " use the emulator and proceed from step 5.</li>" +
                "<li>Ensure that it is on the same WiFi</li>" +
                "<li>Click \"Connect with Facebook\"</li>" +
                "<li>The puzzle should now be ready on your Cryptosphere;</li>" +
                "<li>Solve the puzzle by turning all LEDs green.</li>" +
                "<li>Submit the solution by doing a long shake with the CryptoSphere</li>" +
                "<li>And then you should see the decrypted message here!</li>" +
                "</ol>" +
                "For the puzzle-encryption, you need to turn all the LEDs green." +
                " This is done by observing which LED is highlighted," +
                " and then doing a short shake to flip the LED state.<br>" +
                " When all LEDs are green you just do a long shake to submit the solution.<br>" +
                "Good luck! :)"
                , true)
        }
        else if (puzzleType === "color") {
            popupMsg(
                "<h>Reading a <b>secret</b> message</h><br>" +
                "So you've received a message. But what do you do now?<br>" +
                "If you have a CryptoSphere and have configured according to the guide," +
                " then all you need to do is:" +
                "<ol>" +
                "<li>Power on your CryptoSphere." +
                " If you don't have one, you will need to build one!</li>" +
                "<li>Ensure that it is on the same WiFi</li>" +
                "<li>Click \"Connect with Facebook\"</li>" +
                "<li>Solve the puzzle by entering the correct colors" +
                " (Which you should have agreed on with your friend)</li>" +
                "<li>Submit the solution by doing a long shake with the CryptoSphere</li>" +
                "<li>And then you should see the decrypted message here!</li>" +
                "</ol>" +
                "For the puzzle-encryption, you need to select a number of colors," +
                " that you agreed on with your friend. This is done by carefully rotating the CryptoSphere.<br>" +
                "Holding it completely still until the LED goes out, will continue to the next LED.<br>" +
                "To submit the solution perform a long shake;<br>" +
                "Note that the last LED will disappear.<br><br>" +
                "Good luck! :)"
                , true)
        }

    } else {
        popupMsg(
            "<h>Sending a message</h><br>" +
            "You want to send a message?<br>" +
            "Well, first you will need a CryptoSphere." +
            " Then all you need to do is:" +
            "<ol>" +
            "<li>Power on your CryptoSphere</li>" +
            "<li>Ensure that it is on the same WiFi</li>" +
            "<li>Click \"Connect with Facebook\"</li>" +
            "<li>Write the name of the person you want to send to in \"Receiver\"</li>" +
            "<li>Select puzzle type</li>" +
            "<li>Write the message in the big text area</li>" +
            "</ol>" +
            "If you choose \"Puzzle\"-encryption, then you just need to shake the CryptoSphere" +
            " until you reach a desired level of encryption; Then you twist it to send the message.<br><br>" +
            "For the \"Secret\"-encryption, you need to select a number of colors," +
            " that you agree on with your friend. This is done by carefully rotating the CryptoSphere.<br>" +
            "Holding it completely still until the LED goes out, will continue to the next LED.<br>" +
            "To send the message perform a long shake;<br>" +
            "Note that the last LED will disappear.<br><br>" +
            "Good luck! :)"
            , true)
    }
}

function copyMessage() {
    var messageField = document.getElementById("message");
    messageField.select();
    document.execCommand("Copy");
    popupMsg("Copied to clipboard!");
}