<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 26/01/2018
 * Time: 20:39
 */

require_once('global_requirements.php');
require_once('twitter.php')

$db = new DB();

$jsonData = file_get_contents('php://input');
$data = json_decode($jsonData, true);

if(!(
    array_key_exists('sender', $data)
    && array_key_exists('recipient', $data)
    && array_key_exists('message', $data)
    && array_key_exists('message_intact', $data)
    && array_key_exists('puzzle_type', $data)
    && array_key_exists('puzzle_data', $data)
)) {
    exit("Supply all fields.");
}

$sender = $data['sender'];
$recipient = $data['recipient'];
$recipient_name = $recipient;

$msg = $data['message'];
$msg_intact = $data['message_intact'];
$puzzle_type = $data['puzzle_type'];
$puzzle_data = $data['puzzle_data'];
if (array_key_exists('user_access_token', $data)) {
    $userAccessToken = $data['user_access_token'];
} else {
    $userAccessToken = "";
}


if ($recipient_name == "" || strtolower($recipient_name) == "fem-tech" || strtolower($recipient_name) == "femtech" || strtolower($recipient_name) == "all" || $recipient_name == null) {
    $recipient = "all";
    $recipient_name = "all";
}

$sql = "
    INSERT INTO MESSAGES
    (
      GUID,
      RECIPIENT,
      RECIPIENT_NAME,
      SENDER,
      SENDER_NAME,
      MESSAGE,
      MESSAGE_INTACT
    )
    VALUES
    (
      :GUID,
      :RECIPIENT,
      :RECIPIENT_NAME,
      :SENDER,
      :SENDER_NAME,
      :MESSAGE,
      :MESSAGE_INTACT
    )
";

if($cmd = $db->prepare($sql)) {
    $msgGuid = getGUID();
    $cmd->bindValue(":GUID", $msgGuid);
    $cmd->bindValue(":RECIPIENT", $recipient, SQLITE3_TEXT);
    $cmd->bindValue(":RECIPIENT_NAME", $recipient_name, SQLITE3_TEXT);
    $cmd->bindValue(":SENDER", $sender, SQLITE3_TEXT);
    $cmd->bindValue(":SENDER_NAME", $sender, SQLITE3_TEXT);
    $cmd->bindValue(":MESSAGE", $msg, SQLITE3_TEXT);
    $cmd->bindValue(":MESSAGE_INTACT", $msg_intact, SQLITE3_TEXT);
    $cmd->execute();
}

if ($puzzle_type == "color") {
    $sql = "
        INSERT INTO COLOR_PUZZLES
        (
          GUID,
          COLORJSON,
          NUM_LEDS
        )
        VALUES
        (
          :GUID,
          :COLORJSON,
          :NUM_LEDS
        )
    ";

    if($cmd = $db->prepare($sql)) {
        $guid = getGUID();
        $cmd->bindValue(":GUID", $guid);
        $cmd->bindValue(":COLORJSON", $puzzle_data, SQLITE3_TEXT);
        $cmd->bindValue(":NUM_LEDS", count(json_decode($puzzle_data)), SQLITE3_INTEGER);
        $cmd->execute();

    }

    $sql = "
        INSERT INTO PUZZLES
        (
          PUZZLE_ID,
          MESSAGE_ID,
          PUZZLE_TYPE,
          NUM_COMPLETED,
          MAX_COMPLETED
        )
        VALUES 
        (
          :GUID,
          :MESSAGE_ID,
          :PUZZLE_TYPE,
          0,
          :MAX_COMPLETED
        )
    ";

    $maxCompletions = 1;
    if ($recipient == "all") {
        $maxCompletions = -1;
    }
    if($cmd = $db->prepare($sql)) {
        $cmd->bindValue(":GUID", $guid);
        $cmd->bindValue(":MESSAGE_ID", $msgGuid, SQLITE3_TEXT);
        $cmd->bindValue(":PUZZLE_TYPE", $puzzle_type, SQLITE3_TEXT);
        $cmd->bindValue(":MAX_COMPLETED", $maxCompletions, SQLITE3_TEXT);
        $cmd->execute();

    }
}
elseif ($puzzle_type == "binary") {
    $sql = "
        INSERT INTO COMBINATORIC_PUZZLES
        (
          GUID,
          LOGIC,
          TARGET,
          SOLUTION,
          NUM_LEDS,
          ENCRYPTION_LEVEL
        )
        VALUES
        (
          :GUID,
          :LOGIC,
          :TARGET,
          :SOLUTION,
          :NUM_LEDS,
          :ENCRYPTION_LEVEL
        )
    ";


    $puzzle = generatePuzzle($puzzle_data);

    if($cmd = $db->prepare($sql)) {
        $guid = getGUID();
        $cmd->bindValue(":GUID", $guid);
        $cmd->bindValue(":LOGIC", $puzzle["logic"], SQLITE3_TEXT);
        $cmd->bindValue(":TARGET", $puzzle["target"], SQLITE3_TEXT);
        $cmd->bindValue(":SOLUTION", $puzzle["solution"], SQLITE3_TEXT);
        $cmd->bindValue(":NUM_LEDS", $puzzle["num_leds"], SQLITE3_INTEGER);
        $cmd->bindValue(":ENCRYPTION_LEVEL", $puzzle_data, SQLITE3_INTEGER);
        $cmd->execute();

    }
    $sql = "
        INSERT INTO PUZZLES
        (
          PUZZLE_ID,
          MESSAGE_ID,
          PUZZLE_TYPE,
          NUM_COMPLETED,
          MAX_COMPLETED
        )
        VALUES 
        (
          :GUID,
          :MESSAGE_ID,
          :PUZZLE_TYPE,
          0,
          -1
        )
    ";


    $puzzle = generatePuzzle($puzzle_data);

    if($cmd = $db->prepare($sql)) {
        $cmd->bindValue(":GUID", $guid);
        $cmd->bindValue(":MESSAGE_ID", $msgGuid);
        $cmd->bindValue(":PUZZLE_TYPE", $puzzle_type, SQLITE3_TEXT);
        $cmd->execute();

    }
}

$data['message'] = "";
if ($recipient != "all") {
    if ($recipient_name != "") {
        $data['message'] .= "To ".$recipient_name.":\n";
    } else {
        $data['message'] .= "To ".$recipient.":\n";
    }
} else {
    $data['message'] .= "To everyone:\n";
}

$data['message'] .= $msg."\n\n"
    ."Sent by ".$sender."\n"
    ."Decode here: http://thesis.belhage.dk/start.php?guid=".$msgGuid."\n";

tweet($data['message']);

// $page_id = '227314294507484'; // Facebook page id
// if ($userAccessToken != "" && false) {
//     $graphNode = facebookRequest("POST", $page_id.'/feed', $data, $userAccessToken);
// } else {
//     $data['message'] .= "Sent by ".$sender."\n";
//     $graphNode = facebookRequest("POST", $page_id.'/feed', $data, null);
// }
// $postId = $graphNode->getField("id");

// $sql = "
//     UPDATE MESSAGES
//     SET FB_POST_ID = :FB_POST_ID
//     WHERE GUID = :GUID
// ";

// if($cmd = $db->prepare($sql)) {
//     $guid = getGUID();
//     $cmd->bindValue(":GUID", $guid);
//     $cmd->bindValue(":FB_POST_ID", $postId, SQLITE3_TEXT);
//     $cmd->execute();
// }
echo "{\"type\": \"messageSent\", \"guid\": \"".$msgGuid."\"}\n";
?>
