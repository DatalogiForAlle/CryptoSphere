<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 25/01/2018
 * Time: 12:05
 */


class DB extends SQLite3 {

    // PUBLIC PROPETIES/
    public static $db;

    // PRIVATE PROPETIES
    private static $defaultDB = "db.sqlite";
    private static $defaultPath = "";



    // PUBLIC
    public function __construct($connectionString = false, $reset = false)
    {
        if (!$connectionString) {
            $connectionString = self::$defaultDB;
        }
        if (!file_exists(self::$defaultPath . $connectionString)) {
            parent::__construct(self::$defaultPath . $connectionString);
            $this->createTables();
        } else {
            $this->open(self::$defaultPath . $connectionString);
            if ($reset) {
                $this->createTables();
            }
        }
        self::$db = $connectionString;
    }

    public function reset() {
        $this->close();
        $this->__construct(self::$db, true);
    }

    function getLocalIPByUserID($userID) {
        $sql = "
            SELECT *
            FROM DEVICES
            WHERE USER_ID = :USER_ID
        ";

        $cmd = $this->prepare($sql);
        $cmd->bindValue(":USER_ID", $userID, SQLITE3_TEXT);
        $results = $cmd->execute();
        if ($row = $results->fetchArray()) {
            return $row["IP"];
        }
        return null;
    }

    function getMessageData($guid) {
        $sql = "
            SELECT *
            FROM MESSAGES
            WHERE GUID = :GUID
        ";

        $cmd = $this->prepare($sql);
        $cmd->bindValue(":GUID", $guid, SQLITE3_TEXT);
        $results = $cmd->execute();
        if ($row = $results->fetchArray()) {
            return $row;
        }
        return null;
    }

    function getPuzzleDataFromMessageId($message_id) {
        $sql = "
            SELECT *
            FROM PUZZLES
            INNER JOIN COMBINATORIC_PUZZLES
            ON COMBINATORIC_PUZZLES.GUID = PUZZLE_ID
            WHERE MESSAGE_ID = :MESSAGE_ID
        ";

        $cmd = $this->prepare($sql);
        $cmd->bindValue(":MESSAGE_ID", $message_id, SQLITE3_TEXT);
        $res = $cmd->execute();
        if ($row = $res->fetchArray()) {
            return $row;
        }

        $sql = "
            SELECT *
            FROM PUZZLES
            INNER JOIN COLOR_PUZZLES
            ON COLOR_PUZZLES.GUID = PUZZLE_ID
            WHERE MESSAGE_ID = :MESSAGE_ID
        ";

        $cmd = $this->prepare($sql);
        $cmd->bindValue(":MESSAGE_ID", $message_id, SQLITE3_TEXT);
        $res = $cmd->execute();
        if ($row = $res->fetchArray()) {
            return $row;
        }
        return null;
    }

    function getPuzzleDataFromID($puzzle_id, $puzzle_type) {

        if ($puzzle_type == "binary") {
            $sql = "
                SELECT *
                FROM PUZZLES
                INNER JOIN COMBINATORIC_PUZZLES
                ON COMBINATORIC_PUZZLES.GUID = PUZZLE_ID
                WHERE PUZZLE_ID = :PUZZLE_ID
            ";
        }
        elseif ($puzzle_type == "color") {
            $sql = "
                SELECT *
                FROM PUZZLES
                INNER JOIN COLOR_PUZZLES
                ON COLOR_PUZZLES.GUID = PUZZLE_ID
                WHERE PUZZLE_ID = :PUZZLE_ID
            ";
        }



        $cmd = $this->prepare($sql);
        $cmd->bindValue(":PUZZLE_ID", $puzzle_id, SQLITE3_TEXT);
        $res = $cmd->execute();
        $result = null;
        if ($row = $res->fetchArray()) {
            $result = $row;
        }
        return $result;
    }

    function getPuzzleFromSession($guid) {
        $sql = "
            SELECT *
            FROM PUZZLE_SESSIONS
            JOIN COMBINATORIC_PUZZLES
            ON PUZZLE_SESSIONS.GUID = :GUID
            AND PUZZLE_SESSIONS.PUZZLE_ID = COMBINATORIC_PUZZLES.GUID
            JOIN PUZZLES
            ON PUZZLES.PUZZLE_ID = PUZZLE_SESSIONS.PUZZLE_ID
        ";

        $cmd = $this->prepare($sql);
        $cmd->bindValue(":GUID", $guid, SQLITE3_TEXT);
        $res = $cmd->execute();
        if($row = $res->fetchArray()) {
            return $row;
        }
        return null;
    }

    function startPuzzle($puzzle_id) {
        //$remote_ip = $_SERVER["REMOTE_ADDR"];

        $sql = "
            INSERT INTO PUZZLE_SESSIONS
            (
              GUID, 
              PUZZLE_ID, 
              START_STATE
            )
            VALUES 
            (
              :GUID,
              :PUZZLE_ID,
              :START_STATE
            )
        ";

        $puzzle = $this->getPuzzleDataFromID($puzzle_id, "binary");
        $numLeds = $puzzle["NUM_LEDS"];
        $logic = json_decode($puzzle["LOGIC"]);
        $start_state = array_fill(0, $numLeds, 1);
        while (array_sum($start_state) > 0) {
            $start_state = json_decode($puzzle["TARGET"]);

            for ($n = 0; $n < rand(4, 10); $n++) {
                $bit_on = rand(0, $numLeds);
                for ($i = 0; $i < $numLeds; $i++) {
                    if ($logic[$bit_on][$i] == -1) {
                        $start_state[$i] = -$start_state[$i];
                    }
                }
            }
        }

        $cmd = $this->prepare($sql);
        $sessionGuid = getGUID();
        $cmd->bindValue(":GUID", $sessionGuid);
        $cmd->bindValue(":PUZZLE_ID", $puzzle_id, SQLITE3_TEXT);
        $cmd->bindValue(":START_STATE", json_encode($start_state), SQLITE3_TEXT);
        $cmd->execute();

        return ["START_STATE" => $start_state, "GUID" => $sessionGuid];
    }

    function validateBinarySolution($guid, $submitted) {
        //$submitted = $submitted;

        $puzzle_session = $this->getPuzzleFromSession($guid);

        $start_state = json_decode($puzzle_session["START_STATE"]);
        $logic = json_decode($puzzle_session["LOGIC"]);
        $target = json_decode($puzzle_session["TARGET"]);
        $solution = json_decode($puzzle_session["SOLUTION"]);

        $result_state = applyLogic($start_state, $submitted, $logic);

        if (sizeof($result_state) != sizeof($target)) {
            return "false";
        }
        for ($n = 0; $n < sizeof($target); $n++) {
            if ($result_state[$n] != $target[$n]) {
                return "false";
            }
        }

        $success = $this->setSolutionCompleted($puzzle_session["PUZZLE_ID"]);
        if ($success) {
            $messageData = $this->getMessageData($puzzle_session["MESSAGE_ID"]);
            return $messageData["MESSAGE_INTACT"];
        }

        return "false";

    }

    function validateColorSolution($guid, $submitted) {
        //$submitted = $submitted;

        $puzzle = $this->getPuzzleDataFromMessageId($guid);

        $solution = json_decode($puzzle["COLORJSON"]);
        $errMargin = 50;
        if (count($solution) != count($submitted)) {
            return "false";
        }
        for ($n = 0; $n < count($submitted); $n++) {
            if (
                $submitted[$n][0] < $solution[$n][0] - $errMargin ||
                $submitted[$n][0] > $solution[$n][0] + $errMargin ||
                $submitted[$n][1] < $solution[$n][1] - $errMargin ||
                $submitted[$n][1] > $solution[$n][1] + $errMargin ||
                $submitted[$n][2] < $solution[$n][2] - $errMargin ||
                $submitted[$n][2] > $solution[$n][2] + $errMargin
            ) {
                return "false";
            }
        }


        $success = $this->setSolutionCompleted($puzzle["GUID"]);
        if ($success) {
            $messageData = $this->getMessageData($guid);
            return $messageData["MESSAGE_INTACT"];
        }

        return "false";
    }

    function setSolutionCompleted($guid) {
        $sql = "
            UPDATE PUZZLES
            SET NUM_COMPLETED = NUM_COMPLETED + 1
            WHERE PUZZLE_ID = :GUID AND (MAX_COMPLETED = -1 OR NUM_COMPLETED < MAX_COMPLETED)
        ";

        $cmd = $this->prepare($sql);
        $cmd->bindValue(":GUID", $guid, SQLITE3_TEXT);
        $cmd->execute();
        if ($this->changes() > 0) {
            return true;
        }
        return false;
    }

    function getMessageAsPosted($messageId) {
        if ($messageData = $this->getMessageData($messageId)) {
            $asPosted = $messageData["MESSAGE"]."\n\n"
                //."Sent by ".$messageData["SENDER"]."\n"
                ."Decode here: http://datanauterne.dk/cryptosphere/start.php?guid=".$messageId;
            return $asPosted;
        }
        return null;
    }

    function getRecentMessages($accessToken, $userId) {
        if (validateToken($accessToken, $userId)) {
            $sql = "
                SELECT 
                  MESSAGES.RECIPIENT_NAME AS RECIPIENT,
                  PUZZLES.PUZZLE_TYPE AS PUZZLE_TYPE,
                  COMBINATORIC_PUZZLES.ENCRYPTION_LEVEL AS ENCRYPTION_LEVEL,
                  COLOR_PUZZLES.COLORJSON AS COLORJSON
                FROM MESSAGES
                LEFT JOIN PUZZLES
                ON PUZZLES.MESSAGE_ID = MESSAGES.GUID
                LEFT JOIN COLOR_PUZZLES
                ON COLOR_PUZZLES.GUID = PUZZLES.PUZZLE_ID
                LEFT JOIN COMBINATORIC_PUZZLES
                ON COMBINATORIC_PUZZLES.GUID = PUZZLES.PUZZLE_ID
                WHERE SENDER = :USERID
                LIMIT 5
            ";

            $cmd = $this->prepare($sql);
            $cmd->bindValue(":USERID", $userId, SQLITE3_TEXT);
            $results = $cmd->execute();
            $data = [];
            while ($row = $results->fetchArray()) {
                $row["RECIPIENT"] = userIdToName($row["RECIPIENT"]);
                $data[] = $row;
            }
            return $data;
        }
        return "Invalid access token or user-id: " . $accessToken . " || " . $userId;
    }

    // PRIVATE
    private function createTables() {
        $sql = "
            DROP TABLE IF EXISTS TEST;
            DROP TABLE IF EXISTS DEVICES;
            DROP TABLE IF EXISTS MESSAGES;
            DROP TABLE IF EXISTS PUZZLES;
            DROP TABLE IF EXISTS COMBINATORIC_PUZZLES;
            DROP TABLE IF EXISTS PUZZLE_SESSIONS;
            DROP TABLE IF EXISTS COLOR_PUZZLES;
            
            CREATE TABLE DEVICES (
                USER_ID,
                IP
            );
            CREATE UNIQUE INDEX DEVICES_IDX1
            ON DEVICES (USER_ID);
            
            CREATE TABLE MESSAGES (
                GUID,
                RECIPIENT,
                RECIPIENT_NAME,
                SENDER,
                SENDER_NAME,
                MESSAGE,
                MESSAGE_INTACT,
                FB_POST_ID
            );
            CREATE UNIQUE INDEX MESSAGES_IDX1
            ON MESSAGES (GUID, SENDER);
            
            CREATE TABLE PUZZLES (
                PUZZLE_ID,
                MESSAGE_ID,
                PUZZLE_TYPE,
                NUM_COMPLETED,
                MAX_COMPLETED
            );
            
            CREATE TABLE PUZZLE_SESSIONS (
                GUID,
                PUZZLE_ID,
                START_STATE
            );
            CREATE UNIQUE INDEX PUZZLE_SESSIONS_IDX1
            ON PUZZLE_SESSIONS (GUID, PUZZLE_ID);
            
            CREATE TABLE COMBINATORIC_PUZZLES (
                GUID,
                LOGIC,
                TARGET,
                SOLUTION,
                NUM_LEDS,
                ENCRYPTION_LEVEL
            );
            CREATE UNIQUE INDEX COMBINATORIC_PUZZLES_IDX1
            ON COMBINATORIC_PUZZLES (GUID);
            
            
            CREATE TABLE COLOR_PUZZLES (
                GUID,
                COLORJSON,
                NUM_LEDS
            );
            CREATE UNIQUE INDEX COLOR_PUZZLES_IDX1
            ON COLOR_PUZZLES (GUID);
        ";

        $this->exec($sql);
    }



}
