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
    array_key_exists ("password", $data) &&
    array_key_exists ("userID", $data) &&
    array_key_exists ("userName", $data)

)) {
    exit("Supply \"userID\", \"userName\", \"password\" and \"localIP\".");
}
$userID = $data['userID'];
$userName = $data['userName'];
$password = $data['password'];
$localIP = $data['localIP'];




$sql = "
    REPLACE INTO DEVICES
    (
      USER_ID,
      USER_NAME,
      PASSWORD,
      IP
    )
    VALUES
    (
      :USER_ID,
      :USER_NAME,
      :PASSWORD,
      :IP
    )
";

if($cmd = $db->prepare($sql)) {
    $guid = getGUID();
    $cmd->bindValue(":USER_ID", $userID, SQLITE3_TEXT);
    $cmd->bindValue(":USER_NAME", $userName, SQLITE3_TEXT);
    $cmd->bindValue(":PASSWORD", $password, SQLITE3_TEXT);
    $cmd->bindValue(":IP", $localIP, SQLITE3_TEXT);
    $cmd->execute();
}
