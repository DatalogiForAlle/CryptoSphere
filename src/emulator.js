
var puzzleData = {};
var numLeds = 0;
var flipStates = [];
var ledStates = [];
var logic = [];

function startBinaryEmulator() {
    var emulator = d3.select("#emulator")
        .style("display", "flex")
        .style("opacity", "1");

    puzzleData = JSON.parse(puzzleJson);
    ledStates = puzzleData["start_state"];
    logic = puzzleData["logic"];
    numLeds = ledStates.length;



    flipStates = Array(numLeds).fill(1);

    emulator.selectAll(".led")
        .data(ledStates)
        .enter()
        .append("div")
        .attr("class", function(d, i) {
            if (d === 1) {
                return "led on";
            }
            else if( d === -1) {
                return "led off";
            }
            else {
                return "led disabled";
            }
        }, true)
        .on("click", function(d, i) { switchLed(i) })
}

function switchLed(led) {
    for (var n = 0; n < numLeds; n++) {
        var ledState = ledStates[n];

        if (ledState !== 0) {
            ledState = ledState * logic[led][n];
            if (ledState === 1) {
                d3.select("#emulator .led:nth-child(" + (n + 1) + ")")
                    .classed("off", false)
                    .classed("on", true);
            } else {
                d3.select("#emulator .led:nth-child(" + (n + 1) + ")")
                    .classed("off", true)
                    .classed("on", false);
            }

            ledStates[n] = ledState;
        }
    }
    flipStates[led] = flipStates[led] * -1;
    for (n = 0; n < numLeds; n++) {
        if (ledStates[n] === -1) {
            return;
        }
    }

    success();
}

function success() {
    handleSubmission(flipStates);
    d3.select("#emulator")
        .style("background-color", "lawngreen")
        .transition()
        .delay(1000)
        .duration(750)
        .on("end", function() {
            d3.select("#emulator").style("display", "none");
        })
        .style("opacity", "0");
}