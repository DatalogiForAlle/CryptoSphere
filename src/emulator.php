<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 29/01/2018
 * Time: 12:35
 */

    include_once("global_requirements.php");
    $db = new DB();
    if (!$guid = $_GET["guid"]) {
        $guid = null;
    }
    $messageData = $db->getMessageData($guid);
    $message = "";
    if ($messageData != null) {
        $message = $messageData["MESSAGE"];
    }

?>

<html>
<head>
    <title>Emulator</title>
    <link href="emulator.css" rel="stylesheet" type="text/css" />
    <link href="global_styling.css" rel="stylesheet" type="text/css" />
    <script src="https://d3js.org/d3.v4.min.js"></script>
    <script type="application/javascript">
        var encryptedMessage = <?php echo "\"".$message."\""?>;
    </script>
    <script src="utility_functions.js"></script>
    <script src="emulator.js"></script>
</head>
<body>
    <div id="message"></div>
    <div id="leds"></div>
    <div id="buttons"></div>
</body>
</html>
