<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 26/01/2018
 * Time: 12:36
 */


// Taken from http://guid.us/GUID/PHP
function getGUID(){
    if (function_exists('com_create_guid')){
        return com_create_guid();
    }else{
        mt_srand((double)microtime()*10000);//optional for php 4.2.0 and up.
        $charid = strtoupper(md5(uniqid(rand(), true)));
        $hyphen = chr(45);// "-"
        $uuid = ""//chr(123)// "{"
            .substr($charid, 0, 8).$hyphen
            .substr($charid, 8, 4).$hyphen
            .substr($charid,12, 4).$hyphen
            .substr($charid,16, 4).$hyphen
            .substr($charid,20,12)
            ;//.chr(125);// "}"
        return $uuid;
    }
}

function generatePuzzle($level) {
    $maxLeds = 8;
    $minLeds = 4;

    $numLeds = $minLeds
        + ceil($level / $maxLeds * ($maxLeds - $minLeds));
    $difficulty = floor($level / $maxLeds * ($maxLeds - $minLeds));
    $logic = [];
    for ($n = 0; $n < $maxLeds; $n++) {
        $logicRow = array_fill(0, $maxLeds, 1);
        $adds = [];
        switch ($difficulty) {
            case 1:
                {
                    $adds[0] = $n;
                    $adds[1] = ($n + 1 + $numLeds) % $numLeds;
                }
                break;
            case 2:
                {
                    $adds[0] = ($n + 1 + $numLeds) % $numLeds;
                    $adds[1] = ($n - 1 + $numLeds) % $numLeds;

                }
                break;
            case 3: {
                    $adds[0] = $n;
                    $adds[1] = ($n + rand(-1, 1) + $numLeds) % $numLeds;
                    $adds[1] = ($n + rand(-1, 1) + $numLeds) % $numLeds;
                }
                break;
            case 4: {
                    $adds[0] = ($n + rand(-2, 1) + $numLeds) % $numLeds;
                    $adds[1] = ($n + rand(-1, 2) + $numLeds) % $numLeds;
                    if (rand(0, 3))
                    {
                        $adds[2] = $n;
                    }
                }
                break;
            default: {
                $adds = [$n];
            }
        }
        if ($n < $numLeds) {
            for ($i = 0; $i < $maxLeds; $i++) {
                if (in_array($i, $adds)) {
                    $logicRow[$i] = -1;
                } elseif ($i < $numLeds) {
                    $logicRow[$i] = 1;
                } else {
                    break;
                }
            }
        }
        $logic[$n] = $logicRow;
    }

    $target = array_fill(0, $maxLeds, 0);
    for ($n = 0; $n < $numLeds; $n++) {
        $target[$n] = 1;
    }

    $solution = reverseLogic($target, $logic);

    return ["solution" => json_encode($solution), "target" => json_encode($target),"logic" => json_encode($logic), "num_leds" => $numLeds];
}

function reverseLogic($a, $logic) {
    $b = $a;
    $numLeds = sizeof($a);
    for ($i = 0; $i < $numLeds; $i++) {
        for ($j = 0; $j < $numLeds; $j++) {
            $b[$i] = $b[$i] * $logic[$i][$j];
        }
    }
    return $b;
}

function applyLogic($start_state, $switches, $logic) {
    $numLeds = sizeof($start_state);
    for ($i = 0; $i < $numLeds; $i++) {
        if ($switches[$i] == -1) {
            for ($j = 0; $j < $numLeds; $j++) {
                $start_state[$j] = $start_state[$j] * $logic[$i][$j];

            }
        }
    }
    return $start_state;
}

function asciiCodesToString($str) {
    $stringArray = explode(";",$str);
    array_pop($stringArray);
    foreach ($stringArray as &$char) {
        $char = chr($char);
    }
    return implode($stringArray);
//    return strval($stringArray);
//    return vsprintf("%c", $stringArray);

}