<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 29/01/2018
 * Time: 19:44
 */

include_once("global_requirements.php");
$db = new DB();
if (!$guid = $_GET["guid"]) {
    $guid = null;
}

if (!$submitted = $_GET["solution"]) {
    $submitted = null;
} else {
    $submitted = json_decode($submitted);
}
$puzzle_session = $db->getPuzzleFromSession($guid);

if ($puzzle_session["COMPLETED"] == "TRUE") {
    echo "false";
    exit();
}


$start_state = json_decode($puzzle_session["START_STATE"]);
$solution = json_decode($puzzle_session["SOLUTION"]);

$actualSubmitted = $submitted;
for ($n = 0; $n < sizeof($actualSubmitted); $n++) {
    $actualSubmitted[$n] = $actualSubmitted[$n] * -1 * $start_state[$n];
    if ($actualSubmitted[$n] != $solution[$n]) {
        echo "false";
        return;
    }
}

$db->setSolutionCompleted($guid);
$messageData = $db->getMessageData($puzzle_session["MESSAGE_ID"]);
echo $messageData["MESSAGE_INTACT"];
