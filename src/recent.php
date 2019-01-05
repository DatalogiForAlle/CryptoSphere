<?php

require_once('global_requirements.php');
$db = new DB();
if($res = $db->getLast20Messages()) {
    echo json_encode($res);
}

?>
