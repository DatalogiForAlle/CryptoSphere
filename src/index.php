<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 25/01/2018
 * Time: 11:12
 */
require_once('global_requirements.php');

$db = new DB();


$encryptionLevel = "";
$encryptedMessage = "";
$puzzleJson = "";
$puzzleType = "";
$session_guid = "";
$guid = "";
$puzzle_guid = "";

if (isset($_GET["guid"]))
{
    $guid = $_GET["guid"];
    $messageData = $db->getMessageData($guid);
    if (!$puzzleData = $db->getPuzzleDataFromMessageId($guid)) {
        throw new Exception("No such puzzle!");
    };

    $puzzleType = $puzzleData["PUZZLE_TYPE"];

    $puzzle = [];
    $puzzle_guid = $puzzleData["PUZZLE_ID"];

    $puzzle["puzzle_type"] = $puzzleType;
    if ($puzzleType == "binary") {
        $session = $db->startPuzzle($puzzle_guid);
        $puzzle["start_state"] = $session['START_STATE'];
        $puzzle["logic"] = json_decode($puzzleData['LOGIC']);
        $encryptionLevel = $puzzleData["ENCRYPTION_LEVEL"];
        $session_guid = $session["GUID"];
    }

    $puzzleJson = addslashes(json_encode($puzzle));
    $encryptedMessage = $messageData["MESSAGE"];
}



?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Start</title>

    <link href="global_styling.css" rel="stylesheet" type="text/css" />
    <link href="index.css" rel="stylesheet" type="text/css" />

    <script>
        var puzzleType = "<?php echo $puzzleType; ?>";
        var encryptionLevel = "<?php echo $encryptionLevel; ?>";
        var encryptedMessage = "<?php echo $encryptedMessage; ?>";
        var puzzleJson = "<?php echo $puzzleJson; ?>";
        var session_guid = "<?php echo $session_guid; ?>";
        var message_guid = "<?php echo $guid; ?>";
        var puzzle_guid = "<?php echo $puzzle_guid; ?>";
    </script>
    <script src="https://d3js.org/d3.v4.min.js"></script>
    <script src="utility_functions.js"></script>
    <script src="login.js"></script>
    <script src="index.js"></script>
    <script src="emulator.js"></script>
</head>
<body <?php if (isset($_GET["guid"])) { echo "class=\"receiving\"";} ?> onclick="//hideActiveSearch()">
    <div id="popup"></div>
    <div id="emulator"></div>
    <div id="encryptionLevel"></div>

    <div id="login">
        <input id="userId" type="text" placeholder="User Id"/>
        <input id="password" type="password" placeholder="Password"/>
        <button id="loginButton">Login</button>
        <span id="loadingDots"></span>
    </div>
    <div id="sendingOptions">
        <div id="receiverDiv">
            <input id="receiver" type="text" onkeyup="searchPeople(this.value);" onchange="searchPeople(this.value);" onautocomplete="searchPeople(this.value);" placeholder="Receiver"/>
            <input id="receiverId" type="text"/>
            <div id="activeSearchField"></div>
        </div>
        <select id="puzzleType">
            <option value="color">
                Secret
            </option>
            <option value="binary">
                Puzzle
            </option>
        </select>
        <button id="help" onclick="displayHelp();">?</button>
    </div>

    <div id="messageAbove"><span id="messageHeader"></span><button id="copyButton" onclick="copyMessage()">Copy to clipboard</button></div>
    <textarea maxlength="140" id="message" placeholder="Type message here..."></textarea>
    <button id="sendButton" onclick="sendButton()"></button>
    <!--<button id="respondButton" onclick="">Respond</button>-->
    <button id="recentMessagesButton" onclick="recentMessages()">Recently sent messages</button>
    <?php include_once("device_connector.php")?>
    <button id="emulatorButton" onclick="startBinaryEmulator()">Don't have a cryptoSphere? Use the emulator!</button>
    <span id="userId"></span>
</body>
</html>
