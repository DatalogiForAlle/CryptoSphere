<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 26/01/2018
 * Time: 22:14
 */

require_once('global_requirements.php');

try {
    $function = $_GET['function'];
    $parameters = json_decode($_GET['parameters']);
} catch(Exception $e) {
    echo $e;
}

if (!is_array($parameters)) {
    $parameters = [$parameters];
}


$db = new DB();

switch ($function) {
    case "getLocalIPByUserID":
        if($res = $db->getLocalIPByUserID(...$parameters)) {
            echo $res;
        } else {
            echo "false";
        }
        break;
    case "getMessageAsPosted":
        if($res = $db->getMessageAsPosted(...$parameters)) {
            echo $res;
        } else {
            echo "false";
        }
        break;
    case "validateBinarySolution":
        if($res = $db->validateBinarySolution(...$parameters)) {
            echo $res;
        } else {
            echo "false";
        }
        break;
    case "validateColorSolution":
        if($res = $db->validateColorSolution(...$parameters)) {
            echo $res;
        } else {
            echo "false";
        }
        break;
    case "startPuzzle":
        if($res = $db->startPuzzle(...$parameters)) {
            echo json_encode($res);
        } else {
            echo "false";
        }
        break;
    case "getRecentMessages":
        if($res = $db->getRecentMessages(...$parameters)) {
            echo json_encode($res);
        } else {
            echo "false";
        }
        break;
    default:
        echo "That function is not available";
}