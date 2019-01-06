<?php

require "twitteroauth/autoload.php";
require "twitter_auth_keys.php";

use Abraham\TwitterOAuth\TwitterOAuth;

function tweet($message) {
    $connection = new TwitterOAuth(CONSUMER_KEY, CONSUMER_SECRET, ACCESS_TOKEN, ACCESS_TOKEN_SECRET);
    $content = $connection->get("account/verify_credentials");
    $status = $connection->post("statuses/update", ["status" => $tweet]);
}


?>
