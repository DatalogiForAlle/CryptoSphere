<?php
/**
 * Created by PhpStorm.
 * User: Stoffer
 * Date: 28/02/2018
 * Time: 17:40
 */


function userIdToName($userId) {
    $graphNode = facebookRequest("GET", $userId, [], null);
    $name = $graphNode->getField("name");
    if ($name) {
        return $name;
    } else {
        return $userId;
    }
}


function validateToken($token, $userid) {
    $graphNode = facebookRequest("GET", "me", [], $token);
    $id = $graphNode->getField("id");
    return $userid == $id;
}

function facebookRequest($type, $endpoint, $data, $access_token) {
    try {
        $fb = new Facebook\Facebook([
            'app_id' => '215632238985529',
            'app_secret' => '56f5150c61dc6f76fb9f3bdd7af6edbf',
            'default_graph_version' => 'v2.12',
        ]);
    } catch (Exception $e) {
        echo $e;
        exit();
    }
    if ($access_token == null) {
        $access_token = 'EAADEHcsBwTkBAEKEaqTjZBN3FGKo4HJ0SP57p6unomFrw5laAYEjgnmEv95mGkG7u99pk8mfjmuyBXWyVkZAuNsN4E86AqiD88mZAiwppuRkxwFALxsmj9tEeQWQarcsWcdDPTZCiLJqymvDaDoILSNdgTAgBv3SC6OmYSRPCwZDZD';

    }
    $page_id = '227314294507484';

    try {
        $response = null;
        if (strtolower($type) == "get") {
            $response = $fb->get("/" . $endpoint, $access_token);

        } else if (strtolower($type) == "post") {
            $response = $fb->post("/" . $endpoint, $data, $access_token);
        }
        if($response) {
            $graphNode = $response->getGraphNode();
            return $graphNode;
        }
        else {
            return false;
        }

    } catch(Facebook\Exceptions\FacebookResponseException $e) {
        echo 'Graph returned an error: '.$e->getMessage();
        return false;
    } catch(Facebook\Exceptions\FacebookSDKException $e) {
        echo 'Facebook SDK returned an error: '.$e->getMessage();
        return false;
    }
}