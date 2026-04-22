<?php
    include('auth.php');
    include("./inc/config.php");
    if (isset($_GET["deleteID"]))
    {
        $sql = "DELETE FROM `lst_commands` WHERE `ID` = '".$_GET['deleteID']."';";
		mysql_query($sql);
    }
    echo '({"total":"0", "results":""})';
    die("");
?>