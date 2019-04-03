<?php
/**
 * Created by PhpStorm.
 * User: stoffer
 * Date: 2/1/18
 * Time: 10:40 AM
 */

require_once('global_requirements.php');

$db = new DB();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Start</title>

    <link href="global_styling.css" rel="stylesheet" type="text/css" />
    <link href="posts.css" rel="stylesheet" type="text/css" />
    <script src="https://d3js.org/d3.v4.min.js"></script>
</head>

<body>

    <div class="post-wrapper">
        <?php
        $cmd = $db->prepare("
        SELECT 
            MESSAGES.SENDER_NAME AS SENDER_NAME,
            MESSAGES.SENDER AS SENDER_ID,
            MESSAGES.RECIPIENT_NAME AS RECIPIENT_NAME,
            MESSAGES.RECIPIENT AS RECIPIENT_ID,
            MAX_COMPLETED,
            NUM_COMPLETED,
            PUZZLE_TYPE,
            MESSAGES.GUID AS GUID,
            MESSAGE,
            UNIX
        FROM MESSAGES
        LEFT JOIN PUZZLES
        ON PUZZLES.MESSAGE_ID = MESSAGES.GUID
        ORDER BY UNIX DESC
        ");

        $res = $cmd->execute();

        while ($row = $res->fetchArray()) {
            $disabled = $row["NUM_COMPLETED"] == $row["MAX_COMPLETED"];
            echo "
            <a class='post ".$row["PUZZLE_TYPE"]."' href='".($disabled ? "#" : "index.php?guid=".$row["GUID"])."' disabled='".($disabled ? "true" : "false")."'>
                <div class='recipient'>
                    ".$row["RECIPIENT_NAME"]."
                </div>    
                <div class='sender'>
                    ".$row["SENDER_NAME"]."
                </div>
                <div class='message'>
                    ".$row["MESSAGE"]."
                </div>
                
            </a>
            ";

        }

        ?>
    </div>
</body>