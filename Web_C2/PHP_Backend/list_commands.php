<?php
include('auth.php');
include("./inc/config.php");
require_once("./JSON.php");
$sql = "SELECT * FROM `lst_commands` WHERE 1;";  
$res = mysql_query($sql);
$nbrows = mysql_num_rows($res);
if($nbrows>0) {
    while($rec = mysql_fetch_array($res)) {
        $arr[] = $rec;
    }
    $jsonresult = json_encode($arr);
    echo '{"results":'.$jsonresult.', "total":"'.$nbrows.'"}';
} else {
    echo '({"total":"0", "results":""})';
}
die("");
?>