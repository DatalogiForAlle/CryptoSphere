<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 26/01/2018
 * Time: 12:55
 */

require_once('global_requirements.php');

$db = new DB();

$jsonData = file_get_contents('php://input');
$data = json_decode($jsonData, true);

if(!(
    array_key_exists ("localIP", $data) &&
    array_key_exists ("userID", $data)
)) {
    exit("Supply \"userID\" and \"localIP\".");
}
$userID = $data['userID'];
$localIP = $data['localIP'];




$sql = "
    REPLACE INTO DEVICES
    (
      USER_ID,
      IP
    )
    VALUES
    (
      :USER_ID,
      :IP
    )
";

if($cmd = $db->prepare($sql)) {
    $guid = getGUID();
    //$cmd->bindValue(":GUID", $guid);
    $cmd->bindValue(":USER_ID", $userID, SQLITE3_TEXT);
    $cmd->bindValue(":IP", $localIP, SQLITE3_TEXT);
    $cmd->execute();
}
