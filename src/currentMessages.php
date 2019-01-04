<?php
/**
 * Created by PhpStorm.
 * User: stoffer
 * Date: 2/1/18
 * Time: 10:40 AM
 */

require_once('global_requirements.php');

$db = new DB();

$cmd = $db->prepare("
SELECT *
FROM MESSAGES
");

$res = $cmd->execute();

while ($row = $res->fetchArray()) {
    echo "<a href='start.php?guid=".$row["GUID"]."'>".$row["MESSAGE"]."</a><br>";

}